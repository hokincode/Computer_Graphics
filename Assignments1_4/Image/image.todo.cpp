#include <algorithm>
#include "image.h"
#include <stdlib.h>
#include <math.h>
#include <Util/exceptions.h>

using namespace Util;
using namespace Image;

/////////////
// Image32 //
/////////////
Image32 Image32::addRandomNoise( double noise ) const
{
	Image32 img(*this);
	
	for (iterator iter = img.begin(); iter != img.end(); ++iter) {
		double random_green = (((double)rand())/((double)RAND_MAX)*2)-1;
		double random_red = (((double)rand())/((double)RAND_MAX)*2)-1;
		double random_blue = (((double)rand())/((double)RAND_MAX)*2)-1;
		int temporary_red = (*iter).r + (*iter).r * random_red * noise;
		int temporary_green = (*iter).g + (*iter).g * random_green * noise;
		int temporary_blue = (*iter).b + (*iter).b * random_blue * noise;
		(*iter).r = (unsigned char) std::min(std::max(temporary_red, 0), 255);
		(*iter).g = (unsigned char) std::min(std::max(temporary_green, 0), 255);
		(*iter).b = (unsigned char) std::min(std::max(temporary_blue, 0), 255);
	}
	///////////////////////////
	// Add random noise here //
	///////////////////////////
	return img;
}

Image32 Image32::brighten( double brightness ) const
{
	/////////////////////////
	// Do brightening here //
	/////////////////////////
	Image32 img(*this);

	for (iterator iter = img.begin(); iter != img.end(); ++iter) {
		int temporary_red = (*iter).r  * brightness;
		int temporary_green = (*iter).g  * brightness;
		int temporary_blue = (*iter).b  * brightness;
		(*iter).r = (unsigned char) std::min(std::max(temporary_red, 0), 255);
		(*iter).g = (unsigned char) std::min(std::max(temporary_green, 0), 255);
		(*iter).b = (unsigned char) std::min(std::max(temporary_blue, 0), 255);
	}

	return img;
}

Image32 Image32::luminance( void ) const
{
	Image32 img(*this);

	for (iterator iter = img.begin(); iter != img.end(); ++iter) {
		int luminance_level = 0.30 * (*iter).r + 0.59 * (*iter).g + 0.11 * (*iter).b;
		(*iter).r = (unsigned char) std::min(std::max(luminance_level, 0), 255);
		(*iter).g = (unsigned char) std::min(std::max(luminance_level, 0), 255);
		(*iter).b = (unsigned char) std::min(std::max(luminance_level, 0), 255);
	}

	return img;
}

Image32 Image32::contrast( double contrast ) const
{
	Image32 img(*this);
	int luminance_summation = 0;

	for (iterator iter = img.begin(); iter != img.end(); ++iter) {
		int luminance_level = 0.30 * (*iter).r + 0.59 * (*iter).g + 0.11 * (*iter).b;
		luminance_summation = luminance_summation + luminance_level;
	}

	int luminance_average = luminance_summation / (img._height * img._width);

	for (iterator iter = img.begin(); iter != img.end(); ++iter) {
		int temporary_red = ((*iter).r - luminance_average) * contrast + luminance_average;
		int temporary_green = ((*iter).g - luminance_average) * contrast + luminance_average;
		int temporary_blue = ((*iter).b - luminance_average) * contrast + luminance_average;
		(*iter).r = (unsigned char) std::min(std::max(temporary_red, 0), 255);
		(*iter).g = (unsigned char) std::min(std::max(temporary_green, 0), 255);
		(*iter).b = (unsigned char) std::min(std::max(temporary_blue, 0), 255);
	}

	return img;
}

Image32 Image32::saturate( double saturation ) const
{
	Image32 img(*this);

	for (iterator iter = img.begin(); iter != img.end(); ++iter) {
		int luminance_level = 0.30 * (*iter).r + 0.59 * (*iter).g + 0.11 * (*iter).b;
		int temporary_red = ((*iter).r - luminance_level) * saturation + luminance_level;
		int temporary_green = ((*iter).g - luminance_level) * saturation + luminance_level;
		int temporary_blue = ((*iter).b - luminance_level) * saturation + luminance_level;
		(*iter).r = (unsigned char) std::min(std::max(temporary_red, 0), 255);
		(*iter).g = (unsigned char) std::min(std::max(temporary_green, 0), 255);
		(*iter).b = (unsigned char) std::min(std::max(temporary_blue, 0), 255);
	}

	return img;
}

Image32 Image32::quantize( int bits ) const
{
	//////////////////////////
	// Do quantization here //
	//////////////////////////
	Image32 img(*this);

	for (iterator iter = img.begin(); iter != img.end(); ++iter) {
		double temporary_red_intensity =  (*iter).r/255.0;
		double temporary_green_intensity = (*iter).g/255.0;
		double temporary_blue_intensity = (*iter).b/255.0;
		double power = pow(2,bits);
		int temporary_bits_red = floor(temporary_red_intensity * power);
		int temporary_bits_green = floor(temporary_green_intensity * power);
		int temporary_bits_blue = floor(temporary_blue_intensity * power);
		int temporary_red = temporary_bits_red / (power-1) * 255;
		int temporary_green = temporary_bits_green / (power-1) * 255;
		int temporary_blue = temporary_bits_blue / (power-1) * 255;
		(*iter).r = (unsigned char) std::min(std::max(temporary_red, 0), 255);
		(*iter).g = (unsigned char) std::min(std::max(temporary_green, 0), 255);
		(*iter).b = (unsigned char) std::min(std::max(temporary_blue, 0), 255);
	}

	return img;
}

