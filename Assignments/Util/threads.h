/*
Copyright (c) 2017, Michael Kazhdan
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

#ifndef THREADS_INCLUDED
#define THREADS_INCLUDED

#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <chrono>
#include <future>
#ifdef _OPENMP
#include <omp.h>
#endif // _OPENMP
#include "exceptions.h"

#define OLD_ATOMICS


#ifdef OLD_ATOMICS
#if defined( _WIN32 ) || defined( _WIN64 )
#include <windows.h>
#endif // _WIN32 || _WIN64
template< typename Value >
bool SetAtomic32( volatile Value *value , Value newValue , Value oldValue )
{
#if defined( _WIN32 ) || defined( _WIN64 )
	long &_oldValue = *(long *)&oldValue;
	long &_newValue = *(long *)&newValue;
	return InterlockedCompareExchange( (long*)value , _newValue , _oldValue )==_oldValue;
#else // !_WIN32 && !_WIN64
	uint32_t &_oldValue = *(uint32_t *)&oldValue;
	uint32_t &_newValue = *(uint32_t *)&newValue;
	//	return __sync_bool_compare_and_swap( (uint32_t *)value , _oldValue , _newValue );
	return __atomic_compare_exchange_n( (uint32_t *)value , (uint32_t *)&oldValue , _newValue , false , __ATOMIC_SEQ_CST , __ATOMIC_SEQ_CST );
#endif // _WIN32 || _WIN64
}
template< typename Value >
bool SetAtomic64( volatile Value *value , Value newValue , Value oldValue )
{
#if defined( _WIN32 ) || defined( _WIN64 )
	__int64 &_oldValue = *(__int64 *)&oldValue;
	__int64 &_newValue = *(__int64 *)&newValue;
	return InterlockedCompareExchange64( (__int64*)value , _newValue , _oldValue )==_oldValue;
#else // !_WIN32 && !_WIN64
	uint64_t &_oldValue = *(uint64_t *)&oldValue;
	uint64_t &_newValue = *(uint64_t *)&newValue;
	//	return __sync_bool_compare_and_swap ( (uint64_t *)&value , _oldValue , _newValue );
	return __atomic_compare_exchange_n( (uint64_t *)value , (uint64_t *)&oldValue , _newValue , false , __ATOMIC_SEQ_CST , __ATOMIC_SEQ_CST );
#endif // _WIN32 || _WIN64
}

template< typename Value >
bool SetAtomic( volatile Value *value , Value newValue , Value oldValue )
{
	switch( sizeof(Value) )
	{
	case 4: return SetAtomic32( value , newValue , oldValue );
	case 8: return SetAtomic64( value , newValue , oldValue );
	default:
		WARN_ONCE( "should not use this function: " , sizeof(Value) );
		static std::mutex setAtomicMutex;
		std::lock_guard< std::mutex > lock( setAtomicMutex );
		if( *value==oldValue ){ *value = newValue ; return true; }
		else return false;
	}
}
#endif // OLD_ATOMICS
struct ThreadPool
{
	enum ParallelType
	{
		NONE ,
#ifdef _OPENMP
		OPEN_MP ,
#endif // _OPENMP
		THREAD_POOL ,
		ASYNC
	};
	static const std::vector< std::string > ParallelNames;

	enum ScheduleType
	{
		STATIC ,
		DYNAMIC
	};
	static const std::vector< std::string > ScheduleNames;

	static size_t DefaultChunkSize;
	static ScheduleType DefaultSchedule;

	template< typename ... Functions >
	static void ParallelSections( const Functions & ... functions )
	{
		std::vector< std::future< void > > futures( sizeof...(Functions) );
		_ParallelSections( &futures[0] , functions ... );
		for( size_t t=0 ; t<futures.size() ; t++ ) futures[t].get();
	}

	static void Parallel_for( size_t begin , size_t end , const std::function< void ( unsigned int , size_t ) > &iterationFunction , ScheduleType schedule=DefaultSchedule , size_t chunkSize=DefaultChunkSize );

	static unsigned int NumThreads( void );

	static void Init( ParallelType parallelType , unsigned int numThreads=std::thread::hardware_concurrency() );

	static void Terminate( void );

private:
	ThreadPool( const ThreadPool & ){}
	ThreadPool &operator = ( const ThreadPool & ){ return *this; }

	template< typename Function >
	static void _ParallelSections( std::future< void > *futures , const Function &function ){ *futures = std::async( std::launch::async , function ); }

	template< typename Function , typename ... Functions >
	static void _ParallelSections( std::future< void > *futures , const Function &function , const Functions& ... functions )
	{
		*futures = std::async( std::launch::async , function );
		_ParallelSections( futures+1 , functions ... );
	}
	static inline void _ThreadInitFunction( unsigned int thread );

	static bool _Close;
#ifdef OLD_ATOMICS
	static unsigned int _RemainingTasks;
#else // !OLD_ATOMICS
	static std::atomic< unsigned int > _RemainingTasks;
#endif // OLD_ATOMICS
	static std::mutex _Mutex;
	static std::condition_variable _WaitingForWorkOrClose , _DoneWithWork;
	static std::vector< std::thread > _Threads;
	static std::function< void ( unsigned int ) > _ThreadFunction;
	static ParallelType _ParallelType;
};


#endif // THREADS_INCLUDED
