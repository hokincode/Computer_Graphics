/*
Copyright (c) 2019, Michael Kazhdan
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer. Redistributions in binary form must reproduce
the above copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the distribution. 

Neither the name of the Johns Hopkins University nor the names of its contributors
may be used to endorse or promote products derived from this software without specific
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
*/

#ifndef GEOMETRY_INCLUDED
#define GEOMETRY_INCLUDED

#include <iostream>
#include <string>
#include <limits>
#include <Util/algebra.h>

namespace Util
{
	static const double Pi = 3.1415926535897932384;
	static const double Epsilon = 1e-10;
	static const double Infinity = std::numeric_limits< double >::infinity();

	/*** This is a simple class defining globally accessible variables. */
	struct GlobalProperties
	{
		static bool DebugFlag;
	};

	/** This templated class represents a Dim-dimenaional vector */
	template< unsigned int Dim >
	class Point : public InnerProductSpace< Point< Dim > >
	{
		/** The coordinates of the point */
		double _p[Dim];

		/** Initializes coordinate values from an array */
		void _init( const double *values , unsigned int sz );
	public:
		/** Default constructor, initializes coefficients to zero. */
		Point( void );

		/** Copy constructor */
		Point( const Point &p );

		/** Variadic constructor. Assumes the number of values is equal to the dimension and that all values are doubles. */
		template< typename ... Doubles >
		Point( Doubles ... values );

		/** This method returns a reference to the indexed coefficient.*/
		double &operator[] ( int index );

		/** This method returns a reference to the indexed coefficient.*/
		const double &operator[] ( int index ) const;

		/** This method performs a component-wise multiplication of two ponts and returns the product. */
		Point  operator *  ( const Point &p ) const;

		/** This method multiplies the coefficients of the current point by the coefficients of the input point. */
		Point &operator *= ( const Point &p );	

		/** This method performs a component-wise division of two ponts and returns the ratio. */
		Point  operator /  ( const Point &p ) const;

		/** This method divides the coefficients of the current point by the coefficients of the input point. */
		Point& operator /= ( const Point &p );

		/** This method returns the cross-product of Dim-1 points. */
		template< typename ... Points >
		static Point CrossProduct( Points ... points );

		/** This method returns the cross-product of Dim-1 points. */
		static Point CrossProduct( Point *points );

		/** This method returns the cross-product of Dim-1 points. */
		static Point CrossProduct( const Point *points );

		///////////////////////////////
		// InnerProductSpace methods //
		///////////////////////////////

		/** Scaling method for vector space */
		Point operator * ( double s ) const;

		/** Addition method for vector space */
		Point operator + ( const Point &p ) const;

		/** Dot-product method for inner-product space */
		double dot( const Point &p ) const;
	};

	/** A zero-dimensional point does not really make sense */
	template<> class Point< 0 >{};

	/** Functionality for outputing a point to a stream.*/
	template< unsigned int Dim >
	std::ostream &operator << ( std::ostream &stream , const Point< Dim > &p );

	/** Functionality for inputing a point from a stream.*/
	template< unsigned int Dim >
	std::istream &operator >> ( std::istream &stream , Point< Dim > &p );

	/** This templated class represents a general rectangular, Rows x Cols, matrix that is the base for both rectangular and square matrices
	*  Matrices are stored in row-major order. */
	template< unsigned int Rows , unsigned int Cols , typename MatrixType , typename MatrixTransposeType >
	class _BaseMatrix
	{
		/** The actual matrix entries */
		double _m[Rows][Cols];
	public:
		/** The default constructor generates a zero matrix */
		_BaseMatrix( void );

		/** This method returns the entry of the matrix in the r-th row and the c-th column.*/
		double &operator() ( int r , int c );

		/** This method returns the entry of the matrix in the r-th row and the c-th column.*/
		const double &operator() ( int r , int c ) const;

		/** This method returns the transpose of a matrix.*/
		MatrixTransposeType transpose( void ) const;

		/** This method transforms a Dim-dimensional point by applying the linear transformation. */
		Point< Rows > operator * ( const Point< Cols > &p ) const;	

