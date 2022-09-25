#ifndef SPOT_LIGHT_INCLUDED
#define SPOT_LIGHT_INCLUDED
#include "light.h"

namespace Ray
{
	/** This class describes a spot-light light-source. This light has a fixed 
	* position in space and a fixed direction. The light attenuates both as a
	* function of the distance from the light-source and the angle between the
	* line from the light to the point and the light direction. The cutOffAngle
	* specifies the angle beyond which the light has no contribution.
	*/
	class SpotLight : public Light
	{
		/** The position of the spot-light */
		Util::Point3D _location;

		/** The preferred direction the outgoing light rays */
		Util::Point3D _direction;

		/** The constant term of the attenuation equation */
		double _constAtten;

		/** The linear term of the attenuation equation */
		double _linearAtten;

		/** The quadratic term of the attenuation equation */
		double _quadAtten;

		/** The cut-off angle for the spot light (should be in the range [0,Pi/2]) */
		double _cutOffAngle;

		/** The rate at which the intensity falls off as light travels in the non-preferred direction (should be in the range [0,128]) */
		double _dropOffRate;

	public:
		/** This static method returns the directive describing the Light. */
		static std::string Directive( void ){ return "light_spot"; }

		///////////////////
		// Light methods //
		///////////////////
	private:
		void _write( std::ostream &stream ) const;
		void _read( std::istream &stream );
	public:
		std::string name( void ) const { return "spot light"; }
		Util::Point3D getAmbient ( Util::Ray3D ray , const class RayShapeIntersectionInfo& iInfo , const Material &material ) const;
		Util::Point3D getDiffuse ( Util::Ray3D ray , const class RayShapeIntersectionInfo& iInfo , const Material &material ) const;
		Util::Point3D getSpecular( Util::Ray3D ray , const class RayShapeIntersectionInfo& iInfo , const Material &material ) const;
		bool isInShadow( const class RayShapeIntersectionInfo& iInfo , const Shape &shape , unsigned int tIdx ) const;
		Util::Point3D transparency( const class RayShapeIntersectionInfo &iInfo , const class Shape &shape , Util::Point3D cLimit , unsigned int samples , unsigned int tIdx ) const;
		void drawOpenGL( int index , GLSLProgram * glslProgram ) const;
	};
}
#endif // SPOT_LIGHT_INCLUDED


