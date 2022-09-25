#ifndef SPHERE_LIGHT_INCLUDED
#define SPHERE_LIGHT_INCLUDED
#include "pointLight.h"

namespace Ray
{
	/** This class describes a spherical area-light light-source. This light has a fixed 
	* position in space and a fixed radius. The light attenuates as a function of the distance
	* from the light-source.
	*/
	class SphereLight : public PointLight
	{
	protected:
		/** The radius of the area-light */
		double _radius;

	public:
		/** This static method returns the directive describing the Light. */
		static std::string Directive( void ){ return "light_sphere"; }

		///////////////////
		// Light methods //
		///////////////////
	private:
		void _write( std::ostream &stream ) const;
		void _read( std::istream &stream );
	public:
		std::string name( void ) const { return "sphere light"; }
		Util::Point3D transparency( const class RayShapeIntersectionInfo &iInfo , const class Shape &shape , Util::Point3D cLimit , unsigned int samples , unsigned int tIdx ) const;
	};
}
#endif // SPHERE_LIGHT_INCLUDED