Image32 Image32::randomDither( int bits ) const
{
	//////////////////////////////
	// Do random dithering here //
	//////////////////////////////
	Image32 img(*this);
	if (bits >= 8) return img;
	img = img.addRandomNoise(1.0/(pow(2,bits)));
	img = img.quantize(bits);	
	return img;
}

Image32 Image32::orderedDither2X2( int bits ) const
{
	///////////////////////////////
	// Do ordered dithering here //
	///////////////////////////////
	Image32 img(*this);
	int power = pow(2,bits);
	double level = power - 1;
	int off = 255/(power - 1);
	int matrix[2][2] = {{1,3},{4,2}};
	double normality = 5;
	for (int h = 0; h < img._height; h++) {
		for (int w = 0; w <img._width; w++) {
			double red = img(h,w).r/255.0*level;
			double green = img(h,w).g/255.0*level;
			double blue = img(h,w).b/255.0*level;
			if (red - floor(red) > matrix[h%2][w%2]/normality) {
				img(h,w).r = ceil(red);
			} else {
				img(h,w).r = floor(red);
			}
			if (green - floor(green) > matrix[h%2][w%2]/normality) {
				img(h,w).g = ceil(green);
			} else {
				img(h,w).g = floor(green);
			}
			if (blue - floor(blue) > matrix[h%2][w%2]/normality) {
				img(h,w).b = ceil(blue);
			} else {
				img(h,w).b = floor(blue);
			}
			img(h,w).r *= off;
			img(h,w).b *= off;
			img(h,w).g *= off;
		}
	}
	return img;
}

Image32 Image32::floydSteinbergDither( int bits ) const
{
	///////////////////////////////////////
	// Do Floyd-Steinberg dithering here //
	///////////////////////////////////////
	WARN( "method undefined" );
	return Image32();
}

Image32 Image32::blur3X3( void ) const
{
	//////////////////////
	// Do blurring here //
	//////////////////////
	WARN( "method undefined" );
	return Image32();
}

Image32 Image32::edgeDetect3X3( void ) const
{
	////////////////////////////
	// Do edge detection here //
	////////////////////////////
	WARN( "method undefined" );
	return Image32();
}

Image32 Image32::scaleNearest( double scaleFactor ) const
{
	/////////////////////////////////////////////////
	// Do scaling with nearest-point sampling here //
	/////////////////////////////////////////////////
	WARN( "method undefined" );
	return Image32();
}

Image32 Image32::scaleBilinear( double scaleFactor ) const
{
	////////////////////////////////////////////
	// Do scaling with bilinear sampling here //
	////////////////////////////////////////////
	WARN( "method undefined" );
	return Image32();
}

Image32 Image32::scaleGaussian( double scaleFactor ) const
{
	////////////////////////////////////////////
	// Do scaling with Gaussian sampling here //
	////////////////////////////////////////////
	WARN( "method undefined" );
	return Image32();
}

Image32 Image32::rotateNearest( double angle ) const
{
	//////////////////////////////////////////////////
	// Do rotation with nearest-point sampling here //
	//////////////////////////////////////////////////
	WARN( "method undefined" );
	return Image32();
}

Image32 Image32::rotateBilinear( double angle ) const
{
	/////////////////////////////////////////////
	// Do rotation with bilinear sampling here //
	/////////////////////////////////////////////
	WARN( "method undefined" );
	return Image32();
}

Image32 Image32::rotateGaussian( double angle ) const
{
	/////////////////////////////////////////////
	// Do rotation with Gaussian sampling here //
	/////////////////////////////////////////////
	WARN( "method undefined" );
	return Image32();
}

void Image32::setAlpha( const Image32& matte )
{
	///////////////////////////
	// Set alpha values here //
	///////////////////////////
	WARN( "method undefined" );
}

Image32 Image32::composite( const Image32& overlay ) const
{
	/////////////////////////
	// Do compositing here //
	/////////////////////////
	WARN( "method undefined" );
	return Image32();
}

Image32 Image32::CrossDissolve( const Image32& source , const Image32& destination , double blendWeight )
{
	////////////////////////////
	// Do cross-dissolve here //
	////////////////////////////
	WARN( "method undefined" );
	return Image32();
}

Image32 Image32::warp( const OrientedLineSegmentPairs& olsp ) const
{
	/////////////////////
	// Do warping here //
	/////////////////////
	WARN( "method undefined" );
	return Image32();
}

Image32 Image32::funFilter( void ) const
{
	////////////////////////////
	// Do the fun-filter here //
	////////////////////////////
	WARN( "method undefined" );
	return Image32();
}

Image32 Image32::crop( int x1 , int y1 , int x2 , int y2 ) const
{
	//////////////////////
	// Do cropping here //
	//////////////////////
	WARN( "method undefined" );
	return Image32();
}

Pixel32 Image32::nearestSample( Point2D p ) const
{
	//////////////////////////////
	// Do nearest sampling here //
	//////////////////////////////
	WARN( "method undefined" );
	return Pixel32();
}

Pixel32 Image32::bilinearSample( Point2D p ) const
{
	///////////////////////////////
	// Do bilinear sampling here //
	///////////////////////////////
	WARN( "method undefined" );
	return Pixel32();
}

Pixel32 Image32::gaussianSample( Point2D p , double variance , double radius ) const
{
	///////////////////////////////
	// Do Gaussian sampling here //
	///////////////////////////////
	WARN( "method undefined" );
	return Pixel32();
}
