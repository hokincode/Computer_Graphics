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

#ifndef INTERPOLATION_INCLUDED 
#define INTERPOLATION_INCLUDED
#include <string>
#include <vector>

namespace Util
{
	/** This class represents a scheme for performing intepolation / approximation. */
	class Interpolation
	{
	public:
		/** The types of interpolation */
		enum
		{
			NEAREST ,
			LINEAR ,
			CATMULL_ROM ,
			UNIFORM_CUBIC_B_SPLINE ,
			COUNT
		};

		/** The names of the parameterizations */
		static std::string Names[];

		/** This templated static method interpolates / approximates the data in the samples vector, using the specified time parameter, t, in the range [0,1], and the prescribed interpolation scheme. */
		template< typename SampleType >
		static SampleType Sample( const std::vector< SampleType > &samples , double t , int interpolationType );
	};
}
#include "interpolation.todo.inl"

#endif // INTERPOLATION_INCLUDED
