#include <cmath>
#include <Util/exceptions.h>
#include "scene.h"

using namespace Ray;
using namespace Util;

///////////
// Scene //
///////////
Point3D Scene::Reflect( Point3D v , Point3D n )
{
	//////////////////
	// Reflect here //
	//////////////////
	WARN_ONCE( "method undefined" );
	return Point3D();
}

bool Scene::Refract( Point3D v , Point3D n , double ir , Point3D& refract )
{
	//////////////////
	// Refract here //
	//////////////////
	WARN_ONCE( "method undefined" );
	return false;
}

Point3D Scene::getColor( Ray3D ray , int rDepth , Point3D cLimit , unsigned int lightSamples , unsigned int tIdx )
{
	Point3D color;
	RayTracingStats::IncrementRayNum();
	ShapeProcessingInfo spInfo;
	RayIntersectionFilter rFilter = []( double ){ return true; };
	RayIntersectionKernel rKernel = [&]( const ShapeProcessingInfo &spInfo , const RayShapeIntersectionInfo &_iInfo )
	{
		/////////////////////////////////////////////////////////
		// Create the computational kernel that gets the color //
		/////////////////////////////////////////////////////////
		WARN_ONCE( "method undefined" );
		color = Point3D( 0. , 1. , 0. );
		return true;
	};

	processFirstIntersection( ray , BoundingBox1D( Epsilon , Infinity ) , rFilter , rKernel , spInfo , tIdx );

	return color;
}

//////////////
// Material //
//////////////
void Material::drawOpenGL( GLSLProgram * glslProgram ) const
{
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	WARN_ONCE( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}

/////////////
// Texture //
/////////////
void Texture::initOpenGL( void )
{
	///////////////////////////////////
	// Do OpenGL texture set-up here //
	///////////////////////////////////
	WARN_ONCE( "method undefined" );

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();	
}
