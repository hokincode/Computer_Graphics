#ifndef LINE_SEGMENTS_INCLUDED
#define LINE_SEGMENTS_INCLUDED

#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <Util/geometry.h>
#include <Util/algebra.h>

namespace Image
{
	/** This class represents an oriented line segment. */
	class OrientedLineSegment : public Util::VectorSpace< OrientedLineSegment >
	{
	public:
		/** The constant offset for the denominator in the weight function for Beier-Neely morphing. */
		static const double A;

		/** The numerator exponent in the weight function for Beier-Neely morphing. */
		static const double B;

		/** The total exponent in the weight function for Beier-Neely morphing. */
		static const double P;

		/** The end-points of the line segment. */
		Util::Point2D endPoints[2];

		/** Given a point p, this method returns the weight of the line segment's contribution to the point.*/
		double getWeight( Util::Point2D p ) const;

		/** This method returns the length of the line segment. */
		double length( void ) const;

		/** This method returns the distance of a point from the line segment. */
		double distance( Util::Point2D p ) const;

		/** This method returns the unit-vector perpendicular to the direction of the line segment. */
		Util::Point2D perpendicular( void ) const;

		/** This static method sets the value of the source pixel position given the destination
		*** pixel position and a pair of corresponding source and destination line segments */
		static Util::Point2D GetSourcePosition( const OrientedLineSegment &source , const OrientedLineSegment &destination , Util::Point2D target );

		/////////////////////////
		// VectorSpace methods //
		/////////////////////////
		/** Scaling method for VectorSpace */
		OrientedLineSegment operator * ( double s ) const;

		/** Addition method for VectorSpace */
		OrientedLineSegment operator + ( const OrientedLineSegment &ols ) const;
	};

	/** Functionality for outputing an oriented line segment to a stream.*/
	std::ostream &operator << ( std::ostream &stream , const OrientedLineSegment &ols );

	/** Functionality for inputing an oriented line segment from a stream.*/
	std::istream &operator >> ( std::istream &stream , OrientedLineSegment &ols );

	/** This class represents an ordered list of corresponding line segment pairs. */
	class OrientedLineSegmentPairs : public std::vector< std::pair< OrientedLineSegment , OrientedLineSegment > >
	{
	public:
		/** This method sets the value of the source pixel position (sourceX,sourceY), given the destination
		*** pixel position. It computes the source position prescribed by each of the line segment pairs and
		*** sets the final source position as the weighted average. */
		Util::Point2D getSourcePosition( Util::Point2D destination ) const;
	};

	/** Functionality for outputing a set of oriented line segment pairs to a stream.*/
	std::ostream &operator << ( std::ostream &stream , const OrientedLineSegmentPairs &ols );

	/** Functionality for inputing a set of oriented line segment pairs from a stream.*/
	std::istream &operator >> ( std::istream &stream ,       OrientedLineSegmentPairs &ols );
}
#endif // LINE_SEGMENTS_INCLUDED