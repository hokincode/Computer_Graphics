#include "lineSegments.h"
#include <math.h>
#include <Util/exceptions.h>

using namespace Util;
using namespace Image;

/////////////////////////
// OrientedLineSegment //
/////////////////////////
const double OrientedLineSegment::A=1;

const double OrientedLineSegment::B=1;

const double OrientedLineSegment::P=1;

double OrientedLineSegment::getWeight( Point2D p ) const { return pow( pow( length() , P ) / ( A+distance(p) ) , B ); }

OrientedLineSegment OrientedLineSegment::operator * ( double s ) const
{
	OrientedLineSegment ols;
	for( int i=0 ; i<2 ; i++ ) ols.endPoints[i] = endPoints[i] * s;
	return ols;
}

OrientedLineSegment OrientedLineSegment::operator + ( const OrientedLineSegment &ols ) const
{
	OrientedLineSegment _ols;
	for( int i=0 ; i<2 ; i++ ) _ols.endPoints[i] = endPoints[i] + ols.endPoints[i];
	return _ols;
}

namespace Image
{
	std::ostream &operator << ( std::ostream &stream , const OrientedLineSegment &ols ){ return stream << ols.endPoints[0] << "  " << ols.endPoints[1]; }

	std::istream &operator >> ( std::istream &stream ,       OrientedLineSegment &ols ){ return stream >> ols.endPoints[0] >> ols.endPoints[1]; }
}

//////////////////////////////
// OrientedLineSegmentPairs //
//////////////////////////////

Point2D OrientedLineSegmentPairs::getSourcePosition( Point2D target ) const
{
	Point2D totalD;
	double totalWeight=0;

	for( int i=0 ; i<size() ; i++ )
	{
		// Compute the displacement of the source position with respect to line i from the target
#if 1
		Point2D d = OrientedLineSegment::GetSourcePosition( (*this)[i].first , (*this)[i].second , target );
#else
		Point2D d = OrientedLineSegment::GetSourcePosition( (*this)[i].first , (*this)[i].second , target ) - target;
#endif
		double weight = (*this)[i].second.getWeight( target );
		totalD += d*weight;
		totalWeight += weight;
	}
	return totalD / totalWeight;
}

namespace Image
{
	std::ostream &operator << ( std::ostream &stream , const OrientedLineSegmentPairs &ols )
	{
		stream << ols.size() << std::endl;
		for( int i=0 ; i<ols.size() ; i++ ) stream << ols[i].first << "  " << ols[i].second << std::endl;
		return stream;
	}

	std::istream &operator >> ( std::istream &stream , OrientedLineSegmentPairs &ols )
	{
		size_t sz;
		if( !( stream >> sz ) ) THROW( "Failed to read size" );
		ols.resize( sz );
		for( int i=0 ; i<sz ; i++ ) if( !( stream >> ols[i].first >> ols[i].second ) ) THROW( "Failed to read oriented line segment pair: " , sz );
		return stream;
	}
}