		/////////////////////
		// Algebra methods //
		/////////////////////
		/** Scaling method for vector space */
		MatrixType operator * ( double s ) const;

		/** Addition method for vector space */
		MatrixType operator + ( const MatrixType &m ) const;

		///////////////////////////////
		// InnerProductSpace methods //
		///////////////////////////////
		/** Dot-product method for inner-product space */
		double dot( const _BaseMatrix &p ) const;
	};


	template< unsigned int Rows , unsigned int Cols > class Matrix;

	/** This templated class represents a Rows x Cols matrix.
	*  Matrices are stored in row-major order. */
	template< unsigned int Rows , unsigned int Cols >
	class Matrix : public _BaseMatrix< Rows , Cols , Matrix< Rows , Cols > , Matrix< Cols , Rows > > , InnerProductSpace< Matrix< Rows , Cols > >
	{
	public:
		/** Expose the constructors/methods of the base class */
		using _BaseMatrix< Rows , Cols , Matrix< Rows , Cols > , Matrix< Cols , Rows > >::_BaseMatrix;
		using _BaseMatrix< Rows , Cols , Matrix< Rows , Cols > , Matrix< Cols , Rows > >::operator *;
		using _BaseMatrix< Rows , Cols , Matrix< Rows , Cols > , Matrix< Cols , Rows > >::operator ();
		using _BaseMatrix< Rows , Cols , Matrix< Rows , Cols > , Matrix< Cols , Rows > >::transpose;

		/** The default constructor that initializes the matrix to zero.*/
		Matrix( void );

		/** Multiplication of matrices */
		template< unsigned int _Cols >
		Matrix< Rows , _Cols > operator * ( const Matrix< Cols , _Cols > &m ) const;
	};

	/** Specialization of the matrix class for the case when it is square. */
	template< unsigned int Dim >
	class Matrix< Dim , Dim > : public _BaseMatrix< Dim , Dim , Matrix< Dim , Dim > , Matrix< Dim , Dim > > , public InnerProductSpace< Matrix< Dim , Dim > > , Algebra< Matrix< Dim , Dim > >
	{
	public:
		/** Expose the constructors/methods of the base class */
		using _BaseMatrix< Dim , Dim , Matrix< Dim , Dim > , Matrix< Dim , Dim > >::_BaseMatrix;
		using _BaseMatrix< Dim , Dim , Matrix< Dim , Dim > , Matrix< Dim , Dim > >::operator *;
		using _BaseMatrix< Dim , Dim , Matrix< Dim , Dim > , Matrix< Dim , Dim > >::operator ();
		using _BaseMatrix< Dim , Dim , Matrix< Dim , Dim > , Matrix< Dim , Dim > >::transpose;

		/** The default constructor that initializes the matrix to zero.*/
		Matrix( void );

		/** This constructor generates a matrix by slicing out the top left sub-matrix*/
		Matrix( const Matrix< Dim+1 , Dim+1 > &m );

		/** This constructor generates matrix by projectivizing the input matrix, using m as the linear part and p as the translation.*/
		Matrix( const Matrix< Dim-1 , Dim-1 > &m , Point< Dim-1 > p = Point< Dim-1 >() );

		/** This method returns the determinant of the sub-matrix with the prescribed columns and rows removed. */
		double subDeterminant( int r , int c ) const;

		/** This method returns the determinant of the matrix.*/
		double determinant( void ) const;

		/** This method returns the trace of the matrix.*/
		double trace( void ) const;

		/** This method returns the inverse of a matrix.
		*** The method throws an exception if the determinant is zero. */
		Matrix inverse( void ) const;

		/** This method tries to set the reference to the inverse of the matrix.
		* The boolean value indicates if it succeeded (i.e. if the matrix is non-singular). */
		bool setInverse( Matrix &m ) const;

		/** This method transforms a (Dim-1)-dimensional point by applying the projective transformation. */
		Point< Dim-1 > operator * ( const Point< Dim-1 > &p ) const;

		/** This static method returns the identity matrix. */
		static Matrix Identity( void );

		/** This method returns the logarithm of a matrix, using the specified threshold to terminate the approxmation. */
		static Matrix Log( const Matrix &m , double eps=0.0001 );

