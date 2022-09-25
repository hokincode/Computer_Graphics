#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "Image/bmp.h"
#include "Image/jpeg.h"
#include "Image/image.h"
#include "Util/cmdLineParser.h"

using namespace std;
using namespace Util;
using namespace Image;

CmdLineParameter< string > Input( "in" );
CmdLineParameter< string > Output( "out" );
CmdLineParameterArray< string , 2 > Composite( "composite" );
CmdLineParameterArray< string , 3 > BeierNeelyMorph( "bnMorph" );
CmdLineParameterArray< int , 4 > Crop( "crop" );
CmdLineParameter< double > Noisify( "noisify" , 0. );
CmdLineParameter< double > Brighten( "brighten" , 1.  );
CmdLineParameter< double > Contrast( "contrast" , 1. );
CmdLineParameter< double > Saturate( "saturate" , 1. );
CmdLineParameter< double > ScaleNearest( "scaleNearest" , 1. );
CmdLineParameter< double > ScaleBilinear( "scaleBilinear" , 1. );
CmdLineParameter< double > ScaleGaussian( "scaleGaussian" , 1. );
CmdLineParameter< double > RotateNearest( "rotateNearest" , 0. );
CmdLineParameter< double > RotateBilinear( "rotateBilinear" , 0. );
CmdLineParameter< double > RotateGaussian( "rotateGaussian" , 0. );
CmdLineParameter< int > Quantize( "quantize" , 8 );
CmdLineParameter< int > RandomDither( "rDither" , 8 );
CmdLineParameter< int > OrderedDither2X2( "oDither2x2" , 8 );
CmdLineParameter< int > FloydSteinbergDither( "fsDither" , 8 );
CmdLineReadable Gray( "gray" );
CmdLineReadable Blur3X3( "blur3x3" );
CmdLineReadable Edges3X3( "edges3x3" );
CmdLineReadable Fun( "fun" );

CmdLineReadable* params[] =
{
	&Input , &Output , &Composite , &BeierNeelyMorph , &Crop , &Noisify , &Brighten , &Contrast , &Saturate ,
	&ScaleNearest , &ScaleBilinear , &ScaleGaussian , &RotateNearest , &RotateBilinear , &RotateGaussian ,
	&Quantize , &RandomDither , &OrderedDither2X2 , &FloydSteinbergDither , &Gray , &Blur3X3 , &Edges3X3 , &Fun ,
	NULL
};

void ShowUsage( const string &ex )
{
	cout << "Usage " << ex << ":" << endl;
	cout << "\t --" << Input.name    << " <input image>" << endl;
	cout << "\t[--" << Output.name   << " <output image>]" << endl;
	cout << "\t[--" << Noisify.name  << " <size of noise>=" << Noisify.value << "]" << endl;
	cout << "\t[--" << Brighten.name << " <brightening factor>=" << Brighten.value << "]" << endl;
	cout << "\t[--" << Contrast.name << " <contrast factor>=" << Contrast.value << "]" << endl;
	cout << "\t[--" << Saturate.name << " <saturation factor>=" << Saturate.value << "]" << endl;
	cout << "\t[--" << Quantize.name << " <bits per channel>=" << Quantize.value << "]" << endl;
	cout << "\t[--" << RandomDither.name << " <bits per channel with random dithering>=" << RandomDither.value << "]" << endl;
	cout << "\t[--" << OrderedDither2X2.name << " <bits per channel with ordered dithering>=" << OrderedDither2X2.value << "]" << endl;
	cout << "\t[--" << FloydSteinbergDither.name << " <bits per channel with Floyd-Steinberg dithering>=" << FloydSteinbergDither.value << "]" << endl;
	cout << "\t[--" << Composite.name << " <overlay image> <matte image>]" << endl;
	cout << "\t[--" << BeierNeelyMorph.name << " <destination image> <line segment pair list> <time step>]" << endl;
	cout << "\t[--" << Crop.name << " <x1> <y1> <x2> <y2>]" << endl;
	cout << "\t[--" << ScaleNearest.name << " <scale factor>=" << ScaleNearest.value << "]" << endl;
	cout << "\t[--" << ScaleBilinear.name << " <scale factor>=" << ScaleBilinear.value << "]" << endl;
	cout << "\t[--" << ScaleGaussian.name << " <scale factor>=" << ScaleGaussian.value << "]" << endl;
	cout << "\t[--" << RotateNearest.name << " <angle (in degrees)>=" << RotateNearest.value << "]" << endl;
	cout << "\t[--" << RotateBilinear.name << " <angle (in degrees)>=" << RotateBilinear.value << "]" << endl;
	cout << "\t[--" << RotateGaussian.name << " <angle (in degrees)>=" << RotateGaussian.value << "]" << endl;
	cout << "\t[--" << Blur3X3.name << "]" << endl;
	cout << "\t[--" << Edges3X3.name << "]" << endl;
	cout << "\t[--" << Fun.name << "]" << endl;
	cout << "\t[--" << Gray.name << "]" << endl;
}

