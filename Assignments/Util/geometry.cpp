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

#include <cmath>
#include <algorithm>
#include <SVD/SVDFit.h>
#include <SVD/MatrixMNTC.h>
#include <Util/exceptions.h>
#include "geometry.h"

namespace Util
{
	//////////////////////
	// GlobalProperties //
	//////////////////////
	bool GlobalProperties::DebugFlag = false;

	////////////////
	// Quaternion //
	////////////////
	Quaternion::Quaternion( double r , Point3D i ) : real(r) , imag(i) {}

	Quaternion Quaternion::additiveInverse( void ) const { return Quaternion( -real , -imag ); }

	Quaternion Quaternion::multiplicativeInverse( void ) const { return conjugate() * ( 1./squareNorm() ); }

	double Quaternion::dot( const Quaternion &q ) const { return real*q.real + Point3D::Dot( imag , q.imag ); }

	Quaternion Quaternion::operator * ( double scale ) const { return Quaternion( real*scale , imag*scale ); }

	Quaternion Quaternion::operator + ( const Quaternion &q ) const { return Quaternion( real+q.real , imag + q.imag ); }

	Quaternion Quaternion::operator * ( const Quaternion &q ) const { return Quaternion( real*q.real - Point3D::Dot( imag , q.imag ) , imag*q.real + q.imag*real + Point3D::CrossProduct( imag , q.imag ) ); }

	Quaternion Quaternion::conjugate( void ) const { return Quaternion( real , -imag ); }

	//////////////////////////////
	// TrivialRotationParameter //
	//////////////////////////////
	TrivialRotationParameter::TrivialRotationParameter( void ){ parameter = Matrix3D::Identity(); }

	TrivialRotationParameter::TrivialRotationParameter( const Matrix3D &r ){ parameter = r; }

	TrivialRotationParameter::TrivialRotationParameter( const Matrix3D &r , const TrivialRotationParameter &previous ) : TrivialRotationParameter( r ) {}

	Matrix3D TrivialRotationParameter::operator()( void ) const { return parameter; }

	////////////////////////////
	// EulerRotationParameter //
	////////////////////////////
	EulerRotationParameter::EulerRotationParameter( void ){}

	EulerRotationParameter::EulerRotationParameter( const Matrix3D &r )
	{
		parameter[1] = asin(-r(2,0));
		if( sqrt( r(0,0)*r(0,0) + r(1,0)*r(1,0) )>0.000001 )
		{
			parameter[2] = atan2( r(1,0) , r(0,0) );
			parameter[0] = atan2( r(2,1) , r(2,2) );
		}
		else
		{
			parameter[2] = atan2( r(0,1) , r(2,0) );
			parameter[0] = 0;
		}
	}

	EulerRotationParameter::EulerRotationParameter( const Matrix3D &r , const EulerRotationParameter &previous ) : EulerRotationParameter( r )
	{
		for( int i=0 ; i<3 ; i++ )
		{
			double d = fabs( parameter[i] - previous.parameter[i] );
			double a = parameter[i];
			if( parameter[i]<previous.parameter[i] )
			{
				while( parameter[i]<previous.parameter[i] )
				{
					parameter[i] += 2*Pi;
					if( fabs( parameter[i]-previous.parameter[i] )<d )
					{
						d=fabs( parameter[i]-previous.parameter[i] );
						a=parameter[i];
					}
				}
			}
			else
			{
				while( parameter[i]>previous.parameter[i] )
				{
					parameter[i] -= 2*Pi;
					if( fabs( parameter[i]-previous.parameter[i] )<d )
					{
						d = fabs( parameter[i]-previous.parameter[i] );
						a = parameter[i];
					}
				}
			}
			parameter[i] = a;
		}
	}

	/////////////////////////////
	// MatrixRotationParameter //
	/////////////////////////////
	MatrixRotationParameter::MatrixRotationParameter( void ){ parameter = Matrix3D::Identity(); }

	MatrixRotationParameter::MatrixRotationParameter( const Matrix3D &r ){ parameter = r; }

	MatrixRotationParameter::MatrixRotationParameter( const Matrix3D &r , const MatrixRotationParameter &previous ) : MatrixRotationParameter( r ) {}

	Matrix3D MatrixRotationParameter::operator()( void ) const { return parameter.closestRotation(); }

	////////////////////////////////////
	// SkewSymmetricRotationParameter //
	////////////////////////////////////
	Matrix3D SkewSymmetricRotationParameter::_toMatrix( void ) const
	{
		Matrix3D skew;
		skew(2,1) = parameter[0] , skew(1,2) = -parameter[0];
		skew(0,2) = parameter[1] , skew(2,0) = -parameter[1];
		skew(1,0) = parameter[2] , skew(0,1) = -parameter[2];
		return skew;
	}

	void SkewSymmetricRotationParameter::_fromMatrix( const Matrix3D &skew )
	{
		parameter[0] = ( skew(2,1) - skew(1,2) ) / 2;
		parameter[1] = ( skew(0,2) - skew(2,0) ) / 2;
		parameter[2] = ( skew(1,0) - skew(0,1) ) / 2;
	}

	SkewSymmetricRotationParameter::SkewSymmetricRotationParameter( void ){}

	SkewSymmetricRotationParameter::SkewSymmetricRotationParameter( const Matrix3D &r ){ _fromMatrix( Matrix3D::Log( r ) ); }

