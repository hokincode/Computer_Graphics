#ifndef SCENE_INCLUDED
#define SCENE_INCLUDED
#include <unordered_map>
#include <vector>
#include <Util/geometry.h>
#include <Image/image.h>
#include "shape.h"
#include "light.h"
#include "shapeList.h"
#include "keyFrames.h"
#include "camera.h"

namespace Ray
{
	class Material;
	class Texture;
	class File;
	class KeyFrameFile;
	class Shader;
	class Vertex;

	/** This function tries to read the next directive from a stream.*/
	std::string ReadDirective( std::istream &stream );

	/** This function puts the directive back into the stream. */
	void UnreadDirective( std::istream &stream , const std::string &directive );

	/** This function tries to read the next shape in from a stream, using the specified shape factories to resolve directive with objects. */
	Shape *ReadShape( std::istream &stream , const std::unordered_map< std::string , Util::BaseFactory< Shape > * > &shapeFactories );

	/** Stores the information that is used for rendering of the entire scene */
	class GlobalSceneData
	{
	public:
		/** The camera information */
		Camera camera;

		/** The list of lights in the scene */
		std::vector< Light * > lights;

		/** The shader */
		Shader *shader;

		/** The default constructor */
		GlobalSceneData( void );

		/** The destructor */
		~GlobalSceneData( void );

		/** The set of light factories */
		static std::unordered_map< std::string , Util::BaseFactory< Light > * > LightFactories;
	};

	/** An operator for inserting the global data into a stream */
	std::ostream &operator << ( std::ostream &stream , const GlobalSceneData &data );

	/** An operator for extracting the global data from a stream */
	std::istream &operator >> ( std::istream &stream ,       GlobalSceneData &data );

	/** Stores the information that is used for rendering the contents of a spcecific ray file */
	class LocalSceneData
	{
	public:
		/** The vertex list */
		std::vector< Vertex > vertices;

		/** The list of materials */
		std::vector< Material > materials;

		/** The list of textures */
		std::vector< Texture > textures;

		/** The list of included ray files */
		std::vector< File > files;

		/** The key-frame file */
		KeyFrameFile *keyFrameFile;

		/** This templated method sets the key frame evaluator using the prescribed type of parameter */
		template< typename ParameterType >
		void setKeyFrameEvaluator( void );

		/** This method updates the current time, changing the parameter values as needed */
		void setCurrentTime( double t , int curveFit );

		/** The default constructor */
		LocalSceneData( void );

		/** The destructor */
		~LocalSceneData( void );
	};

	/** An operator for inserting the local data into a stream */
	std::ostream &operator << ( std::ostream &stream , const LocalSceneData &data );

	/** An operator for extracting the local data from a stream */
	std::istream &operator >> ( std::istream &stream ,       LocalSceneData &data );

	/** This class stores all of the information describing the geometry in a scene */
	class SceneGeometry : public Shape
	{
		/** The local data */
		LocalSceneData _localData;

		/** The root of the scene-graph */
		ShapeList _shapeList;

	public:
		/** Initializes the scene geometry, transforming property indices to pointers */
		void init( void );

		/** This templated method sets the key frame evaluator using the prescribed type of parameter */
		template< typename ParameterType >
		void setKeyFrameEvaluator( void );

		/** This method updates the current time, changing the parameter values as needed */
		void setCurrentTime( double t , int curveFit );

