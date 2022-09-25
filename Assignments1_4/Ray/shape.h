#ifndef SHAPE_INCLUDED
#define SHAPE_INCLUDED
#include <vector>
#include <stdexcept>
#include <string>
#include <functional>
#include <atomic>
#include <Util/geometry.h>
#include <Util/factory.h>
#include <GL/glew.h>
#pragma warning( disable : 4290 )
#ifdef __APPLE__
#include <GLUT/glut.h>
#else // !__APPLE__
#include <GL/glut.h>
#endif // __APPLE__
#include <Util/exceptions.h>
#include "GLSLProgram.h"

#ifdef VERBOSE_MESSAGING
inline void AssertOpenGLState( std::string fileName , int line , std::string functionName )
{
	GLenum errorCode;
	if( ( errorCode=glGetError() )!=GL_NO_ERROR )
	{
		std::cerr << Util::MakeMessageString( "[OPEN_GL ERROR]" , fileName , line , functionName , gluErrorString( errorCode ) , " (" , errorCode , ")" ) << std::endl;
		exit( 0 );
	}
}
#ifndef ASSERT_OPEN_GL_STATE
#define ASSERT_OPEN_GL_STATE( ... ) AssertOpenGLState( __FILE__ , __LINE__ , __FUNCTION__ )
#endif // ASSERT_OPEN_GL_STATE
#else // !VERBOSE_MESSAGING
inline void AssertOpenGLState( std::string functionName )
{
	GLenum errorCode;
	if( ( errorCode=glGetError() )!=GL_NO_ERROR )
	{
		std::cerr << Util::MakeMessageString( "[OPEN_GL ERROR]" , functionName , gluErrorString( errorCode ) , " (" , errorCode , ")" ) << std::endl;
		exit( 0 );
	}
}
#ifndef ASSERT_OPEN_GL_STATE
#define ASSERT_OPEN_GL_STATE( ... ) AssertOpenGLState( __FUNCTION__ )
#endif // ASSERT_OPEN_GL_STATE
#endif // VERBOSE_MESSAGING


namespace Ray
{
	/*** This is a simple class defining globally accessible variables. (Feel free to add more as needed.) */
	struct GlobalProperties
	{
		static bool DebugFlag;
	};

	/** This class stores information about the number of rays cast and the number of ray-primitive intersections performed. */
	struct RayTracingStats
	{
		static std::atomic< size_t > _RayNum;
		static std::atomic< size_t > _RayPrimitiveIntersectionNum;
		static std::atomic< size_t > _RayBoundingBoxIntersectionNum;
		static std::atomic< size_t > _ConeBoundingBoxIntersectionNum;
	public:

		static void Reset( void );
		static void IncrementRayNum( unsigned int count=1 );
		static void IncrementRayPrimitiveIntersectionNum( unsigned int count=1 );
		static void IncrementRayBoundingBoxIntersectionNum( unsigned int count=1 );
		static void IncrementConeBoundingBoxIntersectionNum( unsigned int count=1 );
		static size_t RayNum( void );
		static size_t RayPrimitiveIntersectionNum( void );
		static size_t RayBoundingBoxIntersectionNum( void );
		static size_t ConeBoundingBoxIntersectionNum( void );
	};

	/** This class serves as a wrapper for Util::BoundingBox3D, calling RayTracingStats::IncrementRayBoundingBoxIntersectionNum before performing the intersection. */
	struct ShapeBoundingBox : public Util::BoundingBox3D
	{
		ShapeBoundingBox( void ) : Util::BoundingBox3D() {};
		ShapeBoundingBox( const ShapeBoundingBox &bBox ) : Util::BoundingBox3D( bBox ) {}
		ShapeBoundingBox( const Util::BoundingBox3D &bBox ) : Util::BoundingBox3D( bBox ) {}
		ShapeBoundingBox &operator = ( const ShapeBoundingBox &bBox ){ Util::BoundingBox3D::operator = ( bBox ) ; return *this; }
		ShapeBoundingBox &operator = ( const Util::BoundingBox3D &bBox ){ Util::BoundingBox3D::operator = ( bBox ) ; return *this; }
		Util::BoundingBox1D intersect( const Util::Ray3D &ray ) const;
	};

