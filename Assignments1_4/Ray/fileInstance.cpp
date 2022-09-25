#include <stdio.h>
#include "fileInstance.h"

using namespace Ray;
using namespace Util;

//////////////////
// FileInstance //
//////////////////

FileInstance::FileInstance( void ) : _file(NULL) {}

void FileInstance::_write( std::ostream &stream ) const
{
	Shape::WriteInset( stream );
	stream << "#" << Directive() << "  " << _fileIndex;
}

void FileInstance::_read( std::istream &stream )
{
	if( !( stream >> _fileIndex ) ) THROW( "failed to read ray file index" );
}

void FileInstance::init( const LocalSceneData &data )
{
	// Set the .ray file pointer
	if( _fileIndex==-1 ) THROW( "negative ray_file index: " , _fileIndex );
	if( _fileIndex>=data.files.size() ) THROW( "shape specifies a ray_file that is out of bounds: " , _fileIndex , " <= " , data.files.size() );
	_file = &data.files[ _fileIndex ];
	_primitiveNum = _file->primitiveNum();
}

void FileInstance::updateBoundingBox( void ){ _bBox = _file->boundingBox(); }

void FileInstance::initOpenGL( void ){}

bool FileInstance::processFirstIntersection( const Ray3D &ray , const BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const
{
	return _file->processFirstIntersection( ray , range , rFilter , rKernel , spInfo , tIdx );
}

int FileInstance::processAllIntersections( const Ray3D &ray , const BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const
{
	return _file->processAllIntersections( ray , range , rFilter , rKernel , spInfo , tIdx );
}

void FileInstance::processOverlapping( const Filter &filter , const Kernel &kernel , ShapeProcessingInfo spInfo ) const { _file->processOverlapping( filter , kernel , spInfo ); }

bool FileInstance::isInside( Point3D p ) const { return _file->isInside(p); }

void FileInstance::drawOpenGL( GLSLProgram * glslProgram ) const { _file->drawOpenGL( glslProgram ); }
