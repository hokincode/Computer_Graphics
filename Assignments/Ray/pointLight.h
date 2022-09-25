#ifndef POINT_LIGHT_INCLUDED
#define POINT_LIGHT_INCLUDED
#include "light.h"

namespace Ray
{
	/** This class describes a point-light light-source. This light has a fixed position and the light attenuates as function of the distance from the light-source. */
	class PointLight : public Light
	{
	protected:
		/** The position of the spot-light */
		Util::Point3D _location;

		/** The constant term of the attenuation equation */
		double _constAtten;

		/** The linear term of the attenuation equation */
		double _linearAtten;

		/** The quadratic term of the attenuation equation */
		double _quadAtten;

	public:
		/** This static method returns the directive describing the Light. */
		static std::string Directive( void ){ return "light_point"; }

		///////////////////
		// Light methods //
		///////////////////
	private:
		void _write( std::ostream &stream ) const;
		void _read( std::istream &stream );
	public:
		std::string name( void ) const { return "point light"; }
		Util::Point3D getAmbient ( Util::Ray3D ray , const class RayShapeIntersectionInfo& iInfo , const Material &material ) const;
		Util::Point3D getDiffuse ( Util::Ray3D ray , const class RayShapeIntersectionInfo& iInfo , const Material &material ) const;
		Util::Point3D getSpecular( Util::Ray3D ray , const class RayShapeIntersectionInfo& iInfo , const Material &material ) const;
		bool isInShadow( const class RayShapeIntersectionInfo& iInfo , const class Shape &shape , unsigned int tIdx ) const;
		Util::Point3D transparency( const class RayShapeIntersectionInfo &iInfo , const class Shape &shape , Util::Point3D cLimit , unsigned int samples , unsigned int tIdx ) const;
		void drawOpenGL( int index , GLSLProgram *glslProgram ) const;
	};
}
#endif // POINT_LIGHT_INCLUDED


