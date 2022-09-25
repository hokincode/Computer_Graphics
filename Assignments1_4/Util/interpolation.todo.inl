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

#include <math.h>
#include <Util/exceptions.h>

namespace Util
{
	///////////////////
	// Interpolation //
	///////////////////
	template< typename SampleType >
	SampleType Interpolation::Sample( const std::vector< SampleType > &samples , double t , int interpolationType )
	{
		switch( interpolationType )
		{
		case NEAREST:
		{
			t *= samples.size();
			int it1 = (int)floor(t);
			int it2 = ( it1 + 1 ) % samples.size();
			t -= it1;
			if( t<0.5 ) return samples[it1];
			else        return samples[it2];
			break;
		}
		case LINEAR:
			///////////////////////////////////////
			// Perform linear interpolation here //
			///////////////////////////////////////
			WARN_ONCE( "method undefined" );
			return samples[0];
			break;
		case CATMULL_ROM:
			////////////////////////////////////////////
			// Perform Catmull-Rom interpolation here //
			////////////////////////////////////////////
			WARN_ONCE( "method undefined" );
			return samples[0];
			break;
		case UNIFORM_CUBIC_B_SPLINE:
			///////////////////////////////////////////////////////
			// Perform uniform cubic b-spline interpolation here //
			///////////////////////////////////////////////////////
			WARN_ONCE( "method undefined" );
			return samples[0];
			break;
		default:
			ERROR_OUT( "unrecognized interpolation type" );
			return samples[0];
		}
	}
}