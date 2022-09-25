#ifndef RAY_LIGHT_INCLUDED
#define RAY_LIGHT_INCLUDED
#include "shape.h"

namespace Ray
{
	/** This abstract class represents a light source in the scene. */
	class Light
	{
		friend std::ostream &operator << ( std::ostream & , const Light & );
		friend std::istream &operator >> ( std::istream & ,       Light & );

		/** This method writes the Light into the stream (including the starting directive) */
		virtual void _write( std::ostream &stream ) const=0;

		/** This method reads the Light from the stream (excluding the starting directive) */
		virtual void _read( std::istream &stream )=0;
	protected:
		/** The ambient color of the light source */
		Util::Point3D _ambient;

		/** The diffuse color of the light source */
		Util::Point3D _diffuse;

		/** The specular color of the light source */
		Util::Point3D _specular;

	public:
		/** The destructor */
		virtual ~Light( void ){}

		/** This method returns the name of the shape */
		virtual std::string name( void ) const = 0;

		/** This method returns the ambient contribution of the light source to the specified hit location. */
		virtual Util::Point3D getAmbient( Util::Ray3D ray , const class RayShapeIntersectionInfo& iInfo , const Material &material ) const=0;

		/** This method returns the diffuse contribution of the light source to the specified hit location. */
		virtual Util::Point3D getDiffuse( Util::Ray3D ray , const class RayShapeIntersectionInfo& iInfo , const Material &material ) const=0;

		/** This method returns the specular contribution of the light source to the specified hit location. */
		virtual Util::Point3D getSpecular( Util::Ray3D ray , const class RayShapeIntersectionInfo& iInfo , const Material &material ) const=0;

		/** This method tests if the intersection point represented by iInfo is in shadow from the light source.
		*** The returned value is either 0 if the the intersection point is not in shadow or 1 if it is. */
		virtual bool isInShadow( const class RayShapeIntersectionInfo& iInfo , const class Shape &shape , unsigned int tIdx ) const=0;

		/** This method tests if the intersection point represented by iInfo is in partial shadow from the light source.
		*** A ray is cast from the hit location to the light source, and the transparency values are accumulated.
		*** If the light source is an area light source, multiple rays area cast to (uniformly randomly chosen positions on
		*** the area light sources and the transparency is set to the average of the transparency values over the samples.
		*** If the transparency value falls below cLimit, the testing terminates. */
		virtual Util::Point3D transparency( const class RayShapeIntersectionInfo &iInfo , const class Shape &shape , Util::Point3D cLimit , unsigned int samples , unsigned int tIdx ) const=0;

		/** This method calls the necessary OpenGL commands to render the light.
		*** The index argument specifices the index of the light that is to be drawn. */
		virtual void drawOpenGL( int index , GLSLProgram * glslProgram ) const=0;
	};

	/** This operator writes the Light out to a stream */
	inline std::ostream &operator << ( std::ostream &stream , const Light &light ){ light._write( stream ) ; return stream; }

	/** This operator reads the Light in from a stream */
	inline std::istream &operator >> ( std::istream &stream ,       Light &light ){ light._read( stream ) ; return stream; }
}
#endif // RAY_LIGHT_INCLUDED
  

