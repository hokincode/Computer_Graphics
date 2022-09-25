#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <Util/cmdLineParser.h>
#include <Util/timer.h>
#include <Ray/scene.h>
#include <Ray/box.h>
#include <Ray/cone.h>
#include <Ray/cylinder.h>
#include <Ray/sphere.h>
#include <Ray/torus.h>
#include <Ray/triangle.h>
#include <Ray/fileInstance.h>
#include <Ray/directionalLight.h>
#include <Ray/pointLight.h>
#include <Ray/spotLight.h>
#include <Ray/sphereLight.h>
#include <Util/exceptions.h>
#include <Util/threads.h>

using namespace std;
using namespace Ray;
using namespace Util;
using namespace Image;

#undef GLUT_NO_LIB_PRAGMA

CmdLineParameter< string > InputRayFile( "in" );
CmdLineParameter< string > OutputImageFile( "out" );
CmdLineParameter< int > ImageWidth( "width" , 640 );
CmdLineParameter< int > ImageHeight( "height" , 480 );
CmdLineParameter< int > RecursionLimit( "rLimit" , 5 );
CmdLineParameter< float > CutOffThreshold( "cutOff" , 0.0001f );
CmdLineParameter< int > LightSamples( "lSamples" , 100 );
CmdLineParameter< int > Parallelization( "parallel" , (int)ThreadPool::THREAD_POOL );
CmdLineReadable Progress( "progress" );


CmdLineReadable* params[] =
{
	&InputRayFile , &OutputImageFile , &ImageWidth , &ImageHeight , &RecursionLimit , &CutOffThreshold , &LightSamples , &Progress , &Parallelization ,
	NULL
};

void ShowUsage( const string &ex )
{
	cout << "Usage " << ex << ":" << endl;
	cout << "\t --" << InputRayFile.name << " <input ray File>" << endl;
	cout << "\t[--" << OutputImageFile.name << " <output image file>]" << endl;
	cout << "\t[--" << ImageWidth.name << " <image width>=" << ImageWidth.value << "]" << endl;
	cout << "\t[--" << ImageHeight.name << " <image height>=" << ImageHeight.value << "]" << endl;
	cout << "\t[--" << RecursionLimit.name << " <recursion limit>=" << RecursionLimit.value << "]" << endl;
	cout << "\t[--" << CutOffThreshold.name << " <cut-off threshold>=" << CutOffThreshold.value << "]" << endl;
	cout << "\t[--" << LightSamples.name << " <light samples>=" << LightSamples.value << "]" << endl;
	cout << "\t[--" << Parallelization.name << " <parallelization type>=" << Parallelization.value << "]" << endl;
	for( unsigned int i=0 ; i<ThreadPool::ParallelNames.size() ; i++ ) cout << "\t\t" << i << "] " << ThreadPool::ParallelNames[i] << std::endl;
	cout << "\t[--" << Progress.name << "]" << endl;
}

/** A wrapper class for size_t that prints out comma-separated numbers */
struct Size_t
{
	Size_t( size_t v=0 ) : value(v){}
	size_t value;
	size_t &operator()( void ){ return value; }
	const size_t &operator()( void ) const { return value; }
protected:
	static void _Write( std::ostream &stream , size_t top , size_t bottom )
	{
		if( !top ) stream << bottom;
		else
		{
			if( top<1000 ) stream << top;
			else _Write( stream , top/1000 , top%1000 );
			stream << ",";
			if     ( bottom<1   ) stream << "000";
			else if( bottom<10  ) stream << "00";
			else if( bottom<100 ) stream << "0";
			stream << bottom;
		}
	}
	friend std::ostream &operator << ( std::ostream &stream , const Size_t &s );
};

std::ostream &operator << ( std::ostream &stream , const Size_t &s )
{
	if( s.value<10000 ) return stream << s.value;
	Size_t::_Write( stream , s.value/1000 , s.value%1000 );
	return stream;
}

