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

#include "threads.h"


size_t ThreadPool::DefaultChunkSize = 128;
ThreadPool::ScheduleType ThreadPool::DefaultSchedule = ThreadPool::DYNAMIC;
bool ThreadPool::_Close;
#ifdef OLD_ATOMICS
unsigned int ThreadPool::_RemainingTasks;
#else // !OLD_ATOMICS
std::atomic< unsigned int > ThreadPool::_RemainingTasks;
#endif // OLD_ATOMICS
std::mutex ThreadPool::_Mutex;
std::condition_variable ThreadPool::_WaitingForWorkOrClose;
std::condition_variable ThreadPool::_DoneWithWork;
std::vector< std::thread > ThreadPool::_Threads;
std::function< void ( unsigned int ) > ThreadPool::_ThreadFunction;
ThreadPool::ParallelType ThreadPool::_ParallelType;

const std::vector< std::string >ThreadPool::ParallelNames =
{
	"none" ,
#ifdef _OPENMP
	"open mp" ,
#endif // _OPENMP
	"thread pool" ,
	"async"
};
const std::vector< std::string >ThreadPool::ScheduleNames = { "static" , "dynamic" };

void ThreadPool::Parallel_for( size_t begin , size_t end , const std::function< void ( unsigned int , size_t ) > &iterationFunction , ScheduleType schedule , size_t chunkSize )
{
	if( begin>=end ) return;
	size_t range = end - begin;
	size_t chunks = ( range + chunkSize - 1 ) / chunkSize;
	unsigned int threads = (unsigned int)NumThreads();
	std::atomic< size_t > index;
	index.store( 0 );


	if( range<chunkSize || _ParallelType==NONE || threads==1 )
	{
		for( size_t i=begin ; i<end ; i++ ) iterationFunction( 0 , i );
		return;
	}

	auto _ChunkFunction = [ &iterationFunction , begin , end , chunkSize ]( unsigned int thread , size_t chunk )
	{
		const size_t _begin = begin + chunkSize*chunk;
		const size_t _end = std::min< size_t >( end , _begin+chunkSize );
		for( size_t i=_begin ; i<_end ; i++ ) iterationFunction( thread , i );
	};
	auto _StaticThreadFunction = [ &_ChunkFunction , chunks , threads ]( unsigned int thread )
	{
		for( size_t chunk=thread ; chunk<chunks ; chunk+=threads ) _ChunkFunction( thread , chunk );
	};
	auto _DynamicThreadFunction = [ &_ChunkFunction , chunks , &index ]( unsigned int thread )
	{
		size_t chunk;
		while( ( chunk=index.fetch_add(1) )<chunks ) _ChunkFunction( thread , chunk );
	};

	if     ( schedule==STATIC  ) _ThreadFunction = _StaticThreadFunction;
	else if( schedule==DYNAMIC ) _ThreadFunction = _DynamicThreadFunction;

	if( false ){}
#ifdef _OPENMP
	else if( _ParallelType==OPEN_MP )
	{
		if( schedule==STATIC )
#pragma omp parallel for num_threads( threads ) schedule( static , 1 )
			for( int c=0 ; c<chunks ; c++ ) _ChunkFunction( omp_get_thread_num() , c );
		else if( schedule==DYNAMIC )
#pragma omp parallel for num_threads( threads ) schedule( dynamic , 1 )
			for( int c=0 ; c<chunks ; c++ ) _ChunkFunction( omp_get_thread_num() , c );
	}
#endif // _OPENMP
	else if( _ParallelType==ASYNC )
	{
		static std::vector< std::future< void > > futures;
		futures.resize( threads-1 );
		for( unsigned int t=1 ; t<threads ; t++ ) futures[t-1] = std::async( std::launch::async , _ThreadFunction , t );
		_ThreadFunction( 0 );
		for( unsigned int t=1 ; t<threads ; t++ ) futures[t-1].get();
	}
	else if( _ParallelType==THREAD_POOL )
	{
		unsigned int targetTasks = 0;
#ifdef OLD_ATOMICS
		if( !SetAtomic( &_RemainingTasks , threads-1 , targetTasks ) )
#else // !OLD_ATOMICS
		if( !_RemainingTasks.compare_exchange_weak( targetTasks , threads-1 ) )
#endif // OLD_ATOMICS
		{
			WARN( "nested for loop, reverting to serial" );
			for( size_t i=begin ; i<end ; i++ ) iterationFunction( 0 , i );
		}
		else
		{
			_WaitingForWorkOrClose.notify_all();
			{
				std::unique_lock< std::mutex > lock( _Mutex );
				_DoneWithWork.wait( lock , [&]( void ){ return _RemainingTasks==0; } );
			}
		}
	}
}

unsigned int ThreadPool::NumThreads( void ){ return (unsigned int)_Threads.size()+1; }

void ThreadPool::Init( ParallelType parallelType , unsigned int numThreads )
{
	_ParallelType = parallelType;
	if( _Threads.size() && !_Close )
	{
		_Close = true;
		_WaitingForWorkOrClose.notify_all();
		for( unsigned int t=0 ; t<_Threads.size() ; t++ ) _Threads[t].join();
	}
	_Close = true;
	numThreads--;
	_Threads.resize( numThreads );
	if( _ParallelType==THREAD_POOL )
	{
		_RemainingTasks = 0;
		_Close = false;
		for( unsigned int t=0 ; t<numThreads ; t++ ) _Threads[t] = std::thread( _ThreadInitFunction , t );
	}
}
void ThreadPool::Terminate( void )
{
	if( _Threads.size() && !_Close )
	{
		_Close = true;
		_WaitingForWorkOrClose.notify_all();
		for( unsigned int t=0 ; t<_Threads.size() ; t++ ) _Threads[t].join();
		_Threads.resize( 0 );
	}
}

void ThreadPool::_ThreadInitFunction( unsigned int thread )
{
	// Wait for the first job to come in
	std::unique_lock< std::mutex > lock( _Mutex );
	_WaitingForWorkOrClose.wait( lock );
	while( !_Close )
	{
		lock.unlock();
		// do the job
		_ThreadFunction( thread );

		// Notify and wait for the next job
		lock.lock();
		_RemainingTasks--;
		if( !_RemainingTasks ) _DoneWithWork.notify_all();
		_WaitingForWorkOrClose.wait( lock );
	}
}