	/** This is the abstract class that all ray-traceable objects must implement. */
	class Shape
	{
		friend std::ostream &operator << ( std::ostream & , const Shape & );
		friend std::istream &operator >> ( std::istream & ,       Shape & );

		/** This method writes the Shape into the stream (including the starting directive) */
		virtual void _write( std::ostream &stream ) const = 0;

		/** This method reads the Shape from the stream (excluding the starting directive) */
		virtual void _read( std::istream &stream ) = 0;
	protected:
		/** This member represents the bounding box of the shape. */
		ShapeBoundingBox _bBox;

		/** This member represents the number of primitives describing the shape.*/
		size_t _primitiveNum;

	public:
		/** A global variable representing how finely shapes should be tessellated for rendering as triangle meshes. */
		static unsigned int OpenGLTessellationComplexity;

		/** A global variable representing how inset the next shape should be when writing to a stream. */
		static unsigned int WriteInsetSize;

		/** This static method insets for writing. */
		static void WriteInset( std::ostream &stream );

		/** The destructor */
		virtual ~Shape( void ){}

		/** The method returns the bounding box of the shape */
		ShapeBoundingBox boundingBox( void ) const;

		/** This method should be called (once) immediately after setting up the scene graph */
		virtual void init( const class LocalSceneData& data ) = 0;

		/** This method should be called (once) after an OpenGL context has been created */
		virtual void initOpenGL( void ) = 0;

		/** This method should be called to update the bounding boxes in the scene */
		virtual void updateBoundingBox( void ) = 0;

		/** This method returns the name of the shape */
		virtual std::string name( void ) const = 0;

		/** This method determines if a point is inside a shape.
		*** It is assumed that if the shape is not water-tight, the method returns false. */
		virtual bool isInside( Util::Point3D p ) const = 0;

		/** This method calls the necessary OpenGL commands to render the primitive. */
		virtual void drawOpenGL( GLSLProgram *glslProgram ) const=0;

		/** This method adds a triangle to the list of triangles (if the object is of type RayTriangle). */
		virtual void addTrianglesOpenGL( std::vector< class TriangleIndex > &triangles ) {}

		/** This method returns the count of basic shapes contained within the Shape. */
		size_t primitiveNum( void ) const;

		struct ShapeProcessingInfo
		{
			ShapeProcessingInfo( void );
			Util::Matrix4D localToGlobal , globalToLocal;
			Util::Matrix3D directionGlobalToLocal , normalLocalToGlobal;
			const class Material *material;

			enum ProcessingType
			{
				NONE ,
				PROPAGATE ,
				TERMINATE
			};
		};

		typedef std::function< ShapeProcessingInfo::ProcessingType ( const ShapeProcessingInfo & , const Shape & ) > Filter;
		typedef std::function< void ( const ShapeProcessingInfo & , const Shape & ) > Kernel;
		typedef std::function< bool ( double ) > RayIntersectionFilter;
		typedef std::function< void ( const ShapeProcessingInfo & , const class RayShapeIntersectionInfo & ) > RayIntersectionKernel;

		/** This method process Shapes by calling the "kernel" function on every shape whose bounding box passes the "filter" test.
		* The tInfo parameter stores the accumulation of transformations encountered when traversing the scene-graph.
		* Note that the bounding box is in the coordinate frame of the node. */
		virtual void processOverlapping( const Filter &filter , const Kernel &kernel , ShapeProcessingInfo spInfo ) const;

		/** This method processes the first shape which intersect the ray within the prescribed range and passing the rFilter test,
		* invoking the rKernel kernel with the intersection information. The function returns true if there was an intersection.*/
		virtual bool processFirstIntersection( const Util::Ray3D &ray , const Util::BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const = 0;

		/** This method processes all shapes which intersect the ray within the prescribed range and passing the rFilter test,
		* invoking the rKernel kernel with the intersection information. The processing terminates early if the kernel returns false. 
		* The function returns the number of valid intersections. */
		virtual int processAllIntersections( const Util::Ray3D &ray , const Util::BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const = 0;
	};

	/** This operator writes the shape out to a stream. */
	inline std::ostream &operator << ( std::ostream &stream , const Shape &shape ){ shape._write( stream ) ; return stream; }

	/** This operator reads a shape from a stream. */
	inline std::istream &operator >> ( std::istream &stream ,       Shape &shape ){ shape._read( stream ) ; return stream; }
}
#endif // SHAPE_INCLUDED