		///////////////////
		// Shape methods //
		///////////////////
	private:
		void _write( std::ostream &stream ) const;
		void _read( std::istream &stream );
	public:
		std::string name( void ) const { return "scene geometry"; }
		void init( const LocalSceneData &sceneData );
		void initOpenGL( void );
		void updateBoundingBox( void );
		bool processFirstIntersection( const Util::Ray3D &ray , const Util::BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const;
		int processAllIntersections( const Util::Ray3D &ray , const Util::BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const;
		void processOverlapping( const Filter &filter , const Kernel &kernel , ShapeProcessingInfo tInfo ) const;
		bool isInside( Util::Point3D p ) const;
		void drawOpenGL( GLSLProgram *glslProgram ) const;
	};

	/** This class stores all of the information read out from a .ray file.*/
	class Scene : public SceneGeometry
	{
		friend class Window;
		friend class FileInstance;
		friend std::ostream &operator << ( std::ostream & , const Scene & );
		friend std::istream &operator >> ( std::istream & ,       Scene & );

		/** The global data */
		GlobalSceneData _globalData;

	public:
		/** The base directory */
		static std::string BaseDir;

		/** This function reflects the vector v about the normal n. */
		static Util::Point3D Reflect( Util::Point3D v , Util::Point3D n );

		/** This function will refract the vector about the normal using the provided indices of refraction.
		*** The refracted vector is written into refract and a value of true is returned if the refraction succeeded (i.e. the necessary arcsin could be computed). */
		static bool Refract( Util::Point3D v , Util::Point3D n , double ir , Util::Point3D& refract );

		/** This is the function responsible for the recursive ray-tracing returning the color obtained
		*** by shooting a ray into the scene and recursing until either the recursion depth has been reached
		*** or the contribution from subsequent bounces is guaranteed to be less than the cut-off. */
		Util::Point3D getColor( Util::Ray3D ray , int rDepth , Util::Point3D cLimit , unsigned int lightSamples , unsigned int tIdx );

		/** This method ray-traces the scene and returns the computed image */
		Image::Image32 rayTrace( int width , int height , int rLimit , double cLimit , unsigned int lightSamples , bool showProgress );

		/** This method should be called (once) after an OpenGL context has been created */
		void initOpenGL( void );

		/** This method calls the necessary OpenGL commands to render the primitive. */
		void drawOpenGL( void ) const;

		/** This method ray-traces the primitive */
		bool processFirstIntersection( const Util::Ray3D &ray , const Util::BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const;
		int processAllIntersections( const Util::Ray3D &ray , const Util::BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const;
	};

	/** This operator writes a Scene object out to a stream. */
	std::ostream &operator << ( std::ostream &stream , const Scene &scene );

	/** This operator reads in a Scene object from a stream. */
	std::istream &operator >> ( std::istream &stream ,       Scene &scene );

	/** This class stores information about the scene-graph read out from a .ray file. */
	class File : public SceneGeometry
	{
		friend std::istream &operator >> ( std::istream & , File & );
		friend class FileInstance;
		friend class Scene;
	public:
		/** The name of the .ray file */
		std::string filename;
	};

	/** This operator writes a File object out to a stream. */
	std::ostream &operator << ( std::ostream &stream , const File &file );

	/** This operator reads in a File object from a stream. */
	std::istream &operator >> ( std::istream &stream ,       File &file );

	/** This class stores information about a vertex */
	class Vertex
	{
	public:
		/** The position of the vertex */
		Util::Point3D position;

		/** The normal at the vertex */
		Util::Point3D normal;

		/** The texture coordinates at the vertex */
		Util::Point2D texCoordinate;
	};

	/** This operator writes a Vertex object out to a stream. */
	std::ostream &operator << ( std::ostream &stream , const Vertex &vertex );

	/** This operator reads in a Vertex object from a stream. */
	std::istream &operator >> ( std::istream &stream ,       Vertex &vertex );

	/** If a ray intersects a shape, the shape information at the point of intersection is stored in this class. */
	class RayShapeIntersectionInfo
	{
	public:
		/** The default constructor (setting the time to intersection at Infinity) */
		RayShapeIntersectionInfo( void );

		/** The time to intersection */
		double t;

		/** The position, in world coordinates, of the intersection */
		Util::Point3D position;

		/** The normal of the shape at the point of intersection */
		Util::Point3D normal;

		/** The texture coordinates of the the shape at the point of intersection */
		Util::Point2D texture;

		/** Checks if the time to intersection of the first object is before the time to intersection of the second */
		bool operator < ( const RayShapeIntersectionInfo &iInfo ) const;

		/** Checks if the time to intersection of the first object is before the prescribed */
		bool operator < ( double t ) const;
	};

	/** This class stores surface material properties. */
	class Material
	{
		friend SceneGeometry;
		friend Scene;
		friend class File;
		friend std::ostream &operator << ( std::ostream & , const Material & );
		friend std::istream &operator >> ( std::istream & ,       Material & );
		friend std::istream &operator >> ( std::istream & ,       Scene & );

		/** The index of the texture associated with the material */
		int _texIndex;
	public:
		/** The amount of ambient light the surface transmits */
		Util::Point3D ambient;

		/** The amount of diffuse light the surface transmits */
		Util::Point3D diffuse;

		/** The amount of specular light the surface transmits */
		Util::Point3D specular;

		/** The amount of light the surface emits */
		Util::Point3D emissive;

		/** The amount of transparent light the surface transmits */
		Util::Point3D transparent;

		/** The coefficient for the specular fall-off of the material */
		double specularFallOff;

		/** The index of refraction of the material */
		double ir;

		/** A pointer to the texture associated with a surface */
		const Texture *tex;

		/** A string for storing additional material parameters specified by the user */
		std::string foo;

		/** The default constructor */
		Material( void );

		/** This method calls the OpenGL commands for setting up the material. */
		void drawOpenGL( GLSLProgram * glslProgram ) const;
	};

	/** This operator writes out a Materieal object to a stream. */
	std::ostream &operator << ( std::ostream &stream , const Material &material );

	/** This operator reads in a Materieal object from a stream. */
	std::istream &operator >> ( std::istream &stream ,       Material &material );

	/** This class stores information about a texture used for texture mapping shapes */
	class Texture
	{
		friend Scene;
		friend Material;
		friend std::ostream &operator << ( std::ostream & , const Texture & );
		friend std::istream &operator >> ( std::istream & ,       Texture & );

		/** The name of the texture file */
		std::string _filename;

		/** The image used as a texture */
		Image::Image32 _image;

		/** The texture handle for OpenGL rendering */
		GLuint _openGLHandle;
	public:
		/** This method sets up the OpenGL texture */
		void initOpenGL( void );
	};

	/** This operator writes out a Texture object to a stream. */
	std::ostream &operator << ( std::ostream &stream , const Texture &texture );

	/** This operator reads in a Texture object from a stream. */
	std::istream &operator >> ( std::istream &stream ,       Texture &texture );

	/** This class stores information about a vertex shader */
	class Shader
	{
	public:
		/** The name of the vertex shader file */
		std::string vertexShaderFilename;

		/** The name of the fragment shader file */
		std::string fragmentShaderFilename;

		/** The shader */
		GLSLProgram *glslProgram;

		/** The default constructor */
		Shader( void );

		/** The destructor */
		~Shader( void );
	};

	/** This operator writes out a Shader object to a stream. */
	std::ostream &operator << ( std::ostream &stream , const Shader &shader );

	/** This operator reads in a Shader object from a stream. */
	std::istream &operator >> ( std::istream &stream ,       Shader &shader );

	/** This class stores key-frame information associated with a .key file. */
	class KeyFrameFile
	{
	public:
		/** The name of the .key file */
		std::string filename;

		/** The key-frame data associated with a .key file */
		KeyFrameMatrices keyFrameMatrices;
	};

	/** This operator writes out a KeyFrameFile object to a stream. */
	std::ostream &operator << ( std::ostream &stream , const KeyFrameFile &keyFrameFile );

	/** This operator reads in a KeyFrameFile object from a stream. */
	std::istream &operator >> ( std::istream &stream ,       KeyFrameFile &keyFrameFile );
}

#include "scene.inl"
#endif // SCENE_INCLUDED
  

