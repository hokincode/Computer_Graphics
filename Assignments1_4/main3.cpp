#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <Util/cmdLineParser.h>
#include <Ray/window.h>
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

using namespace std;
using namespace Ray;
using namespace Util;

CmdLineParameter< string > InputRayFile( "in" );
CmdLineParameter< int > WindowWidth( "width" , 640 );
CmdLineParameter< int > WindowHeight( "height" , 480 );
CmdLineParameter< int > Complexity( "cplx" , 10 );

CmdLineReadable* params[] =
{
	&InputRayFile , &WindowWidth , &WindowHeight , &Complexity ,
	NULL
};

void ShowUsage( const string &ex )
{
	cout << "Usage " << ex << ":" << endl;
	cout << "\t --" << InputRayFile.name << " <input ray File>" << endl;
	cout << "\t[--" << WindowWidth.name << " <window width>=" << WindowWidth.value << "]" << endl;
	cout << "\t[--" << WindowHeight.name << " <window height>=" << WindowHeight.value << "]" << endl;
	cout << "\t[--" << Complexity.name << " <tessellation complexity>=" << Complexity.value << "]" << endl;
}

int main( int argc , char *argv[] )
{
	CmdLineParse( argc-1 , argv+1 , params );
	if( !InputRayFile.set ){ ShowUsage( argv[0] ) ; return EXIT_FAILURE; }

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

		GlobalSceneData::LightFactories[ DirectionalLight::Directive() ] = new DerivedFactory< Light , DirectionalLight >();
		GlobalSceneData::LightFactories[ PointLight      ::Directive() ] = new DerivedFactory< Light , PointLight >();
		GlobalSceneData::LightFactories[ SpotLight       ::Directive() ] = new DerivedFactory< Light , SpotLight >();
		GlobalSceneData::LightFactories[ SphereLight     ::Directive() ] = new DerivedFactory< Light , SphereLight >();

		Scene::BaseDir = GetFileDirectory( InputRayFile.value );
		Scene scene;
		ifstream istream;
		istream.open( InputRayFile.value );
		if( !istream ) THROW( "Failed to open file for reading: " , InputRayFile.value );
		istream >> scene;
		Shape::OpenGLTessellationComplexity = Complexity.value;
		Window::View( scene , WindowWidth.value , WindowHeight.value );
	}
	catch( const exception& e )
	{
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	};

	for( auto iter=ShapeList::ShapeFactories.begin() ; iter!=ShapeList::ShapeFactories.end() ; iter++ ) delete iter->second;
	for( auto iter=GlobalSceneData::LightFactories.begin() ; iter!=GlobalSceneData::LightFactories.end() ; iter++ ) delete iter->second;

	return EXIT_SUCCESS;
}