		/** This method returns the square-root of a matrix, using the specified threshold to terminate the approxmation. */
		static Matrix SquareRoot( const Matrix &m , double eps=0.000001 );

		/** This method computes the SVD decomposition of the upper 3x3 matrix, such that
		*** r1 and r2 are rotations and the upper 3x3 matrix is equal to r1*diagonal*r2 */
		void SVD( Matrix &r1 , Matrix &diagonal , Matrix &r2 ) const;

		/** This method returns nearest symmetric matrix */
		Matrix symmetrize( void ) const;

		/** This method returns nearest skew-symmetric matrix */
		Matrix skewSymmetrize( void ) const;

		/** This method returns the exponent of a matrix using a Taylor approximation with the specified number of terms. */
		static Matrix Exp( const Matrix &m , int terms=100 );

		/** This method returns the closest rotation matrix */
		Matrix closestRotation( void ) const;

		/////////////////////
		// Algebra methods //
		/////////////////////
		/** Multiplication of matrices */
		template< unsigned int Cols >
		Matrix< Dim , Cols > operator * ( const Matrix< Dim , Cols > &m ) const;
	};

	/** Functionality for outputing a matrices to a stream.*/
	template< unsigned int Rows , unsigned int Cols , typename MatrixType , typename MatrixTransposeType >
	std::ostream &operator << ( std::ostream &stream , const _BaseMatrix< Rows , Cols , MatrixType , MatrixTransposeType > &m );

	/** Functionality for inputting a matrix from a stream.*/
	template< unsigned int Rows , unsigned int Cols , typename MatrixType , typename MatrixTransposeType >
	std::istream &operator >> ( std::istream &stream , _BaseMatrix< Rows , Cols , MatrixType , MatrixTransposeType > &m );


	/** This templated class represents a hyperplane in Dim dimensions. */
	template< unsigned int Dim >
	class Plane
	{
	public:
		/** The normal of the plane */
		Point< Dim > normal;
		/** (Minus) the normal distance of the plane from the origin */
		double distance;

		/** Default constructor*/
		Plane( void );

		/** This constructor generates a plane with normal n, passing through the point p.*/
		Plane( const Point< Dim > &n , const Point< Dim > &p );

		/** This constructor generates a plane that contains the simplex specified by the Dim vertices. */
		template< typename ... Points >
		Plane( Points ... points );

		/** This constructor generates a plane that contains the simplex specified by the Dim vertices. */
		Plane( Point< Dim > *points );

		/** This constructor generates a plane that contains the simplex specified by the Dim vertices. */
		Plane( const Point< Dim > *points );

		/** This method evalues the plane equation at the specified point, returning < p , normal > + distance. */
		double operator()( const Point< Dim > &p ) const;
	};

	/** This templated class represents a Ray.*/
	template< unsigned int Dim >
	class Ray
	{
	public:
		/** The starting point of the ray */
		Point< Dim > position;

		/** The direction of the ray */
		Point< Dim > direction;

		/** The default constructor */
		Ray( void );

		/** The constructor settign the the position and direction of the ray */
		Ray( const Point< Dim > &position , const Point< Dim > &direction );

		/** This method computes the translation of the ray by p and returns the translated ray.*/
		Ray  operator +  ( const Point< Dim > &p ) const;

		/** This method translates the current ray by p.*/
		Ray &operator += ( const Point< Dim > &p );

		/** This method computes the translation of the ray by -p and returns the translated ray.*/
		Ray  operator -  ( const Point< Dim > &p ) const;

		/** This method translates the current ray by -p.*/
		Ray &operator -= ( const Point< Dim > &p );

		/** This method returns the point at a distance of t along the ray. */
		Point< Dim > operator() ( double t ) const;
	};

	/** This method applies a transformation to a ray.*/
	template< unsigned int Dim >
	Ray< Dim > operator * ( const Matrix< Dim+1 , Dim+1 > &m , const Ray< Dim > &ray );

	/** This function prints out the ray.*/
	template< unsigned int Dim >
	std::ostream &operator << ( std::ostream &stream , const Ray< Dim > &ray )
	{
		stream << "[ " << ray.position << " ] [ " << ray.direction << " ]";
		return stream;
	}

