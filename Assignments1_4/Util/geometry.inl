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

#include <algorithm>
#include <SVD/SVDFit.h>
#include <SVD/MatrixMNTC.h>
#include <Util/exceptions.h>

namespace Util
{
	///////////
	// Point //
	///////////
	template< unsigned int Dim >
	void Point< Dim >::_init( const double *values , unsigned int sz )
	{
		if     ( sz==0   ) memset( _p , 0 , sizeof(_p) );
		else if( sz==Dim ) memcpy( _p , values , sizeof(_p) );
		else ERROR_OUT( "Should never be called" );
	}

	template< unsigned int Dim >
	Point< Dim >::Point( void ){ memset( _p , 0 , sizeof(_p) ); }

	template< unsigned int Dim >
	Point< Dim >::Point( const Point &p ){ memcpy( _p , p._p , sizeof(_p) ); }

	template< unsigned int Dim >
	template< typename ... Doubles >
	Point< Dim >::Point( Doubles ... values )
	{
		static_assert( sizeof...(values)==Dim || sizeof...(values)==0 , "[ERROR] Point< Dim >::Point: Invalid number of coefficients" );
		const double _values[] = { static_cast< double >( values )... };
		_init( _values , sizeof...(values) );
	}

	template< unsigned int Dim >
	Point< Dim > Point< Dim >::operator * ( double s ) const { Point p ; for( int i=0 ; i<Dim ; i++ ) p._p[i] = _p[i] * s ; return p; }

	template< unsigned int Dim >
	Point< Dim > Point< Dim >::operator + ( const Point &p ) const { Point q ; for( int i=0 ; i<Dim ; i++ ) q._p[i] += _p[i] + p._p[i] ; return q; }

	template< unsigned int Dim >
	double Point< Dim >::dot( const Point &q ) const
	{
		double dot = 0;
		for( int i=0 ; i<Dim ; i++ ) dot += _p[i] * q._p[i];
		return dot;
	}

	template< unsigned int Dim >
	double& Point< Dim >::operator[] ( int i ){ return _p[i]; }

	template< unsigned int Dim >
	const double &Point< Dim >::operator[] ( int i ) const { return _p[i]; }

	template< unsigned int Dim >
	Point< Dim >  Point< Dim >::operator * ( const Point &q ) const
	{
		Point p;
		for( int i=0 ; i<Dim ; i++ ) p[i] = _p[i]*q._p[i];
		return p;
	}

	template< unsigned int Dim >
	Point< Dim >  Point< Dim >::operator / ( const Point &q ) const
	{
		Point p;
		for( int i=0 ; i<Dim ; i++ ) p[i] = _p[i]/q._p[i];
		return p;
	}

	template< unsigned int Dim >
	Point< Dim > &Point< Dim >::operator *= ( const Point &q ){	return (*this) = (*this) * q; }

	template< unsigned int Dim >
	Point< Dim > &Point< Dim >::operator /= ( const Point &q ){	return (*this) = (*this) / q; }

	template< unsigned int Dim >
	template< typename ... Points >
	Point< Dim > Point< Dim >::CrossProduct( Points ... points )
	{
		static_assert( sizeof ... ( points )==Dim-1 , "[ERROR] Number of points in cross-product must be one less than the dimension" );
		const Point< Dim > _points[] = { points ... };
		return CrossProduct( _points );
	}

	template< unsigned int Dim >
	Point< Dim > Point< Dim >::CrossProduct( Point *points ){ return CrossProduct( (const Point *)points );}

	template< unsigned int Dim >
	Point< Dim > Point< Dim >::CrossProduct( const Point *points )
	{
		Matrix< Dim , Dim > M;
		for( int d=0 ; d<Dim ; d++ ) for( int c=0 ; c<Dim-1 ; c++ ) M(d,c) = points[c][d];
		Point p;
		for( int d=0 ; d<Dim ; d++ ) p[d] = ( d&1 ) ? -M.subDeterminant( d , Dim-1 ) : M.subDeterminant( d , Dim-1 );
		return p;
	}

	template< unsigned int Dim >
	std::ostream &operator << ( std::ostream &stream , const Point< Dim > &p )
	{
		for( int i=0 ; i<Dim-1 ; i++ ) stream << p[i] << " ";
		stream << p[Dim-1];
		return stream;
	}
	template< unsigned int Dim >
	std::istream &operator >> ( std::istream &stream , Point< Dim > &p )
	{
		for( int i=0 ; i<Dim ; i++ ) stream >> p[i];
		return stream;
	}

