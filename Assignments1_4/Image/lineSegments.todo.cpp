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
	double len = sqrt(pow(endPoints[1][0] -  endPoints[0][0],2) + pow(endPoints[1][1] -  endPoints[0][1],2));
	return len;
}
double OrientedLineSegment::distance( Point2D p ) const
{
	//////////////////////////////
	// Return the distance here //
	//////////////////////////////
	double dis = 0.0;
	Point2D end_1 = endPoints[0];
	Point2D end_2 = endPoints[1];

	double A = p[0] - end_1[0];
	double B = p[1] - end_1[1];
	double C = end_2[0] - end_1[0];
	double D = end_2[1] - end_2[1];

	double dot = A * C + B * D;
	double len_sq = C * C + D * D;
	double param = - 1.0;
  	if (len_sq != 0) param = dot / len_sq;

	double xx = 0.0;
	double yy = 0.0;

	if (param < 0) {
    	xx = end_1[0];
    	yy = end_1[0];
  	}
  	else if (param > 1) {
   		xx = end_2[0];
    	yy = end_2[0];
  	}
  	else {
   		xx = end_1[0] + param * C;
    	yy = end_1[0] + param * D;
  	}

	dis = sqrt(pow(p[0]-xx, 2) + pow(p[1]-yy,2));

	return dis;
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