	SkewSymmetricRotationParameter::SkewSymmetricRotationParameter( const Matrix3D &r , const SkewSymmetricRotationParameter &previous ) : SkewSymmetricRotationParameter( r )
	{
		// Given a non-zero skew symmetric matrix S, the matrices { S / |S| 2 k Pi } exponentiate to the identity
		// In particular, given a non-zero skew symmetric matrix S, the matrices { S * ( 1 + 2 k Pi / |S| ) } all exponentiate to the same thing

		// Clean up the old rotation's logarithm (just in case)
		const Point3D &_parameter = previous.parameter;

		// Choose the representative of parameter that is closest to _parameter
		double _n = _parameter.length() , n = parameter.length();
		if( n==0 && _n==0 ) ;
		else if( n==0 ) // If the first logarithm is zero, solve for the logarithm of the identity closest to _parameter
		{
			// Solve for k minimizing:
			//        E(k) = || _previous - _previous 2 k Pi / ||_previous|| ||^2
			//             = || _previous ||^2 + (2 k Pi )^2 - 2 < _previous , _previous > 2 k Pi / ||_previous||
			//             = || _previous ||^2 + (2 k Pi )^2 - 4 k Pi ||_previous||
			// Taking the derivative with respect to k:
			//       E'(k) = 8 Pi^2 k - 4 Pi ||_previous||
			// Setting the derivative equal to zero gives:
			//           0 = 2 Pi k - ||_previous||
			//           k = ||_previous|| / ( 2 Pi )
			double k = _n/( 2 * Pi );
			parameter = _parameter * ( 2 * floor( k+0.5 ) / _n );
		}
		else // If it's not zero, solve for the logarithm of S closest to _previous
		{
			// Solve for k minimizing:
			//        E(k) = || _previous - parameter * ( 1 + 2 k Pi / ||parameter|| ) ||^2
			//             = || _previous ||^2 + || parameter ||^2 * ( 1 + 2 k Pi / ||parameter|| )^2 - 2 < _previous , parameter > * ( 1 + 2 k Pi / ||parameter|| )
			//             = || _previous ||^2 + || parameter ||^2 - 2< _previous , S > + ( 2 k Pi )^2 - 2 < _previous , parameter > * 2 k Pi / ||parameter||
			//             = const + ( 2 k Pi )^2 - 2 < _previous , parameter > * 2 k Pi / ||parameter||
			// Taking the derivative with respect to k:
			//       E'(k) = 8 Pi^2 k - 2 < _previous , parameter > 2 Pi / ||parameter||
			//             = 8 Pi^2 k - 4 < _previous , parameter > Pi / ||parameter||
			// Setting the derivative equal to zero gives:
			//           0 = 2 Pi k - < _previous , parameter > / ||parameter||
			//           k = < _previous ,parameterS > / ( 2 Pi ||parameter|| )
			double k = Point3D::Dot( parameter , _parameter ) / ( 2 * n * Pi );
			parameter = parameter * ( 1 + 2 * floor( k+0.5 ) * Pi / n );
		}
	}

	Matrix3D SkewSymmetricRotationParameter::operator()( void ) const { return Matrix3D::Exp( _toMatrix() ); }

	/////////////////////////////////
	// QuaternionRotationParameter //
	/////////////////////////////////
	QuaternionRotationParameter::QuaternionRotationParameter( void ){ parameter = 1.; }

	QuaternionRotationParameter::QuaternionRotationParameter( const Matrix3D &R )
	{
		parameter.real = 1+R.trace();
		if( parameter.real<=Epsilon )
		{
			int i[3];

			if     ( R(0,0)>=R(1,1) && R(0,0)>=R(2,2) ) i[0] = 0 , i[1] = 1 , i[2] = 2;
			else if( R(1,1)>=R(0,0) && R(1,1)>=R(2,2) ) i[0] = 1 , i[1] = 2 , i[2] = 0;
			else if( R(2,2)>=R(0,0) && R(2,2)>=R(1,1) ) i[0] = 2 , i[1] = 0 , i[2] = 1;
			else ERROR_OUT( "Couldn't find dominating diagonal" );

			double r = sqrt( 1 + R(i[0],i[0]) - R(i[1],i[1]) - R(i[2],i[2]) );
			double s = 1./(2*r);
			parameter.real = ( R(i[1],i[2]) - R(i[2],i[1]) ) * s;
			parameter.imag[ i[0] ] = r/2;
			parameter.imag[ i[1] ] = ( R(i[1],i[0]) + R(i[0],i[1]) ) * s;
			parameter.imag[ i[2] ] = ( R(i[2],i[0]) + R(i[0],i[2]) ) * s;
		}
		else
		{
			parameter.real = 0.5 * sqrt( parameter.real );
			parameter.imag[0] = -1./(4*parameter.real) * ( R(1,2)-R(2,1) );
			parameter.imag[1] = -1./(4*parameter.real) * ( R(2,0)-R(0,2) );
			parameter.imag[2] = -1./(4*parameter.real) * ( R(0,1)-R(1,0) );
		}
		parameter = parameter.unit();
	}

	QuaternionRotationParameter::QuaternionRotationParameter( const Matrix3D &r , const QuaternionRotationParameter &previous ) : QuaternionRotationParameter( r )
	{
		if( ( parameter-previous.parameter ).squareNorm()>( parameter+previous.parameter ).squareNorm() ) parameter *= -1.;
	}
}