	/////////////////
	// _BaseMatrix //
	/////////////////
	template< unsigned int Rows , unsigned int Cols , typename MatrixType , typename MatrixTransposeType >
	double _BaseMatrix< Rows , Cols , MatrixType , MatrixTransposeType >::dot( const _BaseMatrix &m ) const
	{
		double dot = 0;
		for( int r=0 ; r<Rows ; r++ ) for( int c=0 ; c<Cols ; c++ ) dot += operator()(r,c) * m(r,c);
		return dot;
	}

	template< unsigned int Rows , unsigned int Cols , typename MatrixType , typename MatrixTransposeType >
	_BaseMatrix< Rows , Cols , MatrixType , MatrixTransposeType >::_BaseMatrix( void ){ memset( _m , 0 , sizeof(_m) ); }

	template< unsigned int Rows , unsigned int Cols , typename MatrixType , typename MatrixTransposeType >
	double& _BaseMatrix< Rows , Cols , MatrixType , MatrixTransposeType >::operator() ( int r , int c ) { return _m[r][c]; }

	template< unsigned int Rows , unsigned int Cols , typename MatrixType , typename MatrixTransposeType >
	const double &_BaseMatrix< Rows , Cols , MatrixType , MatrixTransposeType >::operator() ( int r , int c ) const { return _m[r][c]; }

	template< unsigned int Rows , unsigned int Cols , typename MatrixType , typename MatrixTransposeType >
	MatrixTransposeType _BaseMatrix< Rows , Cols , MatrixType , MatrixTransposeType >::transpose( void ) const
	{
		MatrixTransposeType n;
		for( int r=0 ; r<Rows ; r++ ) for( int c=0 ; c<Cols ; c++ ) n(c,r) = operator()(r,c);
		return n;
	}

	template< unsigned int Rows , unsigned int Cols , typename MatrixType , typename MatrixTransposeType >
	Point< Rows > _BaseMatrix< Rows , Cols , MatrixType , MatrixTransposeType >::operator * ( const Point< Cols > &p ) const
	{
		Point< Rows > q;
		for( int r=0 ; r<Rows ; r++ ) for( int c=0 ; c<Cols ; c++ ) q[r] += operator()(r,c) * p[c];
		return q;
	}

	template< unsigned int Rows , unsigned int Cols , typename MatrixType , typename MatrixTransposeType >
	MatrixType _BaseMatrix< Rows , Cols , MatrixType , MatrixTransposeType >::operator * ( double s ) const { MatrixType n ; for( int r=0 ; r<Rows ; r++ ) for( int c=0 ; c<Cols ; c++ ) n(r,c) = operator()(r,c) * s ; return n; }

	template< unsigned int Rows , unsigned int Cols , typename MatrixType , typename MatrixTransposeType >
	MatrixType _BaseMatrix< Rows , Cols , MatrixType , MatrixTransposeType >::operator + ( const MatrixType &m ) const { MatrixType n ; for( int r=0 ; r<Rows ; r++ ) for( int c=0 ; c<Cols ; c++ ) n(r,c) = operator()(r,c) + m(r,c) ; return n; }

	////////////
	// Matrix //
	////////////
	template< unsigned int Rows , unsigned int Cols > Matrix< Rows , Cols >::Matrix( void ) : _BaseMatrix< Rows , Cols , Matrix< Rows , Cols > , Matrix< Cols , Rows > >() {}

	template< unsigned int Rows , unsigned int Cols >
	template< unsigned int _Cols >
	Matrix< Rows , _Cols > Matrix< Rows , Cols >::operator * ( const Matrix< Cols , _Cols > &m ) const
	{
		Matrix< Rows , _Cols > n;
		for( int r=0 ; r<Rows ; r++ ) for( int c=0 ; c<_Cols ; c++ ) for( int i=0 ; i<Cols ; i++ ) n(r,c) += operator()(r,i) * m(i,c);
		return n;
	}

	//////////////////
	// SquareMatrix //
	//////////////////
	template< unsigned int Dim > SquareMatrix< Dim >::Matrix( void ) : _BaseMatrix< Dim , Dim , SquareMatrix< Dim > , SquareMatrix< Dim > >() {}
	template< unsigned int Dim >
	SquareMatrix< Dim >::Matrix( const SquareMatrix< Dim+1 > &n ){ for( int i=0 ; i<Dim ; i++ ) for( int j=0 ; j<Dim ; j++ ) operator()(i,j) = n(i,j); }

	template< unsigned int Dim >
	SquareMatrix< Dim >::Matrix( const SquareMatrix< Dim-1 > &n , Point< Dim-1 > p ) : Matrix()
	{
		for( int i=0 ; i<Dim-1 ; i++ ) for( int j=0 ; j<Dim-1 ; j++ ) operator()(i,j) = n(i,j);
		operator()(Dim-1,Dim-1) = 1.;
		for( int i=0 ; i<Dim-1 ; i++ ) operator()(i,Dim-1) = p[i];
	}

