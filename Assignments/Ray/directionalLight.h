#ifndef DIRECTIONAL_LIGHT_INCLUDED
#define DIRECTIONAL_LIGHT_INCLUDED
#include "light.h"

namespace Ray
{
	/** This class represents a directional light source.
	*** The light has no position in space and hence has no attenuation properties. */
	class DirectionalLight : public Light
	{
		/** The direction the outgoing light rays */
		Util::Point3D _direction;
	public:
		/** This static method returns the directive describing the Light. */
		static std::string Directive( void ){ return "light_dir"; }

		///////////////////
		// Light methods //
		///////////////////
	private:
		void _write( std::ostream &stream ) const;
		void _read( std::istream &stream );
	public:
		std::string name( void ) const { return "directional light"; }
		Util::Point3D getAmbient ( Util::Ray3D ray , const class RayShapeIntersectionInfo& iInfo , const Material &material ) const;
		Util::Point3D getDiffuse ( Util::Ray3D ray , const class RayShapeIntersectionInfo& iInfo , const Material &material ) const;
		Util::Point3D getSpecular( Util::Ray3D ray , const class RayShapeIntersectionInfo& iInfo , const Material &material ) const;
		bool isInShadow( const class RayShapeIntersectionInfo& iInfo , const class Shape &shape , unsigned int tIdx ) const;
		Util::Point3D transparency( const class RayShapeIntersectionInfo &iInfo , const class Shape &shape , Util::Point3D cLimit , unsigned int samples , unsigned int tIdx ) const;
		void drawOpenGL( int index , GLSLProgram * glslProgram ) const;
	};
}
	
#endif // DIRECTIONAL_LIGHT_INCLUDED
	
	