	/** This templated class represents a bounding box.
	*** If any of the coefficients of the first corner are greater than or equal to the coefficients of the second, the bounding box is assumed to be empty. */
	template< unsigned int Dim >
	class BoundingBox
	{
		template< unsigned int _Dim >
		friend BoundingBox< _Dim > operator * ( const Matrix< _Dim+1 , _Dim+1 > & , const BoundingBox< _Dim > & );

		/** The end-points of the bounding box. */
		Point< Dim > _p[2];
	public:
		/** The default constructor */
		BoundingBox( void );

		/** This constructor creates the (minimal) bounding box containing the two points. */
		BoundingBox( const Point< Dim > &p1 , const Point< Dim > &p2 );

		/** This constructor generates the (minimal) bounding box that contains all of the points in the input array.*/
		BoundingBox( const Point< Dim > *pList , int pSize );

		/** This method returns the value of the indexed corner of the bounding box.
		*** Valid values for index are { 0 , 1 }. */
		Point< Dim > &operator[] ( int index );

		/** This method returns the value of the indexed corner of the bounding box.
		*** Valid values for index are { 0 , 1 }. */
		const Point< Dim > &operator[] ( int index ) const;

		/** This method returns the (minimal) bounding box containing the union of the two bounding boxes.
		*** If one of the bounding boxes is empty, it is ignored. */
		BoundingBox  operator +  ( const BoundingBox &b ) const;

		/** This method returns the (minimal) bounding box containing the union of the two bounding boxes.
		*** If one of the bounding boxes is empty, it is ignored. */
		BoundingBox& operator += ( const BoundingBox &b );

		/** This method returns intersection of the two bounding boxes. */
		BoundingBox  operator ^  ( const BoundingBox &b ) const;

		/** This method returns intersection of the two bounding boxes. */
		BoundingBox& operator ^= ( const BoundingBox &b );

		/** This method returns true if a point in inside the box */
		bool isInside( const Point< Dim > &p ) const;

		/** This method indicates if the bounding box is empty. */
		bool isEmpty( void ) const;

		/** This method returns the span of the intersection of the box with the ray.
		*** If the ray does not intersect the ray, it returns an empty span */
		BoundingBox< 1 > intersect( const Ray< Dim > &ray ) const;
	};

	template< unsigned int Dim > struct QuadricBoundingBoxIntersectionInfo;

	/** This templated class represents a quadric surface. */
	template< unsigned int Dim >
	class Quadric
	{
		Matrix< Dim , Dim > _Q;
		Point< Dim > _L;
		double _C;

		template< unsigned int _Dim >
		friend class Quadric;

	public:
		/** The default constructor sets the quadric to the zero function. */
		Quadric( void );

		/** Constructs a quadric with the prescribed quadratic, linear, and constant terms.
		*  (The quadric's matrix is set to the average of Q and its transpose.) */
		Quadric( const Matrix< Dim , Dim > &Q , const Point< Dim > &L , const double &C );

		/** Constructs a quadric with the prescribed matrix.*/
		Quadric( const Matrix< Dim+1 , Dim+1 > &Q );

		/** This method returns the quadric as a homogenous matrix. */
		Matrix< Dim+1 , Dim+1 > operator()( void ) const;

		/** This method returns the quadric's quadratic term. */
		Matrix< Dim , Dim > getQuadratic( void ) const;

		/** This method returns (half) the quadric's linear term. */
		Point< Dim > getLinear( void ) const;

		/** This method returnsthe quadric's constant term. */
		double getConstant( void ) const;

		/** This method sets the quadratic term*/
		void setQuadratic( Matrix< Dim , Dim > Q );

		/** This method sets the (half) linear term*/
		void setLinear( Point< Dim > L );

		/** This method sets the linear term*/
		void setConstant( double C );

		/** This method returns the evaluation of the quadric at a prescribed point. */
		double operator()( const Point< Dim > &p ) const;

		/** This method returns the one-dimensional quadric obtained by restricting the Dim-dimensional quadratic to the ray. */
		Quadric< 1 > intersect( const Ray< Dim > &ray ) const;