	template< unsigned int Dim >
	template< unsigned int Cols >
	Matrix< Dim , Cols > SquareMatrix< Dim >::operator * ( const Matrix< Dim , Cols > &m ) const
	{
		Matrix< Dim , Cols > n;
		for( int r=0 ; r<Dim ; r++ ) for( int c=0 ; c<Cols ; c++ ) for( int i=0 ; i<Dim ; i++ ) n(r,c) += operator()(r,i) * m(i,c);
		return n;
	}

	template< unsigned int Dim >
	double SquareMatrix< Dim >::subDeterminant( int r , int c ) const
	{
		SquareMatrix< Dim-1 > m;
		int rr[Dim-1] , cc[Dim-1];
		for( int a=0 , _r=0 , _c=0 ; a<Dim ; a++ )
		{
			if( a!=r ) rr[_r++] = a;
			if( a!=c ) cc[_c++] = a;
		}
		for( int _c=0 ; _c<Dim-1 ; _c++ ) for( int _r=0 ; _r<Dim-1 ; _r++ ) m(_r,_c) = operator()( rr[_r] , cc[_c] );
		return m.determinant();
	}

	template< unsigned int Dim >
	double SquareMatrix< Dim >::determinant( void ) const
	{
		double det = 0.;
		for( int d=0 ; d<Dim ; d++ ) 
			if( d&1 ) det -= operator()(0,d) * subDeterminant( 0 , d );
			else      det += operator()(0,d) * subDeterminant( 0 , d );
		return det;
	}

	template<>
	inline double SquareMatrix< 3 >::determinant( void ) const
	{
		return
			operator()(0,0)*( operator()(1,1)*operator()(2,2) - operator()(1,2)*operator()(2,1) ) -
			operator()(0,1)*( operator()(1,0)*operator()(2,2) - operator()(1,2)*operator()(2,0) ) +
			operator()(0,2)*( operator()(1,0)*operator()(2,1) - operator()(1,1)*operator()(2,0) );
	}

	template<>
	inline double SquareMatrix< 2 >::determinant( void ) const { return operator()(0,0)*operator()(1,1) - operator()(0,1)*operator()(1,0); }

	template<>
	inline double SquareMatrix< 1 >::determinant( void ) const { return operator()(0,0); }

	template< unsigned int Dim >
	double SquareMatrix< Dim >::trace( void ) const
	{
		double tr = 0;
		for( int i=0 ; i<Dim ; i++ ) tr += operator()(i,i);
		return tr;
	}
	template< unsigned int Dim >
	SquareMatrix< Dim > SquareMatrix< Dim >::inverse( void ) const
	{
		Matrix inv;
		if( !setInverse( inv ) ) THROW( " singular matrix" );
		return inv;
	}

	template< unsigned int Dim >
	bool SquareMatrix< Dim >::setInverse( Matrix &inv ) const
	{
		double d = determinant();
		if( !d ) return false;
		for( int i=0 ; i<Dim ; i++ ) for( int j=0 ; j<Dim ; j++ )
			if( (i+j)%2==0 ) inv(i,j) =  subDeterminant( j , i ) / d;
			else             inv(i,j) = -subDeterminant( j , i ) / d;
		return true;
	}

	template<>
	inline bool SquareMatrix< 3 >::setInverse( Matrix &inv ) const
	{
		double det = determinant();
		if( !det ) return false;
		inv(0,0) =  ( operator()(1,1)*operator()(2,2) - operator()(1,2)*operator()(2,1) ) / det;
		inv(0,1) = -( operator()(0,1)*operator()(2,2) - operator()(2,1)*operator()(0,2) ) / det;
		inv(0,2) =  ( operator()(0,1)*operator()(1,2) - operator()(0,2)*operator()(1,1) ) / det;
		inv(1,0) = -( operator()(1,0)*operator()(2,2) - operator()(1,2)*operator()(2,0) ) / det;
		inv(1,1) =  ( operator()(0,0)*operator()(2,2) - operator()(0,2)*operator()(2,0) ) / det;
		inv(1,2) = -( operator()(0,0)*operator()(1,2) - operator()(0,2)*operator()(1,0) ) / det;
		inv(2,0) =  ( operator()(1,0)*operator()(2,1) - operator()(1,1)*operator()(2,0) ) / det;
		inv(2,1) = -( operator()(0,0)*operator()(2,1) - operator()(0,1)*operator()(2,0) ) / det;
		inv(2,2) =  ( operator()(0,0)*operator()(1,1) - operator()(0,1)*operator()(1,0) ) / det;

		return true;
	}

