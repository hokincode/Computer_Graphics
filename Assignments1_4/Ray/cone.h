#ifndef CONE_INCLUDED
#define CONE_INCLUDED
#include <Util/geometry.h>
#include <Util/polynomial.h>
#include "shape.h"

namespace Ray
{
	/** This class represents a cone whose central axis is parallel to the y-axis, and 
	* is defined by the center of the cone, the height from the tip to the base
	* and the base of the cone. */
	class Cone : public Shape
	{
		/** The OpenGL vertex buffer identifier */
		GLuint _vertexBufferID = 0;

		/** The OpenGL element buffer identifier */
		GLuint _elementBufferID = 0;

		/** The index of the material associated with the box */
		int _materialIndex;

		/** The material associated with the cone */
		const class Material *_material;
	public:
		/** The center of the cone */
		Util::Point3D center;

		/** The height of the cone */
		double height;

		/** The radius of the cone */
		double radius;

		/** This static method returns the directive describing the shape. */
		static std::string Directive( void ){ return "shape_cone"; }

		/** The default constructor */
		Cone( void );

		///////////////////
		// Shape methods //
		///////////////////
	private:
		void _write( std::ostream &stream ) const;
		void _read( std::istream &stream );
	public:
		std::string name( void ) const { return "cone"; }
		void init( const class LocalSceneData &data );
		void initOpenGL( void );
		void updateBoundingBox( void );
		bool processFirstIntersection( const Util::Ray3D &ray , const Util::BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const;
		int processAllIntersections( const Util::Ray3D &ray , const Util::BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const;
		bool isInside( Util::Point3D p ) const;
		void drawOpenGL( GLSLProgram *glslProgram ) const;
	};
}
#endif // CONE_INCLUDED