		/** This method sets the extremum of the quadric.
		* As it is possible for the extremum not to be unique (e.g. for a cylinder),
		* this method will return a boolean value indicating if the setting of the extremum was successfull.
		*/
		bool setExtremum( Point< Dim > &extremum ) const;

		/** This method returns the quadric in _Dim dimensions obtained by transforming this quadric.
		* The evaluation of the new quadric at the point p is the value of the original quadric at the
		* point T*p. */
		template< unsigned int _Dim >
		Quadric< _Dim > operator * ( const Matrix< Dim , _Dim > &T ) const;

		/** This method returns the translated quadric.
		* The evaluation of the translated quadric at the point p is the value of the original quadric at the
		* point p+t */
		Quadric< Dim > operator + ( const Point< Dim > &t ) const;

		/** A structure for determining if a quadric overlaps a bounding box.*/
		struct BoundingBoxOverlap;

		friend struct BoundingBoxOverlap;
	};

	template< unsigned int Dim >
	struct Quadric< Dim >::BoundingBoxOverlap
	{
		/** A constructor initializing the structure with a quadratic.*/
		BoundingBoxOverlap( const Quadric< Dim > &Q );

		/** An overloaded operator for testing if the quadric intersects a bounding box.*/
		bool operator()( const BoundingBox< Dim > & bBox ) const;
	protected:
		template< unsigned int _Dim > friend struct Quadric< _Dim >::BoundingBoxOverlap;
		Quadric< Dim > _quadric;
		Matrix< Dim , Dim > _Q , _Qinv;
		Matrix< Dim+1 , Dim > _T;
		Matrix< Dim , Dim+1 > _Tt , _Tt_Q;
		struct Quadric< Dim-1 >::BoundingBoxOverlap _boundaryInfo[Dim];

		BoundingBoxOverlap( void );
		void _set( const Quadric< Dim > &Q );
		bool _intersect( const Quadric< Dim > &Q , const BoundingBox< Dim > &bBox ) const;
	};

	template<>
	struct Quadric<1>::BoundingBoxOverlap
	{
		/** A constructor initializing the structure with a quadratic.*/
		BoundingBoxOverlap( const Quadric< 1 > &Q ){ _set(Q); }

		/** An overloaded operator for testing if the quadric intersects a bounding box.*/
		bool operator()( const BoundingBox< 1 > & bBox ) const;
	protected:
		template< unsigned int _Dim > friend struct Quadric< _Dim >::BoundingBoxOverlap;
		Quadric< 1 > _quadric;
		Matrix< 1 , 1 > _Q , _Qinv;
		Matrix< 2 , 1 > _T;
		Matrix< 1 , 2 > _Tt , _Tt_Q;

		BoundingBoxOverlap( void ){}
		void _set( const Quadric< 1 > &Q );
		bool _intersect( const Quadric< 1 > &Q , const BoundingBox< 1 > &bBox ) const;
	};

	/** This method returns the bounding box generated by first transforming the initial bounding box according to the specified transformation and then
	* finding the minimal axis-aligned bounding box containing the transformed box. */
	template< unsigned int Dim >
	BoundingBox< Dim > operator * ( const Matrix< Dim+1 , Dim+1 > &m , const BoundingBox< Dim > &b );

	/** Functionality for outputing a bounding box to a stream.*/
	template< unsigned int Dim >
	std::ostream &operator << ( std::ostream &stream , const Point< Dim > &p );

	////////////////////////////////////////////
	// Classes specialized for 2D, 3D, and 4D //
	////////////////////////////////////////////

	/** A point in 1D */
	typedef Point< 1 > Point1D;

	/** A point in 2D */
	typedef Point< 2 > Point2D;

	/** A point in 3D */
	typedef Point< 3 > Point3D;

	/** A point in 4D */
	typedef Point< 4 > Point4D;

	/** A square matrix */
	template< unsigned int Dim > using SquareMatrix = Matrix< Dim , Dim >;

	/** A 1x1 matrix */
	typedef Matrix< 1 , 1 > Matrix1D;

	/** A 2x2 matrix */
	typedef Matrix< 2 , 2 > Matrix2D;

