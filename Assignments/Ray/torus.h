#ifndef TORUS_INCLUDED
#define TORUS_INCLUDED
#include <Util/geometry.h>
#include <Util/polynomial.h>
#include "shape.h"

namespace Ray
{
	/** This class describes a torus, and is represented by its center and two radii */
	class Torus : public Shape
	{
		/** The OpenGL vertex buffer identifier */
		GLuint _vertexBufferID = 0;

		/** The OpenGL element buffer identifier */
		GLuint _elementBufferID = 0;

		/** The index of the material associated with the box */
		int _materialIndex;

		/** The material associated with the torus */
		const class Material *_material;

	public:
		/** This static method returns the directive describing the shape. */
		static std::string Directive( void ){ return "shape_torus"; }

		/** The center of the torus */
		Util::Point3D center;

		/** The inner radius of the torus */
		double iRadius;

		/** The outer radius of the torus */
		double oRadius;

		/** The default constructor */
		Torus( void );

		///////////////////
		// Shape methods //
		///////////////////
	private:
		void _write( std::ostream &stream ) const;
		void _read( std::istream &stream );
	public:
		std::string name( void ) const { return "torus"; }
		void init( const class LocalSceneData &data );
		void initOpenGL( void );
		void updateBoundingBox( void );
		bool processFirstIntersection( const Util::Ray3D &ray , const Util::BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const;
		int processAllIntersections( const Util::Ray3D &ray , const Util::BoundingBox1D &range , const RayIntersectionFilter &rFilter , const RayIntersectionKernel &rKernel , ShapeProcessingInfo spInfo , unsigned int tIdx ) const;
		bool isInside( Util::Point3D p ) const;
		void drawOpenGL( GLSLProgram * glslProgram ) const;
	};
}
#endif // TORUS_INCLUDED

