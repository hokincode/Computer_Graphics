#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <iostream>
#include <Util/exceptions.h>
#include "GLSLProgram.h"

/////////////////
// GLSLProgram //
/////////////////

template<> void GLSLProgram::glUniformiv< 1 >( GLint location  , GLsizei count , const GLint* value ){ glUniform1iv( location , count , value ); }

template<> void GLSLProgram::glUniformiv< 2 >( GLint location  , GLsizei count , const GLint* value ){ glUniform2iv( location , count , value ); }

template<> void GLSLProgram::glUniformiv< 3 >( GLint location  , GLsizei count , const GLint* value ){ glUniform3iv( location , count , value ); }

template<> void GLSLProgram::glUniformiv< 4 >( GLint location  , GLsizei count , const GLint* value ){ glUniform4iv( location , count , value ); }

template<> void GLSLProgram::glUniformfv< 1 >( GLint location  , GLsizei count , const GLfloat* value ){ glUniform1fv( location , count , value ); }

template<> void GLSLProgram::glUniformfv< 2 >( GLint location  , GLsizei count , const GLfloat* value ){ glUniform2fv( location , count , value ); }

template<> void GLSLProgram::glUniformfv< 3 >( GLint location  , GLsizei count , const GLfloat* value ){ glUniform3fv( location , count , value ); }

template<> void GLSLProgram::glUniformfv< 4 >( GLint location  , GLsizei count , const GLfloat* value ){ glUniform4fv( location , count , value ); }

template<> void GLSLProgram::glUniformMatrixfv< 2 >( GLint location  , GLsizei count , GLboolean transpose , const GLfloat* value )
{
	GLint id;
	glGetIntegerv( GL_CURRENT_PROGRAM , &id );
	if( !id ) WARN( "No current program" );
	else glUniformMatrix2fv( location , count , transpose , value );
}

template<> void GLSLProgram::glUniformMatrixfv< 3 >( GLint location  , GLsizei count , GLboolean transpose , const GLfloat* value )
{
	GLint id;
	glGetIntegerv( GL_CURRENT_PROGRAM , &id );
	if( !id ) WARN( "No current program" );
	else glUniformMatrix3fv( location , count , transpose , value );
}

template<> void GLSLProgram::glUniformMatrixfv< 4 >( GLint location  , GLsizei count , GLboolean transpose , const GLfloat* value )
{
	GLint id;
	glGetIntegerv( GL_CURRENT_PROGRAM , &id );
	if( !id ) WARN( "No current program" );
	else glUniformMatrix4fv( location , count , transpose , value );
}

GLSLProgram::GLSLProgram( const std::string &vs_filename , const std::string &fs_filename ) : _handle( 0 ) , _linked( false )
{
	// Check that the first shader is a vertex shader and that the file exists. If it does, read in the code.
	if( GetShaderType( vs_filename )!=GL_VERTEX_SHADER ) THROW( "Expected vertex shader: " , vs_filename );
	if( !FileExists( vs_filename ) ) THROW( "Vertex shader not found: " , vs_filename );
	{
		std::ifstream inFile( vs_filename , std::ios::in );
		if( !inFile ) THROW( "Unable to open vertex shader: " , vs_filename );
		std::stringstream code;
		code << inFile.rdbuf();
		inFile.close();
		_vertexSource = code.str();
	}

	// Check that the second shader is a fragment shader and that the file exists. If it does, read in the code.
	if( GetShaderType( fs_filename )!=GL_FRAGMENT_SHADER ) THROW( "Expected fragment shader: " , fs_filename );
	if( !FileExists( fs_filename ) ) THROW( "Fragment shader not found: " , fs_filename );
	{
		std::ifstream inFile( fs_filename , std::ios::in );
		if( !inFile ) THROW( "Unable to open fragment shader: " , fs_filename );
		std::stringstream code;
		code << inFile.rdbuf();
		inFile.close();
		_fragmentSource = code.str();
	}
}

GLSLProgram::~GLSLProgram( void )
{
	if( _handle==0 ) return;

	// Query the number of attached shaders
	GLint numShaders = 0;
	glGetProgramiv( _handle , GL_ATTACHED_SHADERS , &numShaders );

	// Get the shader names
	GLuint * shaderNames = new GLuint[numShaders];
	glGetAttachedShaders( _handle, numShaders , NULL , shaderNames );

	// Delete the shaders
	for( int i=0 ; i<numShaders ; i++ ) glDeleteShader( shaderNames[i] );

	// Delete the program
	glDeleteProgram( _handle );

	delete[] shaderNames;
}

GLenum GLSLProgram::GetShaderType( const std::string &fileName )
{
	int numExts = sizeof( GLSLShaderInfo::extensions ) / sizeof( GLSLShaderInfo::shader_file_extension );

	std::string ext = GetExtension( fileName );
	bool matchFound = false;
	for( int i=0 ; i<numExts ; i++ ) if( ext==GLSLShaderInfo::extensions[i].ext ) return GLSLShaderInfo::extensions[i].type;
	THROW( "Unrecognized extension: " , ext );
	return -1;
}