	/** A 3x3 matrix */
	typedef Matrix< 3 , 3 > Matrix3D;

	/** A 4x4 matrix */
	typedef Matrix< 4 , 4 > Matrix4D;

	/** A plane in 2D */
	typedef Plane< 2 > Plane2D;

	/** A plane in 3D */
	typedef Plane< 3 > Plane3D;

	/** A plane in 4D */
	typedef Plane< 4 > Plane4D;

	/** A ray in 1D */
	typedef Ray< 1 > Ray1D;

	/** A ray in 2D */
	typedef Ray< 2 > Ray2D;

	/** A ray in 3D */
	typedef Ray< 3 > Ray3D;

	/** A ray in 4D */
	typedef Ray< 4 > Ray4D;

	/** A bounding box in 1D */
	typedef BoundingBox< 1 > BoundingBox1D;

	/** A bounding box in 2D */
	typedef BoundingBox< 2 > BoundingBox2D;

	/** A bounding box in 3D */
	typedef BoundingBox< 3 > BoundingBox3D;

	/** A bounding box in 4D */
	typedef BoundingBox< 4 > BoundingBox4D;

	/** This class represents a quaternion */
	class Quaternion : public Field< Quaternion > , public _InnerProductSpace< Quaternion >
	{
	public:
		/** The real component of the quaternion */
		double real;

		/** The imaginary components of the quaternion */
		Point3D imag;

		/** This constructor generates a quaternion with real value r and imaginary components i.*/
		Quaternion( double r=0 , Point3D i = Point3D() );

		/** This method returns the dot product of two quaternions.*/
		double dot( const Quaternion& q ) const;

		/** This method returns the complex conjugate of a quaternion */
		Quaternion conjugate( void ) const;

		///////////////////
		// Field methods //
		///////////////////

		/** This method returns the negation of a quaternion. */
		Quaternion additiveInverse( void ) const;

		/** This method returns the reciprocal of a quaternion. */
		Quaternion multiplicativeInverse( void ) const;

		/** This method returnts the product of a quaternion with a scalar */
		Quaternion operator * ( double scale ) const;

		/** This method returns the sum of two quaternions */
		Quaternion  operator +  ( const Quaternion& q ) const;

		/** This method returns the product of two quaternions. */
		Quaternion  operator *  ( const Quaternion& q ) const;

	};

	/** A description of the different parameterizatons for a 3x3 rotation matrix */
	namespace RotationParameters
	{
		/** The types of parameterizations */
		enum
		{
			TRIVIAL ,
			ROTATION ,
			EULER ,
			SKEW_SYMMETRIC ,
			QUATERNION ,
			COUNT
		};

		/** The names of the parameterizations */
		const std::string Names[] = { "trivial" , "closest rotation" , "euler" , "skew symmetric" , "quaternion" };
	}

	/** This abstract templated class fills in the vector space operators for the rotation parameter */
	template< typename RotationParameterType , typename ParameterType >
	class RotationParameter
	{
	public:
		ParameterType parameter;

		/** This method transforms the parameter into a rotation */
		virtual Matrix3D operator()( void ) const = 0;

		/////////////////////////
		// VectorSpace methods //
		/////////////////////////

		/** This method returns the product of the parameter with a scalar */
		RotationParameterType operator * ( double scale ) const;

		/** This method returns the sum of two parameters */
		RotationParameterType operator + ( const RotationParameterType &p ) const;
	};

	/** This class represents a parametrization of 3x3 (rotation) matrices by matrices */
	class TrivialRotationParameter : public RotationParameter< TrivialRotationParameter , Matrix3D > , public VectorSpace< TrivialRotationParameter >
	{
	public:
		/** The default constructor */
		TrivialRotationParameter( void );

		/** The constructor sets the parameters from a rotation matrix */
		TrivialRotationParameter( const Matrix3D &r );

		/** The constructor sets the parameters from a rotation matrix and the previous parameter */
		TrivialRotationParameter( const Matrix3D &r , const TrivialRotationParameter &previous );

		/** This method transforms the parameter into a rotation */
		Matrix3D operator()( void ) const;
	};