int main( int argc , char *argv[] )
{
	CmdLineParse( argc-1 , argv+1 , params );
	if( !InputRayFile.set ){ ShowUsage( argv[0] ) ; return EXIT_FAILURE; }
	ThreadPool::Init( (ThreadPool::ParallelType)Parallelization.value );

	Scene::BaseDir = GetFileDirectory( InputRayFile.value );
	Scene scene;
	try
	{
		ShapeList::ShapeFactories[ Box              ::Directive() ] = new DerivedFactory< Shape , Box >();
		ShapeList::ShapeFactories[ Cone             ::Directive() ] = new DerivedFactory< Shape , Cone >();
		ShapeList::ShapeFactories[ Cylinder         ::Directive() ] = new DerivedFactory< Shape , Cylinder >();
		ShapeList::ShapeFactories[ Sphere           ::Directive() ] = new DerivedFactory< Shape , Sphere >();
		ShapeList::ShapeFactories[ Torus            ::Directive() ] = new DerivedFactory< Shape , Torus >();
		ShapeList::ShapeFactories[ Triangle         ::Directive() ] = new DerivedFactory< Shape , Triangle >();
		ShapeList::ShapeFactories[ FileInstance     ::Directive() ] = new DerivedFactory< Shape , FileInstance >();
		ShapeList::ShapeFactories[ ShapeList        ::Directive() ] = new DerivedFactory< Shape , ShapeList >();
		ShapeList::ShapeFactories[ TriangleList     ::Directive() ] = new DerivedFactory< Shape , TriangleList >();
		ShapeList::ShapeFactories[ StaticAffineShape::Directive() ] = new DerivedFactory< Shape , StaticAffineShape >();
		ShapeList::ShapeFactories[ Union            ::Directive() ] = new DerivedFactory< Shape , Union >();
		ShapeList::ShapeFactories[ Intersection     ::Directive() ] = new DerivedFactory< Shape , Intersection >();
		ShapeList::ShapeFactories[ Difference       ::Directive() ] = new DerivedFactory< Shape , Difference >();

		GlobalSceneData::LightFactories[ DirectionalLight::Directive() ] = new DerivedFactory< Light , DirectionalLight >();
		GlobalSceneData::LightFactories[ PointLight      ::Directive() ] = new DerivedFactory< Light , PointLight >();
		GlobalSceneData::LightFactories[ SpotLight       ::Directive() ] = new DerivedFactory< Light , SpotLight >();
		GlobalSceneData::LightFactories[ SphereLight     ::Directive() ] = new DerivedFactory< Light , SphereLight >();

		ifstream istream;
		istream.open( InputRayFile.value );
		if( !istream ) THROW( "Failed to open file for reading: " , InputRayFile.value );

		Timer timer;
		istream >> scene;
		std::cout << "\tRead: " << timer.elapsed() << " seconds" << std::endl;

		timer.reset();
		RayTracingStats::Reset();
		Image32 img = scene.rayTrace( ImageWidth.value , ImageHeight.value , RecursionLimit.value , CutOffThreshold.value , LightSamples.value , Progress.set );
		std::cout << "\tRay-traced: " << timer.elapsed() << " seconds" << std::endl;
		std::cout << "\tPixels: " << Size_t( ImageWidth.value ) << " x " << Size_t( ImageHeight.value ) << std::endl;
		std::cout << "\tPrimitives: " << Size_t( scene.primitiveNum() ) << std::endl;
		std::cout << "\tRays: " << Size_t( RayTracingStats::RayNum() ) << " (" << (double)RayTracingStats::RayNum()/(ImageWidth.value*ImageHeight.value) << " rays/pixel)" << std::endl;
		std::cout << "\tPrimitive intersections: " << Size_t( RayTracingStats::RayPrimitiveIntersectionNum() ) << " (" << (double)RayTracingStats::RayPrimitiveIntersectionNum()/RayTracingStats::RayNum() << " intersections/ray)" << std::endl;
		std::cout << "\tBounding-box intersections: " << Size_t( RayTracingStats::RayBoundingBoxIntersectionNum() ) << " (" << (double)RayTracingStats::RayBoundingBoxIntersectionNum()/RayTracingStats::RayNum() << " intersections/ray)" << std::endl;
		if( RayTracingStats::ConeBoundingBoxIntersectionNum() )
			std::cout << "\tCone-bounding-box intersections: " << Size_t( RayTracingStats::ConeBoundingBoxIntersectionNum() ) << " (" << (double)RayTracingStats::ConeBoundingBoxIntersectionNum()/RayTracingStats::RayNum() << " intersections/ray)" << std::endl;
		if( OutputImageFile.set ) img.write( OutputImageFile.value );
	}
	catch( const exception &e )
	{
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}

	for( auto iter=ShapeList::ShapeFactories.begin() ; iter!=ShapeList::ShapeFactories.end() ; iter++ ) delete iter->second;
	for( auto iter=GlobalSceneData::LightFactories.begin() ; iter!=GlobalSceneData::LightFactories.end() ; iter++ ) delete iter->second;

	ThreadPool::Terminate();

	return EXIT_SUCCESS;
}