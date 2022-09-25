#ifndef ALGEBRA_INCLUDED
#define ALGEBRA_INCLUDED

#include <cmath>

namespace Util
{
	/** This templated class fills in the group operators for the class Element
	* It assumes the following operations are well defined for const Element &e and const Element &f:
	*		Element g = e.additiveInverse();
	*		Element g = e + f;
	*/
	template< class Element >
	class AdditiveGroup
	{
	public:
		/** This method returns the additive inverse of an element */
		friend Element operator - ( const Element &e ){ return e.additiveInverse(); }

		/** This method returns the ratio of two elements */
		friend Element operator - ( const Element &e1 , const Element &e2 ){ return e1 + (-e2); }

		/** This method returns the (in-place) product of two elements */
		friend Element& operator += ( Element &e1 , const Element &e2 ){ e1 = e1 + e2 ; return e1; }

		/** This method returns the (in-place) ratio of two elements */
		friend Element& operator -= ( Element &e1 , const Element &e2 ){ e1 = e1 - e2 ; return e1; }
	};

	/** This templated class fills in the group operators for the class Element
	* It assumes the following operations are well defined for const Element &e and const Element &f:
	*		Element g = e.multiplicativeInverse();
	*		Element g = e * f;
	*/
	template< class Element >
	class MultiplicativeGroup
	{
	public:
		/** This method returns the ratio of two elements */
		friend Element operator / ( const Element &e1 , const Element &e2 ){ return e1 * e2.multiplicativeInverse(); }

		/** This method returns the (in-place) product of two elements */
		friend Element& operator *= ( Element &e1 , const Element &e2 ){ e1 = e1 * e2 ; return e1; }

		/** This method returns the (in-place) ratio of two elements */
		friend Element& operator /= ( Element &e1 , const Element &e2 ){ e1 = e1 / e2 ; return e1; }
	};

	/** This templated class fills in the field operators for the class Element
	  * It assumes the following operations are well defined for const Element &e and const Element &f:
	  *		Element g = e.additiveInverse();
	  *		Element g = e.multiplicativeInverse();
	  *		Element g = e + f;
	  *		Element g = e * f;
	  */
	template< class Element >
	class Field : public AdditiveGroup< Element > , public MultiplicativeGroup< Element >{};

	/** This templated class fills in the vector space methods for the class Element
	  * It assumes the following operations are well defined for const Element &e, const Element &f, and double s:
	  *		Element g = e + f;
	  *		Element g = e * s;
	  */
	template< class Element >
	class VectorSpace
	{
	public:
		/** This method returns the difference of two elements */
		friend Element operator - ( const Element &e1 , const Element &e2 ){ return e1 + (-e2); }

		/** This method returns the division of the element by a scalar */
		friend Element operator / ( const Element &e , double s ){ return e * (1./s); }

		/** This method returns the (in-place) sum of two elements */
		friend Element& operator += ( Element & e1 , const Element &e2 ){ e1 = e1 + e2 ; return e1; }

		/** This method returns the (in-place) difference of two elements */
		friend Element& operator -= ( Element &e1 , const Element &e2 ){ return e1 += (-e2); }

		/** This method returns the (in-place) product of the element with a scalar */
		friend Element& operator *= ( Element &e , double s ){ e = e*s ; return e; }

		/** This method returns the (in-place) division of the element by a scalar */
		friend Element& operator /= ( Element &e , double s ){ return e *= (1./s); }

		/** This method returns the negation of an element */
		friend Element  operator - ( const Element &e ){ return e * (-1.); }

		/** This method returns the product of a scalar with the element */
		friend Element  operator * ( double s , const Element &e ){ return e*s; }
	};

	/** This templated class fills in the inner-product space methods for the class Element
	  * It assumes the following operations are well defined for const Element &e and const Element &f:
	  *		double dot = e.dot( f );
	  */
	template< class Element >
	class _InnerProductSpace
	{
	public:
		/** This static method returns the dot-product of two elements */
		static double Dot( const Element &e1 , const Element &e2 ){ return e1.dot(e2); }

		/** This static method returns the (squared) L2-norm of an element */
		static double SquareNorm( const Element &e ){ return e.dot(e); }

		/** This static method returns the L2-norm of an element */
		static double Length( const Element &e ){ return sqrt( e.dot(e) ); }

		/** This static method returns (squared) L2-distance between two elements */
		static double SquareDistance( const Element &e1 , const Element &e2 ){ return SquareNorm(e1-e2); }

		/** This static method returns the L2-distance between two elements */
		static double Distance( const Element &e1 , const Element &e2 ){ return sqrt( SquareDistance(e1,e2) ); }

		/** This method returns the (squared) L2-norm of an element */
		double squareNorm( void ) const { return SquareNorm( *( Element * )this ); }

		/** This method returns L2-norm of an element */
		double length( void ) const { return sqrt( squareNorm() ); }

		/** This method returns the normalized element */
		Element unit( void ) const { return *( Element * )this / _InnerProductSpace< Element >::length(); }
	};

	/** This templated class fills in the vector-space and inner-product space methods for the class Element
	  * It assumes the following operations are well defined for const Element &e, const Element &f, and double s:
	  *		Element g = e + f;
	  *		Element g = e * s;
	  *		double dot = e.dot( f );
	  */
	template< class Element >
	class InnerProductSpace : public VectorSpace< Element > , public _InnerProductSpace< Element >{};

	/** This templated class fills in the algebra methods and operators for the class Element
	  * It assumes the following operations are well defined for const Element &e, const Element &f, and double s:
	  *		Element g = e + f;
	  *		Element g = e * f;
	  *		Element g = e * s;
	  */
	template< class Element >
	class Algebra : public VectorSpace< Element >
	{
	public:
		/** An operator performing (in-place) element multiplication */
		friend Element& operator *= ( Element &e1 , const Element &e2 ){ e1 = e1 * e2 ; return e1; }
	};
}
#endif // ALGEBRA_INCLUDED