std::string GLSLProgram::GetExtension( const std::string & filename )
{
	size_t loc = filename.find_last_of( '.' );
	if( loc!=std::string::npos ) return filename.substr( loc , std::string::npos );
	return "";
}

void GLSLProgram::compileShader( const std::string & source , GLenum type )
{
	if( _handle<=0 )
	{
		_handle = glCreateProgram();
		if( _handle==0 ) THROW( "Unable to create shader program" );
	}

	GLuint shaderHandle = glCreateShader( type );

	const char * c_code = source.c_str();
	glShaderSource( shaderHandle , 1 , &c_code , NULL );

	// Compile the shader
	glCompileShader( shaderHandle );

	// Check for errors
	int result;
	glGetShaderiv( shaderHandle , GL_COMPILE_STATUS , &result );
	if( GL_FALSE==result )
	{
		// Compile failed, get log
		int length = 0;
		std::string logString;
		glGetShaderiv( shaderHandle , GL_INFO_LOG_LENGTH , &length );
		if( length>0 )
		{
			char * c_log = new char[length];
			int written = 0;
			glGetShaderInfoLog( shaderHandle , length , &written , c_log );
			logString = c_log;
			delete[] c_log;
		}
		THROW( "Shader[ " , GetShaderTypeString( type ) , " ] compilation failed: " , logString.c_str() );
	}
	else glAttachShader( _handle , shaderHandle );
}

void GLSLProgram::link( void )
{
	if( _linked ) return;
	if( _handle<=0 ) THROW( "Program has not been compiled." );

	glLinkProgram( _handle );

	int status = 0;
	glGetProgramiv( _handle , GL_LINK_STATUS , &status );
	if( GL_FALSE==status )
	{
		// Store log and return false
		int length = 0;
		std::string logString;

		glGetProgramiv( _handle , GL_INFO_LOG_LENGTH , &length );

		if( length>0 )
		{
			char * c_log = new char[length];
			int written = 0;
			glGetProgramInfoLog( _handle , length , &written , c_log );
			logString = c_log;
			delete[] c_log;
		}
		THROW( "Program link failed: " , logString );
	}
	else _linked = true;
}

void GLSLProgram::use( void )
{
	if( _handle<=0 || (!_linked) ) THROW( "Shader has not been linked" );
	glUseProgram( _handle );
}

void GLSLProgram::setUniform( const std::string &name , int val , bool showWarning )
{
	GLint loc = glGetUniformLocation( _handle , name.c_str() );
	if( loc>=0 ) glUniform1i( loc , val );
	else if( showWarning ) WARN( "Non-existant uniform: %s" , name.c_str() );
}

void GLSLProgram::setUniform( const std::string &name , float val , bool showWarning )
{
	GLint loc = glGetUniformLocation( _handle , name.c_str() );
	if( loc>=0 ) glUniform1f( loc , val );
	else if( showWarning ) WARN( "Non-existant uniform: %s" , name.c_str() );
}

void GLSLProgram::setUniform( const std::string &name , double val , bool showWarning )
{
	GLint loc = glGetUniformLocation( _handle , name.c_str() );
	if( loc>=0 ) glUniform1f( loc , (GLfloat)val );
	else if( showWarning ) WARN( "Non-existant uniform: %s" , name.c_str() );
}

void GLSLProgram::validate( void )
{
	if( !_linked ) THROW( "Program is not linked" );

	GLint status;
	glValidateProgram( _handle );
	glGetProgramiv( _handle , GL_VALIDATE_STATUS , &status );

	if( GL_FALSE==status )
	{
		// Store log and return false
		int length = 0;
		std::string logString;

		glGetProgramiv( _handle , GL_INFO_LOG_LENGTH , &length );

		if( length>0 )
		{
			char * c_log = new char[length];
			int written = 0;
			glGetProgramInfoLog( _handle , length , &written , c_log );
			logString = c_log;
			delete[] c_log;
		}
		THROW( "Program failed to validate: " , logString );
	}
}

bool GLSLProgram::FileExists( const std::string & fileName )
{
	struct stat info;
	int ret = -1;

	ret = stat( fileName.c_str() , &info );
	return 0 == ret;
}

void GLSLProgram::init( void )
{
	try
	{
		compileShader(   _vertexSource , GL_VERTEX_SHADER   );
		compileShader( _fragmentSource , GL_FRAGMENT_SHADER );
		link();
		validate();
	}
	catch( Util::Exception &e )
	{
		std::cerr << e.what() << std::endl;
		ERROR_OUT( "giving up" );
	}
}

std::string GLSLProgram::GetShaderTypeString( GLenum type )
{
	switch( type )
	{
	case GL_VERTEX_SHADER:          return "vertex";
	case GL_FRAGMENT_SHADER:        return "fragment";
	default:                        return "?";
	}
}

