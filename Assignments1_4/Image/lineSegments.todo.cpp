#include "lineSegments.h"
#include <math.h>
#include <Util/exceptions.h>

using namespace Util;
using namespace Image;

////////////////////////////
// Image processing stuff //
////////////////////////////
double OrientedLineSegment::length( void ) const
{
	////////////////////////////
	// Return the length here //
	////////////////////////////
	WARN( "method undefined" );
	return -1.;
}
double OrientedLineSegment::distance( Point2D p ) const
{
	//////////////////////////////
	// Return the distance here //
	//////////////////////////////
	WARN( "method undefined" );
	return -1.;
}
Point2D OrientedLineSegment::perpendicular( void ) const
{
	////////////////////////////////
	// Set the perpendicular here //
	////////////////////////////////
	WARN( "method undefined" );
	return Point2D();
}

Point2D OrientedLineSegment::GetSourcePosition( const OrientedLineSegment& source , const OrientedLineSegment& destination , Point2D target )
{
	//////////////////////////////////
	// Set the source position here //
	//////////////////////////////////
	WARN( "method undefined" );
	return Point2D();
}