	template<>
	inline bool SquareMatrix< 2 >::setInverse( Matrix &inv ) const
	{
		double det = determinant();
		if( !det ) return false;
		inv(0,0) =  operator()(1,1) / det;
		inv(1,1) =  operator()(0,0) / det;
		inv(1,0) = -operator()(1,0) / det;
		inv(0,1) = -operator()(0,1) / det;

		return true;
	}

	template<>
	inline bool SquareMatrix< 1 >::setInverse( Matrix &inv ) const
	{
		double det = operator()(0,0);
		if( !det ) return false;
		inv(0,0) = 1./det;
		return true;
	}

	template< unsigned int Dim >
	Point< Dim-1 > SquareMatrix< Dim >::operator * ( const Point< Dim-1 > &p ) const
	{
		Point< Dim > q;
		for( int i=0 ; i<Dim-1 ; i++ ) q[i] = p[i];
		q[Dim-1] = 1;
		q = (*this) * q;
		Point< Dim-1 > _q;
		for( int i=0 ; i<Dim-1 ; i++ ) _q[i] = q[i] / q[Dim-1];
		return _q;
	}

	template< unsigned int Dim >
	SquareMatrix< Dim > SquareMatrix< Dim >::Identity( void )
	{
		Matrix m;
		for( int i=0 ; i<Dim ; i++ ) m(i,i) = 1;
		return m;
	}

	template< unsigned int Dim >
	void SquareMatrix< Dim >::SVD( Matrix& r1 , Matrix& d , Matrix& r2 ) const
	{
		GXMatrixMNd M( Dim , Dim );
		GXMatrixMNd U, W, Vt;

		d = r1 = r2 = Identity();

		for( int i=0 ; i<Dim ; i++ ) for(  int j=0 ; j<Dim ; j++ ) M(i,j) = operator()(i,j);
		SVDMat( M , U , W , Vt );  // M == U . DiagonalMatrix(W) . Vt
		for( int i=0 ; i<Dim ; i++ )
		{
			for( int j=0 ; j<Dim ; j++ ) r1(j,i) = U(j,i) , r2(j,i) = Vt(j,i);
			d(i,i) = W(i,0);
		}
	}

	// Code borrowed from:
	// Linear Combination of Transformations
	// Marc Alexa
	template< unsigned int Dim >
	SquareMatrix< Dim > SquareMatrix< Dim >::SquareRoot( const Matrix& m , double eps )
	{
		Matrix X,Y;
		X = m;
		Y = Identity();
		while( (X*X-m).squareNorm()>eps*eps )
		{
			Matrix iX = X.inverse();
			Matrix iY = Y.inverse();
			X = (X+iY) / 2;
			Y = (Y+iX) / 2;
		}
		return X;
	}

	template< unsigned int Dim >
	SquareMatrix< Dim > SquareMatrix< Dim >::Log( const Matrix& m , double eps )
	{
		Matrix I = Identity();
		Matrix X , Z , A=m;
		int k=0;
		while( (A-I).squareNorm()>0.25 )
		{
			A = SquareRoot( A , eps );
			k++;
		}
		A = I-A;
		X = Z = A;
		int i = 1;
		while( Z.squareNorm()>eps*eps )
		{
			Z = Z*A;
			i++;
			X += Z*( 1.0/i );
		}
		return X * ( -pow( 2.0 , (double)k ) );
	}

	template< unsigned int Dim >
	SquareMatrix< Dim > SquareMatrix< Dim >::symmetrize( void ) const { return ( (*this)+transpose() ) / 2; }

	template< unsigned int Dim >
	SquareMatrix< Dim > SquareMatrix< Dim >::skewSymmetrize( void ) const { return ( (*this)-transpose() ) / 2; }

	template< unsigned int Rows , unsigned int Cols , typename MatrixType , typename MatrixTransposeType >
	std::ostream &operator << ( std::ostream &stream , const _BaseMatrix< Rows , Cols , MatrixType , MatrixTransposeType > &m )
	{
		for( unsigned int c=0 ; c<Cols ; c++ ) for( unsigned int r=0 ; r<Rows ; r++ )
		{
			stream << m(r,c);
			if( r!=Rows-1 || c!=Cols-1 ) stream << " ";
		}
		return stream;
	}

