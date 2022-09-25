#include <cmath>
#include <Util/exceptions.h>
#include "scene.h"
#include "spotLight.h"

using namespace Ray;
using namespace Util;

///////////////
// SpotLight //
///////////////
Point3D SpotLight::getAmbient( Ray3D ray , const RayShapeIntersectionInfo &iInfo , const Material &material ) const
{
	////////////////////////////////////////////////////
	// Get the ambient contribution of the light here //
	////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return Point3D();
}

Point3D SpotLight::getDiffuse( Ray3D ray , const RayShapeIntersectionInfo &iInfo , const Material &material ) const
{
	////////////////////////////////////////////////////
	// Get the diffuse contribution of the light here //
	////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return Point3D();
}

Point3D SpotLight::getSpecular( Ray3D ray , const RayShapeIntersectionInfo &iInfo , const Material &material ) const
{
	/////////////////////////////////////////////////////
	// Get the specular contribution of the light here //
	/////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return Point3D();
}

bool SpotLight::isInShadow( const RayShapeIntersectionInfo& iInfo , const Shape &shape , unsigned int tIdx ) const
{
	//////////////////////////////////////////////
	// Determine if the light is in shadow here //
	//////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return false;
}

Point3D SpotLight::transparency( const RayShapeIntersectionInfo &iInfo , const Shape &shape , Point3D cLimit , unsigned int samples , unsigned int tIdx ) const
{
	//////////////////////////////////////////////////////////
	// Compute the transparency along the path to the light //
	//////////////////////////////////////////////////////////
	WARN_ONCE( "method undefined" );
	return Point3D( 1. , 1. , 1. );
}

void SpotLight::drawOpenGL( int index , GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	WARN_ONCE( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}
