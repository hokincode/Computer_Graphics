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
	Point2D end_1 = endPoints[0];
	Point2D end_2 = endPoints[1];

	Point2D end_normal(end_2[0] - end_1[0], end_2[1] - end_1[1]);

	int x = - end_normal[1];
	int y = end_normal[0];

	return Point2D(x,y);
}

Point2D OrientedLineSegment::GetSourcePosition( const OrientedLineSegment& source , const OrientedLineSegment& destination , Point2D target )
{
	//////////////////////////////////
	// Set the source position here //
	//////////////////////////////////
	WARN( "method undefined" );

	double z1 = destination.endPoints[1][0] - destination.endPoints[0][0];
	double z2 = destination.endPoints[1][1] - destination.endPoints[0][1];

	double j1 = target[0] - destination.endPoints[0][0];
	double j2 = target[1] - destination.endPoints[0][1];

	double raw_u = z1 * j1 + z2 * j2;

	double length_dest = sqrt(pow(destination.endPoints[1][0] - destination.endPoints[0][0],2) - pow(destination.endPoints[1][1] - destination.endPoints[0][1], 2));

	double u = raw_u / length_dest / length_dest;

	// double v = destination.distance(target);

	double P1X = destination.endPoints[0][0];
	double P1Y = destination.endPoints[0][1];
	double P2X = destination.endPoints[1][0];
	double P2Y = destination.endPoints[1][1];
	double P3X = target[0];
	double P3Y = target[1];

	double numerator = P2Y*(P1X-P3X) + P1Y*(P3X-P2X) + P3Y*(P2X-P1X);
    double denominator = (P2X-P1X)*(P1X-P3X) + (P2Y-P1Y)*(P1Y-P3Y);
    double ratio = numerator/denominator;

    double angleRad = atan(ratio);

	double target_endpoint_distance = sqrt(pow(target[0] - destination.endPoints[0][0],2) - pow(target[1] - destination.endPoints[0][1], 2));

	double v = target_endpoint_distance * sin(angleRad);

	if (v < 0) {
		v = - v;
	}

	double s_x = source.endPoints[1][0] - source.endPoints[0][0];
	double s_y = source.endPoints[1][1] - source.endPoints[0][1];

	double new_u_x = source.endPoints[1][0] + s_x * u;
	double new_u_y = source.endPoints[1][1] + s_y * u;

	double souce_slope = (source.endPoints[1][1] - source.endPoints[0][1]) / (source.endPoints[1][0] - source.endPoints[0][0]);
	double find_p_d_slope = 1 /  souce_slope;
	double rad = atan(find_p_d_slope);

	double final_p_x = new_u_x + sin(rad) * v;
	double final_p_y = new_u_y + cos(rad) * v; 

	return Point2D((int)final_p_x, (int)final_p_y);
}