	template< unsigned int Rows , unsigned int Cols , typename MatrixType , typename MatrixTransposeType >
	std::istream &operator >> ( std::istream &stream , _BaseMatrix< Rows , Cols , MatrixType , MatrixTransposeType > &m )
	{
		for( unsigned int c=0 ; c<Cols ; c++ ) for( unsigned int r=0 ; r<Rows ; r++ ) stream >> m(r,c); 
		return stream;
	}

	///////////
	// Plane //
	///////////
	template< unsigned int Dim >
	Plane< Dim >::Plane( void ) : distance(0) {}

	template< unsigned int Dim >
	Plane< Dim >::Plane( const Point< Dim > &n , const Point< Dim > &p )
	{
		normal = n.unit();
		distance = Point< Dim >::Dot( normal , p );
	}

	template< unsigned int Dim >
	template< typename ... Points >
	Plane< Dim >::Plane( Points ... points )
	{
		static_assert( sizeof ... ( points )==Dim , "[ERROR] Number of points in plane constructor must equal the dimension" );
		const Point< Dim > _points[] = { points ... };
		(*this) = Plane( _points );
	}

	template< unsigned int Dim >
	Plane< Dim >::Plane( Point< Dim > *points ) : Plane( (const Point< Dim > *)points ) {}

	template< unsigned int Dim >
	Plane< Dim >::Plane( const Point< Dim > *points )
	{
		Point< Dim > _points[Dim-1];
		for( int i=1 ; i<Dim ; i++ ) _points[i-1] = points[i] - points[0];
		normal = Point< Dim >::CrossProduct( _points ).unit();
		distance = Point< Dim >::Dot( normal , points[0] );
	}

	template< unsigned int Dim >
	double Plane< Dim >::operator() ( const Point< Dim > &p ) const
	{
		return Point< Dim >::Dot( normal , p ) - distance;
	}

	/////////
	// Ray //
	/////////
	template< unsigned int Dim >
	Ray< Dim >::Ray( void ){}

	template< unsigned int Dim >
	Ray< Dim >::Ray( const Point< Dim > &p , const Point< Dim > &d ) : position(p) , direction(d) {}

	template< unsigned int Dim >
	Point< Dim > Ray< Dim >::operator() ( double s ) const { return position+direction*s; }

	template< unsigned int Dim >
	Ray< Dim >  Ray< Dim >::operator +  ( const Point< Dim > &p ) const { return Ray( position+p , direction );}

	template< unsigned int Dim >
	Ray< Dim > &Ray< Dim >::operator += ( const Point< Dim > &p ){ position += p ; return *this; }

	template< unsigned int Dim >
	Ray< Dim >  Ray< Dim >::operator -  ( const Point< Dim > &p ) const { return Ray( position-p , direction );}

	template< unsigned int Dim >
	Ray< Dim > &Ray< Dim >::operator -= ( const Point< Dim > &p ){ position -= p ; return *this; }

	template< unsigned int Dim >
	Ray< Dim > operator * ( const Matrix< Dim+1 , Dim+1 > &m , const Ray< Dim >& r )
	{
		return Ray< Dim >( m * r.position , Matrix< Dim , Dim >(m) * r.direction );
	}


	/////////////////
	// BoundingBox //
	/////////////////
	template< unsigned int Dim >
	BoundingBox< Dim >::BoundingBox( void ){}

	template< unsigned int Dim >
	BoundingBox< Dim >::BoundingBox( const Point< Dim > &p1 , const Point< Dim > &p2 )
	{
		for( int d=0 ; d<Dim ; d++ ) _p[0][d] = std::min< double >( p1[d] , p2[d] ) , _p[1][d] = std::max< double >( p1[d] , p2[d] );
	}

	template< unsigned int Dim >
	BoundingBox< Dim >::BoundingBox( const Point< Dim > *pList , int pSize )
	{
		if( pSize>0 )
		{
			_p[0] = _p[1] = pList[0];
			for( int i=1 ; i<pSize ; i++ ) for( int j=0 ; j<Dim ; j++ ) _p[0][j] = std::min< double >( _p[0][j] , pList[i][j] ) , _p[1][j] = std::max< double >( _p[1][j] , pList[i][j] );
		}
	}

	template< unsigned int Dim >
	Point< Dim > &BoundingBox< Dim >::operator[] ( int idx ){ return _p[idx]; }

	template< unsigned int Dim >
	const Point< Dim > &BoundingBox< Dim >::operator[] ( int idx ) const { return _p[idx]; }

	template< unsigned int Dim >
	BoundingBox< Dim > BoundingBox< Dim >::operator + ( const BoundingBox &b ) const
	{
		Point< Dim > pList[4];
		Point< Dim > q;

		if( b.isEmpty() ) return *this;
		if(   isEmpty() ) return b;
		pList[0] = _p[0];
		pList[1] = _p[1];
		pList[2] = b._p[0];
		pList[3] = b._p[1];
		return BoundingBox( pList , 4 );
	}