int main( int argc , char *argv[] )
{
	CmdLineParse( argc-1 , argv+1 , params );
	if( !Input.set ) { ShowUsage( argv[0] ) ; return EXIT_FAILURE; }

	// Try to read in the input image
	Image32 image;
	image.read( Input.value );
	cout << "Input dimensions: " << image.width() << " x " << image.height() << endl;

	try
	{
		// Filter the image
		if( Noisify.set )              image = image.addRandomNoise( Noisify.value );
		if( Brighten.set )             image = image.brighten( Brighten.value );
		if( Gray.set )                 image = image.luminance();
		if( Contrast.set )             image = image.contrast( Contrast.value );
		if( Saturate.set )             image = image.saturate( Saturate.value );
		if( Quantize.set )             image = image.quantize( Quantize.value );
		if( RandomDither.set )         image = image.randomDither( RandomDither.value );
		if( OrderedDither2X2.set )     image = image.orderedDither2X2( OrderedDither2X2.value );
		if( FloydSteinbergDither.set ) image = image.floydSteinbergDither( FloydSteinbergDither.value );

		if( Composite.set )
		{
			Image32 overlay , matte;
			// Read in the target image
			overlay.read( Composite.values[0] );
			// Read in the matte image
			matte.read( Composite.values[1] );
			// Set the alpha value of the overlay image using the values of the matte image
			overlay.setAlpha( matte );
			// Perform the compositing
			image = image.composite( overlay );
		}
		if( Blur3X3.set )  image = image.blur3X3();
		if( Edges3X3.set ) image = image.edgeDetect3X3();
		if( ScaleNearest.set )  image = image.scaleNearest ( ScaleNearest.value  );
		if( ScaleBilinear.set ) image = image.scaleBilinear( ScaleBilinear.value );
		if( ScaleGaussian.set ) image = image.scaleGaussian( ScaleGaussian.value );
		if( RotateNearest.set )  image = image.rotateNearest ( RotateNearest.value  );
		if( RotateBilinear.set ) image = image.rotateBilinear( RotateBilinear.value );
		if( RotateGaussian.set ) image = image.rotateGaussian( RotateGaussian.value );

		if( Fun.set ) image = image.funFilter();
		if( Crop.set ) image = image.crop( Crop.values[0] , Crop.values[1] , Crop.values[2] , Crop.values[3] );

		if( BeierNeelyMorph.set )
		{
			double timeStep = atof( BeierNeelyMorph.values[2].c_str() );
			Image32 dest;
			OrientedLineSegmentPairs olsp;

			// Read the destination image
			dest.read( BeierNeelyMorph.values[0] );
			// Read in the list of corresponding line segments
			ifstream istream;
			istream.open( BeierNeelyMorph.values[1] );
			if( !istream ) THROW( "Failed to open file for reading: " , BeierNeelyMorph.values[1] );
			try{ istream >> olsp; }
			catch( Util::Exception e ){ THROW( "failed to read OrientedLineSegmentPairs: " , BeierNeelyMorph.values[1] , " -- " , e.what() ); }
			image = Image32::BeierNeelyMorph( image , dest , olsp , timeStep );
		}

		cout << "Output dimensions: " << image.width() << " x " << image.height() << endl;

		// Try to write out the output image
		if( Output.set ) image.write( Output.value );
	}
	catch( const exception& e )
	{
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	};
	return EXIT_SUCCESS;
}