	/** This class represents a parametrization of 3x3 rotation matrices by Euler angles */
	class EulerRotationParameter : public RotationParameter< EulerRotationParameter , Point3D > ,  public VectorSpace< EulerRotationParameter >
	{
	public:
		/** The default constructor */
		EulerRotationParameter( void );

		/** The constructor sets the parameters from a rotation matrix */
		EulerRotationParameter( const Matrix3D &r );

		/** The constructor sets the parameters from a rotation matrix and the previous parameter */
		EulerRotationParameter( const Matrix3D &r , const EulerRotationParameter &previous );

		/** This method transforms the parameter into a rotation */
		Matrix3D operator()( void ) const;
	};

	/** This class represents a parametrization of 3x3 rotation matrices by 3x3 matrices */
	class MatrixRotationParameter : public RotationParameter< MatrixRotationParameter , Matrix3D > , public VectorSpace< MatrixRotationParameter >
	{
	public:
		/** The default constructor */
		MatrixRotationParameter( void );

		/** The constructor sets the parameters from a rotation matrix */
		MatrixRotationParameter( const Matrix3D &r );

		/** The constructor sets the parameters from a rotation matrix and the previous parameter */
		MatrixRotationParameter( const Matrix3D &r , const MatrixRotationParameter &previous );

		/** This method transforms the parameter into a rotation */
		Matrix3D operator()( void ) const;
	};

	/** This class represents a parametrization of 3x3 rotation matrices by skew-symmetric matrices */
	class SkewSymmetricRotationParameter : public RotationParameter< SkewSymmetricRotationParameter , Point3D > , public VectorSpace< SkewSymmetricRotationParameter >
	{
		Matrix3D _toMatrix( void ) const;
		void _fromMatrix( const Matrix3D &skew );
	public:
		/** The default constructor */
		SkewSymmetricRotationParameter( void );

		/** The constructor sets the parameters from a rotation matrix */
		SkewSymmetricRotationParameter( const Matrix3D &r );

		/** The constructor sets the parameters from a rotation matrix and the previous parameter */
		SkewSymmetricRotationParameter( const Matrix3D &r , const SkewSymmetricRotationParameter &previous );

		/** This method transforms the parameter into a rotation */
		Matrix3D operator()( void ) const;
	};

	/** This class represents a parametrization of 3x3 rotation matrices by skew-symmetric matrices */
	class QuaternionRotationParameter : public RotationParameter< QuaternionRotationParameter , Quaternion > , public VectorSpace< QuaternionRotationParameter >
	{
	public:
		/** The default constructor */
		QuaternionRotationParameter( void );

		/** The constructor sets the parameters from a rotation matrix */
		QuaternionRotationParameter( const Matrix3D &r );

		/** The constructor sets the parameters from a rotation matrix and the previous parameter */
		QuaternionRotationParameter( const Matrix3D &r , const QuaternionRotationParameter &previous );

		/** This method transforms the parameter into a rotation */
		Matrix3D operator()( void ) const;
	};

	/** This templated class represents a parametrization of a 4x4 (rigid) matrix represented as a rotation and translation */
	template< typename RotationParameterType >
	class TransformationParameter : public VectorSpace< TransformationParameter< RotationParameterType > >
	{
	public:
		/** The parametric representation of the rotation */
		RotationParameterType rotationParameter;

		/** The translation */
		Point3D translation;

		/** The default constructor */
		TransformationParameter( void );

		/** This constructor sets the parameters from a 4x4 matrix */
		TransformationParameter( const Matrix4D &m );

		/** This constructor sets the parameters from a 4x4 matrix and the previous parameter */
		TransformationParameter( const Matrix4D &m , const TransformationParameter &p );

		/** This method returns the product of the parameter with a scalar */
		TransformationParameter operator * ( double scale ) const;

		/** This method returns the sum of two parameters */
		TransformationParameter operator + ( const TransformationParameter &tp ) const;

		/** This method transforms the parameter into a rotation */
		Matrix4D operator()( void ) const;
	};
}
#include "geometry.inl"
#include "geometry.todo.inl"
#endif // GEOMETRY_INCLUDED