	template< unsigned int Dim >
	BoundingBox< Dim > &BoundingBox< Dim >::operator += ( const BoundingBox &b ){ return (*this) = (*this) + b; }

	template< unsigned int Dim >
	BoundingBox< Dim > BoundingBox< Dim >::operator ^ ( const BoundingBox &b ) const
	{

		if( isEmpty() || b.isEmpty() ) return BoundingBox();
		BoundingBox _b;
		for( int j=0 ; j<Dim ; j++ ) _b._p[0][j] = std::max< double >( _p[0][j] , b._p[0][j] ) , _b._p[1][j] = std::min< double >( _p[1][j] , b._p[1][j] );
		if( _b.isEmpty() ) _b._p[0] = _b._p[1] = ( _b._p[0] + _b._p[1] ) / 2;
		return _b;
	}

	template< unsigned int Dim >
	BoundingBox< Dim > &BoundingBox< Dim >::operator ^= ( const BoundingBox &b ){ return (*this) = (*this) ^ b; }

	template< unsigned int Dim >
	BoundingBox< Dim > operator * ( const Matrix< Dim+1 , Dim+1 > &m , const BoundingBox< Dim > &b )
	{
		Point< Dim > v[1<<Dim];
		for( int idx=0 ; idx<(1<<Dim) ; idx++ )
		{
			Point< Dim > p;
			for( int d=0 ; d<Dim ; d++ ) p[d] = b[(idx>>d)&1][d];
			v[idx] = m * p;
		}
		return BoundingBox< Dim >( v , 1<<Dim );
	}

	template< unsigned int Dim >
	bool BoundingBox< Dim >::isInside( const Point< Dim > &p ) const
	{
		for( int d=0 ; d<Dim ; d++ ) if( p[d]<=_p[0][d] || p[d]>=_p[1][d] ) return false;
		return true;
	}

	template< unsigned int Dim >
	bool BoundingBox< Dim >::isEmpty( void ) const
	{
		for( int d=0 ; d<Dim ; d++ ) if( _p[0][d]>=_p[1][d] ) return true;
		return false;
	}

	template< unsigned int Dim >
	std::ostream &operator << ( std::ostream &stream , const BoundingBox< Dim > &b )
	{
		stream << "[ " << b[0] << " ] [ " << b[1] << " ]";
		return stream;
	}

	/////////////
	// Quadric //
	/////////////
	template< unsigned int Dim >
	Quadric< Dim >::Quadric( void ) : _C(0) {}

	template< unsigned int Dim >
	Quadric< Dim >::Quadric( const Matrix< Dim , Dim > &Q , const Point< Dim > &L , const double &C ) :_Q(Q) , _L(L) , _C(C) {}

	template< unsigned int Dim >
	Quadric< Dim >::Quadric( const Matrix< Dim+1 , Dim+1 > &Q )
	{
		_C = Q(Dim,Dim);
		for( unsigned int i=0 ; i<Dim ; i++ )
		{
			_L[i] = ( Q(i,Dim) + Q(Dim,i) ) / 2.;
			for( unsigned int j=0 ; j<Dim ; j++ ) _Q(i,j) = ( Q(i,j) + Q(j,i) ) / 2.;
		}
	}

	template< unsigned int Dim >
	SquareMatrix< Dim+1 > Quadric< Dim >::operator()( void ) const
	{
		SquareMatrix< Dim+1 > Q;
		Q(Dim,Dim) = _C;
		for( unsigned int i=0 ; i<Dim ; i++ )
		{
			Q(Dim,i) = Q(i,Dim) = _L[i];
			for( unsigned int j=0 ; j<Dim ; j++ ) Q(i,j) = _Q(i,j);
		}
		return Q;
	}

	template< unsigned int Dim >
	Matrix< Dim , Dim > Quadric< Dim >::getQuadratic( void ) const { return _Q; }

	template< unsigned int Dim >
	Point< Dim > Quadric< Dim >::getLinear( void ) const { return _L; }

	template< unsigned int Dim >
	double Quadric< Dim >::getConstant( void ) const { return _C; }

	template< unsigned int Dim >
	void Quadric< Dim >::setQuadratic( Matrix< Dim , Dim > Q ){ _Q = ( Q + Q.transpose() )/2.; }

	template< unsigned int Dim >
	void Quadric< Dim >::setLinear( Point< Dim > L ){ _L = L; }

	template< unsigned int Dim >
	void Quadric< Dim >::setConstant( double C ){ _C = C; }

