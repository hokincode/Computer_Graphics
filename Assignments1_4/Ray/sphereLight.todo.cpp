#include <cmath>
#include <random>
#include <Util/exceptions.h>
#include "scene.h"
#include "sphereLight.h"

using namespace Ray;
using namespace Util;

/////////////////
// SphereLight //
/////////////////
Point3D SphereLight::transparency( const RayShapeIntersectionInfo &iInfo , const Shape &shape , Point3D cLimit , unsigned int samples , unsigned int tIdx ) const
{
	//////////////////////////////////////////////////////////
	// Compute the transparency along the path to the light //
	//////////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return PointLight::transparency( iInfo , shape , cLimit , samples , tIdx );
}