	template< unsigned int Dim >
	double Quadric< Dim >::operator()( const Point< Dim > &p ) const { return Point< Dim >::Dot( p , _Q*p ) + 2. * Point< Dim >::Dot( p , _L ) + _C; }

	template< unsigned int Dim >
	template< unsigned int _Dim >
	Quadric< _Dim > Quadric< Dim >::operator * ( const Matrix< Dim , _Dim > &T ) const
	{
		// Q(p) = _Q( T(p) )
		//      = [ T * p ]^t * _Q._Q * [ T * p ] + 2 * _L^t * T * p + _C
		//      = p^t * [ T^t *_Q._Q * T ] *p + 2 ( [T^t*_L]^t * p ) + _C
		Quadric< _Dim > Q;
		Q._Q = T.transpose() * _Q * T;
		Q._L = T.transpose() * _L;
		Q._C = _C;
		return Q;
	}

	template< unsigned int Dim >
	Quadric< Dim > Quadric< Dim >::operator + ( const Point< Dim > &t ) const
	{
		// Q(p) = _Q( p+t )
		//      = [ p + t ]^t * _Q._Q * [ p + t ] + 2 * _Q._L^t * [ p + t ] + _Q._C
		//      = p^t * _Q._Q *p + 2 ( [ _Q._Q^t * t + _Q._L ]^t * p ) + _Q._C + t^t * _Q._Q * t + 2 * _Q._L^t * t
		Quadric< Dim > Q;
		Q._Q = _Q;
		Q._L = _Q*t + _L;
		Q._C = _C + Point< Dim >::Dot( t , _Q*t ) + 2. * Point< Dim >::Dot( _L , t );
		return Q;
	}

	template< unsigned int Dim >
	bool Quadric< Dim >::setExtremum( Point< Dim > &extremum ) const
	{
		// The equation for the quardic is given by
		//		Q(p) = p^t * _Q *p + 2 * < _L , p > + c
		// The gradient is:
		//		\nabla Q|_p = 2 * _Q * p + 2 * _L
		// Setting the gradient to zero gives:
		//		p = - _Q^{-1} * _L
		try{ extremum = -_Q.inverse() * _L; }
		catch( std::exception & ){ return false; }
		return true;
	}

	/////////////////////////////////
	// Quadric::BoundingBoxOverlap //
	/////////////////////////////////
	template< unsigned int Dim >
	Quadric< Dim >::BoundingBoxOverlap::BoundingBoxOverlap( void ){}

	template< unsigned int Dim >
	Quadric< Dim >::BoundingBoxOverlap::BoundingBoxOverlap( const Quadric< Dim > &Q ){ _set( Q ); }

	template< unsigned int Dim >
	bool Quadric< Dim >::BoundingBoxOverlap::operator()( const BoundingBox< Dim > & bBox ) const { return _intersect( _quadric , bBox ); }

	template< unsigned int Dim >
	void Quadric< Dim >::BoundingBoxOverlap::_set( const Quadric< Dim > &Q )
	{
		_quadric = Q;
		_Q = Q._Q;
		try{ _Qinv = _Q.inverse(); }
		catch(...){}
		for( unsigned int d=0 ; d<Dim ; d++ )
		{
			unsigned idx = 0;
			for( unsigned int dd=0 ; dd<Dim ; dd++ ) if( dd!=d ) _boundaryInfo[d]._T(dd,idx++) = 1.;
			_boundaryInfo[d]._Tt = _boundaryInfo[d]._T.transpose();
			_boundaryInfo[d]._set( Q * _boundaryInfo[d]._T );
			_boundaryInfo[d]._Tt_Q = _boundaryInfo[d]._Tt * _Q;
		}
	}

	template< unsigned int Dim >
	bool Quadric< Dim >::BoundingBoxOverlap::_intersect( const Quadric< Dim > &Q , const BoundingBox< Dim > &bBox ) const
	{
		// Intersection can be found by finding the minimizer of the quadric within the cube and testing if it evaluates to a positive
		// or negative value.
		// The minimizer will be:
		// 1. The point at which the gradient vanishes, if that point is inside the cube. Or,
		// 2. A point on the face at which the gradient vanishes

		// Case 2:
		{
			for( unsigned int d=0 ; d<Dim ; d++ )
			{
				BoundingBox< Dim-1 > _bBox;

				// Compute the bounding box and the transformation mapping the (Dim-1)-dimensional plane containing the face into Dim-dimensional space

				// Example:
				// We have (x,z) -> (x,c,z)
				// Setting
				//     | 1 0 |
				// T = | 0 0 |
				//     | 0 1 |
				// and
				//     | 0 |
				// v = | c |
				//     | 0 |
				// We get:
				//		_Q(_p) = Q( T*_p + v )
				//		       = _p^t * [ T^t * Q._Q * T ] * _p + 2 * < T*_p + v , Q._L > + 2 * < Q._Q*T*_p , v > + _C + < v , Q._Q * v >
				//		       = _p^t * [ T^t * Q._Q * T ] * _p + 2 * < _p , T^t*Q._L > + 2 * < _p , T^t*Q._Q*v > + _C + < v , Q._Q * v > + 2 * < v , Q._L >

				{
					unsigned idx = 0;
					for( unsigned int dd=0 ; dd<Dim ; dd++ ) if( dd!=d )
					{
						_bBox[0][idx] = bBox[0][dd];
						_bBox[1][idx] = bBox[1][dd];
						idx++;
					}
				}

				Quadric< Dim-1 > _Q;
				_Q._Q = _boundaryInfo[d]._Q;

				Point< Dim-1 > _L = _boundaryInfo[d]._Tt * Q._L;
				Point< Dim-1 > _v;
				for( unsigned int i=0 ; i<Dim-1 ; i++ ) _v[i] = _boundaryInfo[d]._Tt_Q(i,d);

				for( unsigned int o=0 ; o<2 ; o++ )
				{
					double v = bBox[o][d];
					_Q._L = _L + _v * v;
					_Q._C =  Q._C + v * v * Q._Q(d,d) + 2. * v * Q._L[d];
					if( _boundaryInfo[d]._intersect( _Q , _bBox ) ) return true;
				}
			}
		}

		// Case 1:
		Point< Dim > extremum = -_Qinv * Q._L;
		return bBox.isInside( extremum ) && Q( extremum )<0;
	}

	inline void Quadric< 1 >::BoundingBoxOverlap::_set( const Quadric< 1 > &Q )
	{
		_quadric = Q;
		_Q = Q._Q;
		try{ _Qinv = _Q.inverse(); } catch(...){}
	}

	inline bool Quadric< 1 >::BoundingBoxOverlap::_intersect( const Quadric< 1 > &Q , const BoundingBox< 1 > &bBox ) const
	{
		if( Q(bBox[0])<0 || Q(bBox[1])<0 ) return true;
		Point< 1 > extremum = -_Qinv * Q._L;
		return extremum[0]>bBox[0][0] && extremum[0]<bBox[1][0] && Q(extremum)<0;
	}

	///////////////////////
	// RotationParameter //
	///////////////////////
	template< typename RotationParameterType , typename ParameterType >
	RotationParameterType RotationParameter< RotationParameterType , ParameterType >::operator * ( double scale ) const
	{
		RotationParameterType p;
		p.parameter = parameter * scale;
		return p;
	}

	template< typename RotationParameterType , typename ParameterType >
	RotationParameterType RotationParameter< RotationParameterType , ParameterType >::operator + ( const RotationParameterType &p ) const
	{
		RotationParameterType _p;
		_p.parameter = parameter + p.parameter;
		return _p;
	}

	/////////////////////////////
	// TransformationParameter //
	/////////////////////////////
	template< typename RotationParameterType >
	TransformationParameter< RotationParameterType > TransformationParameter< RotationParameterType >::operator * ( double scale ) const
	{
		TransformationParameter tp = *this;
		tp.rotationParameter *= scale;
		tp.translation *= scale;
		return tp;
	}

	template< typename RotationParameterType >
	TransformationParameter< RotationParameterType > TransformationParameter< RotationParameterType >::operator + ( const TransformationParameter &tp ) const
	{
		TransformationParameter _tp = *this;
		_tp.rotationParameter += tp.rotationParameter;
		_tp.translation += tp.translation;
		return _tp;
	}

	template< typename RotationParameterType >
	TransformationParameter< RotationParameterType >::TransformationParameter( void ) {}

	template< typename RotationParameterType >
	TransformationParameter< RotationParameterType >::TransformationParameter( const Matrix4D &m ) : rotationParameter( Matrix3D( m ) )
	{
		for( int i=0 ; i<3 ; i++ ) translation[i] = m(i,3);
	}

	template< typename RotationParameterType >
	TransformationParameter< RotationParameterType >::TransformationParameter( const Matrix4D &m , const TransformationParameter &p ) : rotationParameter( Matrix3D( m ) , p.rotationParameter )
	{
		for( int i=0 ; i<3 ; i++ ) translation[i] = m(i,3);
	}

	template< typename RotationParameterType >
	Matrix4D TransformationParameter< RotationParameterType >::operator () ( void ) const
	{
		return Matrix4D( rotationParameter() , translation );
	}
}