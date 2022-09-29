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
	Image32 img(*this);
	Image32 dest;
	dest.setSize(_width,_height);
	double alpha = 7.0 / 16.0;
	double beta = 3.0 / 16.0;
	double gamma = 5.0 / 16.0;
	double delta = 1.0 / 16.0;
	for (int h = 0; h < img._height; h++) {
		for (int w = 0; w <img._width; w++) {
			double temporary_red_intensity =  img(w,h).r/255.0;
			double temporary_green_intensity = img(w,h).g/255.0;
			double temporary_blue_intensity = img(w,h).b/255.0;
			double power = pow(2,bits);
			int temporary_bits_red = floor(temporary_red_intensity * power);
			int temporary_bits_green = floor(temporary_green_intensity * power);
			int temporary_bits_blue = floor(temporary_blue_intensity * power);
			int temporary_red = temporary_bits_red / (power-1) * 255;
			int temporary_green = temporary_bits_green / (power-1) * 255;
			int temporary_blue = temporary_bits_blue / (power-1) * 255;
			unsigned char r = std::min(std::max(temporary_red, 0), 255);
			unsigned char g = std::min(std::max(temporary_green, 0), 255);
			unsigned char b = std::min(std::max(temporary_blue, 0), 255);
			double error_red = img(w,h).r - r;
			double error_blue = img(w,h).b - b;
			double error_green = img(w,h).g - g;
			dest(w,h).r = r;
			dest(w,h).g = g;
			dest(w,h).b = b;
			int temp = 0;

			int decision = 0;

			if (h + 1 == img._height && w + 1 != img._width && w != 0) {
				decision = 1;
			} else if (w + 1 == img._width && h + 1 != img._height) {
				decision = 2;
			} else if (w == 0 && h + 1 != img._height) {
				decision = 3;
			} else if (w >= 1 && w < img._width - 1 && h < img._height - 1) {
				decision = 4;
			}

			switch (decision) {
				case 1: 
					temp = (int) img(w+1,h).r + alpha * error_red;
 					img(w+1,h).r = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w+1,h).g + alpha * error_green;
 					img(w+1,h).g = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w+1,h).b + alpha * error_blue;
 					img(w+1,h).b = (unsigned char) std::min(std::max(temp, 0), 255);
					break;

				case 2:
					temp = (int) img(w-1,h+1).r + beta * error_red;
					img(w-1,h+1).r = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w,h+1).r + gamma * error_red;
					img(w,h+1).r = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w-1,h+1).g + beta * error_green;
					img(w-1,h+1).g = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w,h+1).g + gamma * error_green;
					img(w,h+1).g = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w-1,h+1).b + beta * error_blue;
					img(w-1,h+1).b = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w,h+1).b + gamma * error_blue;
					img(w,h+1).b = (unsigned char) std::min(std::max(temp, 0), 255);
					break;

				case 3:
					temp = (int) img(w+1,h).r + alpha * error_red;
 					img(w+1,h).r = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w,h+1).r + gamma * error_red;
					img(w,h+1).r = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w+1,h+1).r + delta * error_red;
					img(w+1,h+1).r = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w+1,h).g + alpha * error_green;
 					img(w+1,h).g = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w,h+1).g + gamma * error_green;
					img(w,h+1).g = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w+1,h+1).g + delta * error_green;
					img(w+1,h+1).g = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w+1,h).b + alpha * error_blue;
 					img(w+1,h).b = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w,h+1).b + gamma * error_blue;
					img(w,h+1).b = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w+1,h+1).b + delta * error_blue;
					img(w+1,h+1).b = (unsigned char) std::min(std::max(temp, 0), 255);
					break;
				
				case 4:
					temp = (int) img(w+1,h).r + alpha * error_red;
 					img(w+1,h).r = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w-1,h+1).r + beta * error_red;
					img(w-1,h+1).r = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w,h+1).r + gamma * error_red;
					img(w,h+1).r = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w+1,h+1).r + delta * error_red;
					img(w+1,h+1).r = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w+1,h).g + alpha * error_green;
 					img(w+1,h).g = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w-1,h+1).g + beta * error_green;
					img(w-1,h+1).g = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w,h+1).g + gamma * error_green;
					img(w,h+1).g = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w+1,h+1).g + delta * error_green;
					img(w+1,h+1).g = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w+1,h).b + alpha * error_blue;
 					img(w+1,h).b = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w-1,h+1).b + beta * error_blue;
					img(w-1,h+1).b = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w,h+1).b + gamma * error_blue;
					img(w,h+1).b = (unsigned char) std::min(std::max(temp, 0), 255);

					temp = (int) img(w+1,h+1).b + delta * error_blue;
					img(w+1,h+1).b = (unsigned char) std::min(std::max(temp, 0), 255);
					break;
			}
		}
	}
	return dest;
}

Image32 Image32::blur3X3( void ) const
{
	Image32 img(*this);
	Image32 dest;
	dest.setSize(img._width,img._height);
	std::vector<std::vector<double>> mask = {{1.0/16.0, 2.0/16.0, 1.0/16.0},{2.0/16.0, 4.0/16.0, 2.0/16.0},{1.0/16.0, 2.0/16.0, 1.0/16.0}};
	for (int h = 1; h < img._height - 1; h++) {
		for (int w = 1; w <img._width - 1; w++) {
			double r = (
				img(w-1,h-1).r * mask[0][0] + img(w,h-1).r * mask[0][1] + img(w+1,h-1).r * mask[0][2] +
				img(w-1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w+1,h).r * mask[1][2] +
				img(w-1,h+1).r * mask[2][0] + img(w,h+1).r * mask[2][1] + img(w+1,h+1).r * mask[2][2]
			);

			double b = (
				img(w-1,h-1).b * mask[0][0] + img(w,h-1).b * mask[0][1] + img(w+1,h-1).b * mask[0][2] +
				img(w-1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w+1,h).b * mask[1][2] +
				img(w-1,h+1).b * mask[2][0] + img(w,h+1).b * mask[2][1] + img(w+1,h+1).b * mask[2][2]
			);
			double g = (
				img(w-1,h-1).g * mask[0][0] + img(w,h-1).g * mask[0][1] + img(w+1,h-1).g * mask[0][2] +
				img(w-1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w+1,h).g * mask[1][2] +
				img(w-1,h+1).g * mask[2][0] + img(w,h+1).g * mask[2][1] + img(w+1,h+1).g * mask[2][2]
			);
		dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
		dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
		dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);
		}
	}

	for (int w = 1; w <img._width - 1; w++){
		int h = 0;
		double r = (
			img(w-1,h+1).r * mask[0][0] + img(w,h+1).r * mask[0][1] + img(w+1,h+1).r * mask[0][2] +
			img(w-1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w+1,h).r * mask[1][2] +
			img(w-1,h+1).r * mask[2][0] + img(w,h+1).r * mask[2][1] + img(w+1,h+1).r * mask[2][2]
		);
		double b = (
			img(w-1,h+1).b * mask[0][0] + img(w,h+1).b * mask[0][1] + img(w+1,h+1).b * mask[0][2] +
			img(w-1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w+1,h).b * mask[1][2] +
			img(w-1,h+1).b * mask[2][0] + img(w,h+1).b * mask[2][1] + img(w+1,h+1).b * mask[2][2]
		);
		double g = (
			img(w-1,h+1).g * mask[0][0] + img(w,h+1).g * mask[0][1] + img(w+1,h+1).g * mask[0][2] +
			img(w-1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w+1,h).g * mask[1][2] +
			img(w-1,h+1).g * mask[2][0] + img(w,h+1).g * mask[2][1] + img(w+1,h+1).g * mask[2][2]
		);
		dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
		dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
		dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);
	}

	for (int w = 1; w <img._width - 1; w++){
		int h = img._height - 1;
		double r = (
			img(w-1,h-1).r * mask[0][0] + img(w,h-1).r * mask[0][1] + img(w+1,h-1).r * mask[0][2] +
			img(w-1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w+1,h).r * mask[1][2] +
			img(w-1,h-1).r * mask[2][0] + img(w,h-1).r * mask[2][1] + img(w+1,h-1).r * mask[2][2]
		);
		double b = (
			img(w-1,h-1).b * mask[0][0] + img(w,h-1).b * mask[0][1] + img(w+1,h-1).b * mask[0][2] +
			img(w-1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w+1,h).b * mask[1][2] +
			img(w-1,h-1).b * mask[2][0] + img(w,h-1).b * mask[2][1] + img(w+1,h-1).b * mask[2][2]
		);
		double g = (
			img(w-1,h-1).g * mask[0][0] + img(w,h-1).g * mask[0][1] + img(w+1,h-1).g * mask[0][2] +
			img(w-1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w+1,h).g * mask[1][2] +
			img(w-1,h-1).g * mask[2][0] + img(w,h-1).g * mask[2][1] + img(w+1,h-1).g * mask[2][2]
		);
		dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
		dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
		dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);
	}

	for (int h = 1; h <img._height - 1; h++) {
		int w = img._width - 1;
		double r = (
			img(w-1,h-1).r * mask[0][0] + img(w,h-1).r * mask[0][1] + img(w-1,h-1).r * mask[0][2] +
			img(w-1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w-1,h).r * mask[1][2] +
			img(w-1,h+1).r * mask[2][0] + img(w,h+1).r * mask[2][1] + img(w-1,h+1).r * mask[2][2]
		);
		double b = (
			img(w-1,h-1).b * mask[0][0] + img(w,h-1).b * mask[0][1] + img(w-1,h-1).b * mask[0][2] +
			img(w-1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w-1,h).b * mask[1][2] +
			img(w-1,h+1).b * mask[2][0] + img(w,h+1).b * mask[2][1] + img(w-1,h+1).b * mask[2][2]
		);
		double g = (
			img(w-1,h-1).g * mask[0][0] + img(w,h-1).g * mask[0][1] + img(w-1,h-1).g * mask[0][2] +
			img(w-1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w-1,h).g * mask[1][2] +
			img(w-1,h+1).g * mask[2][0] + img(w,h+1).g * mask[2][1] + img(w-1,h+1).g * mask[2][2]
		);
		dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
		dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
		dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);
	}

	for (int h = 1; h <img._height - 1; h++) {
		int w = 0;
		double r = (
			img(w+1,h-1).r * mask[0][0] + img(w,h-1).r * mask[0][1] + img(w+1,h-1).r * mask[0][2] +
			img(w+1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w+1,h).r * mask[1][2] +
			img(w+1,h+1).r * mask[2][0] + img(w,h+1).r * mask[2][1] + img(w+1,h+1).r * mask[2][2]
		);
		double b = (
			img(w+1,h-1).b * mask[0][0] + img(w,h-1).b * mask[0][1] + img(w+1,h-1).b * mask[0][2] +
			img(w+1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w+1,h).b * mask[1][2] +
			img(w+1,h+1).b * mask[2][0] + img(w,h+1).b * mask[2][1] + img(w+1,h+1).b * mask[2][2]
		);
		double g = (
			img(w+1,h-1).g * mask[0][0] + img(w,h-1).g * mask[0][1] + img(w+1,h-1).g * mask[0][2] +
			img(w+1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w+1,h).g * mask[1][2] +
			img(w+1,h+1).g * mask[2][0] + img(w,h+1).g * mask[2][1] + img(w+1,h+1).g * mask[2][2]
		);
		dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
		dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
		dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);
	}

	int w = 0;
	int h = 0;
	double r = (
		img(w+1,h+1).r * mask[0][0] + img(w,h+1).r * mask[0][1] + img(w+1,h+1).r * mask[0][2] +
		img(w+1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w+1,h).r * mask[1][2] +
		img(w+1,h+1).r * mask[2][0] + img(w,h+1).r * mask[2][1] + img(w+1,h+1).r * mask[2][2]
	);
	double b = (
		img(w+1,h+1).b * mask[0][0] + img(w,h+1).b * mask[0][1] + img(w+1,h+1).b * mask[0][2] +
		img(w+1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w+1,h).b * mask[1][2] +
		img(w+1,h+1).b * mask[2][0] + img(w,h+1).b * mask[2][1] + img(w+1,h+1).b * mask[2][2]
	);
	double g = (
		img(w+1,h+1).g * mask[0][0] + img(w,h+1).g * mask[0][1] + img(w+1,h+1).g * mask[0][2] +
		img(w+1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w+1,h).g * mask[1][2] +
		img(w+1,h+1).g * mask[2][0] + img(w,h+1).g * mask[2][1] + img(w+1,h+1).g * mask[2][2]
	);
	dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
	dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
	dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);

	w = 0;
	h = img._height - 1;
	r = (
		img(w+1,h-1).r * mask[0][0] + img(w,h-1).r * mask[0][1] + img(w+1,h-1).r * mask[0][2] +
		img(w+1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w+1,h).r * mask[1][2] +
		img(w+1,h-1).r * mask[2][0] + img(w,h-1).r * mask[2][1] + img(w+1,h-1).r * mask[2][2]
	);
	b = (
		img(w+1,h-1).b * mask[0][0] + img(w,h-1).b * mask[0][1] + img(w+1,h-1).b * mask[0][2] +
		img(w+1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w+1,h).b * mask[1][2] +
		img(w+1,h-1).b * mask[2][0] + img(w,h-1).b * mask[2][1] + img(w+1,h-1).b * mask[2][2]
	);
	g = (
		img(w+1,h-1).g * mask[0][0] + img(w,h-1).g * mask[0][1] + img(w+1,h-1).g * mask[0][2] +
		img(w+1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w+1,h).g * mask[1][2] +
		img(w+1,h-1).g * mask[2][0] + img(w,h-1).g * mask[2][1] + img(w+1,h-1).g * mask[2][2]
	);
	dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
	dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
	dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);

	w = img._width - 1;
	h = 0;
	r = (
		img(w-1,h+1).r * mask[0][0] + img(w,h+1).r * mask[0][1] + img(w-1,h+1).r * mask[0][2] +
		img(w-1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w-1,h).r * mask[1][2] +
		img(w-1,h+1).r * mask[2][0] + img(w,h+1).r * mask[2][1] + img(w-1,h+1).r * mask[2][2]
	);
	b = (
		img(w-1,h+1).b * mask[0][0] + img(w,h+1).b * mask[0][1] + img(w-1,h+1).b * mask[0][2] +
		img(w-1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w-1,h).b * mask[1][2] +
		img(w-1,h+1).b * mask[2][0] + img(w,h+1).b * mask[2][1] + img(w-1,h+1).b * mask[2][2]
	);
	g = (
		img(w-1,h+1).g * mask[0][0] + img(w,h+1).g * mask[0][1] + img(w-1,h+1).g * mask[0][2] +
		img(w-1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w-1,h).g * mask[1][2] +
		img(w-1,h+1).g * mask[2][0] + img(w,h+1).g * mask[2][1] + img(w-1,h+1).g * mask[2][2]
	);
	dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
	dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
	dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);

	w = img._width - 1;
	h = img._height - 1;
	r = (
		img(w-1,h-1).r * mask[0][0] + img(w,h-1).r * mask[0][1] + img(w-1,h-1).r * mask[0][2] +
		img(w-1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w-1,h).r * mask[1][2] +
		img(w-1,h-1).r * mask[2][0] + img(w,h-1).r * mask[2][1] + img(w-1,h-1).r * mask[2][2]
	);
	b = (
		img(w-1,h-1).b * mask[0][0] + img(w,h-1).b * mask[0][1] + img(w-1,h-1).b * mask[0][2] +
		img(w-1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w-1,h).b * mask[1][2] +
		img(w-1,h-1).b * mask[2][0] + img(w,h-1).b * mask[2][1] + img(w-1,h-1).b * mask[2][2]
	);
	g = (
		img(w-1,h-1).g * mask[0][0] + img(w,h-1).g * mask[0][1] + img(w-1,h-1).g * mask[0][2] +
		img(w-1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w-1,h).g * mask[1][2] +
		img(w-1,h-1).g * mask[2][0] + img(w,h-1).g * mask[2][1] + img(w-1,h-1).g * mask[2][2]
	);
	dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
	dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
	dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);

	return dest;
}

Image32 Image32::edgeDetect3X3( void ) const
{
	Image32 img(*this);
	Image32 dest;
	dest.setSize(img._width,img._height);
	std::vector<std::vector<double>> mask = {{-1.0, -1.0, -1.0},{-1.0, 8.0, -1.0},{-1.0, -1.0, -1.0}};
	for (int h = 1; h < img._height - 1; h++) {
		for (int w = 1; w <img._width - 1; w++) {
			double r = (
				img(w-1,h-1).r * mask[0][0] + img(w,h-1).r * mask[0][1] + img(w+1,h-1).r * mask[0][2] +
				img(w-1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w+1,h).r * mask[1][2] +
				img(w-1,h+1).r * mask[2][0] + img(w,h+1).r * mask[2][1] + img(w+1,h+1).r * mask[2][2]
			);

			double b = (
				img(w-1,h-1).b * mask[0][0] + img(w,h-1).b * mask[0][1] + img(w+1,h-1).b * mask[0][2] +
				img(w-1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w+1,h).b * mask[1][2] +
				img(w-1,h+1).b * mask[2][0] + img(w,h+1).b * mask[2][1] + img(w+1,h+1).b * mask[2][2]
			);
			double g = (
				img(w-1,h-1).g * mask[0][0] + img(w,h-1).g * mask[0][1] + img(w+1,h-1).g * mask[0][2] +
				img(w-1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w+1,h).g * mask[1][2] +
				img(w-1,h+1).g * mask[2][0] + img(w,h+1).g * mask[2][1] + img(w+1,h+1).g * mask[2][2]
			);
			dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
			dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
			dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);
		}
	}

	for (int w = 1; w <img._width - 1; w++){
		int h = 0;
		double r = (
			img(w-1,h+1).r * mask[0][0] + img(w,h+1).r * mask[0][1] + img(w+1,h+1).r * mask[0][2] +
			img(w-1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w+1,h).r * mask[1][2] +
			img(w-1,h+1).r * mask[2][0] + img(w,h+1).r * mask[2][1] + img(w+1,h+1).r * mask[2][2]
		);
		double b = (
			img(w-1,h+1).b * mask[0][0] + img(w,h+1).b * mask[0][1] + img(w+1,h+1).b * mask[0][2] +
			img(w-1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w+1,h).b * mask[1][2] +
			img(w-1,h+1).b * mask[2][0] + img(w,h+1).b * mask[2][1] + img(w+1,h+1).b * mask[2][2]
		);
		double g = (
			img(w-1,h+1).g * mask[0][0] + img(w,h+1).g * mask[0][1] + img(w+1,h+1).g * mask[0][2] +
			img(w-1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w+1,h).g * mask[1][2] +
			img(w-1,h+1).g * mask[2][0] + img(w,h+1).g * mask[2][1] + img(w+1,h+1).g * mask[2][2]
		);
		dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
		dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
		dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);
	}

	for (int w = 1; w <img._width - 1; w++){
		int h = img._height - 1;
		double r = (
			img(w-1,h-1).r * mask[0][0] + img(w,h-1).r * mask[0][1] + img(w+1,h-1).r * mask[0][2] +
			img(w-1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w+1,h).r * mask[1][2] +
			img(w-1,h-1).r * mask[2][0] + img(w,h-1).r * mask[2][1] + img(w+1,h-1).r * mask[2][2]
		);
		double b = (
			img(w-1,h-1).b * mask[0][0] + img(w,h-1).b * mask[0][1] + img(w+1,h-1).b * mask[0][2] +
			img(w-1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w+1,h).b * mask[1][2] +
			img(w-1,h-1).b * mask[2][0] + img(w,h-1).b * mask[2][1] + img(w+1,h-1).b * mask[2][2]
		);
		double g = (
			img(w-1,h-1).g * mask[0][0] + img(w,h-1).g * mask[0][1] + img(w+1,h-1).g * mask[0][2] +
			img(w-1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w+1,h).g * mask[1][2] +
			img(w-1,h-1).g * mask[2][0] + img(w,h-1).g * mask[2][1] + img(w+1,h-1).g * mask[2][2]
		);
		dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
		dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
		dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);
	}

	for (int h = 1; h <img._height - 1; h++) {
		int w = img._width - 1;
		double r = (
			img(w-1,h-1).r * mask[0][0] + img(w,h-1).r * mask[0][1] + img(w-1,h-1).r * mask[0][2] +
			img(w-1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w-1,h).r * mask[1][2] +
			img(w-1,h+1).r * mask[2][0] + img(w,h+1).r * mask[2][1] + img(w-1,h+1).r * mask[2][2]
		);
		double b = (
			img(w-1,h-1).b * mask[0][0] + img(w,h-1).b * mask[0][1] + img(w-1,h-1).b * mask[0][2] +
			img(w-1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w-1,h).b * mask[1][2] +
			img(w-1,h+1).b * mask[2][0] + img(w,h+1).b * mask[2][1] + img(w-1,h+1).b * mask[2][2]
		);
		double g = (
			img(w-1,h-1).g * mask[0][0] + img(w,h-1).g * mask[0][1] + img(w-1,h-1).g * mask[0][2] +
			img(w-1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w-1,h).g * mask[1][2] +
			img(w-1,h+1).g * mask[2][0] + img(w,h+1).g * mask[2][1] + img(w-1,h+1).g * mask[2][2]
		);
		dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
		dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
		dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);
	}

	for (int h = 1; h <img._height - 1; h++) {
		int w = 0;
		double r = (
			img(w+1,h-1).r * mask[0][0] + img(w,h-1).r * mask[0][1] + img(w+1,h-1).r * mask[0][2] +
			img(w+1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w+1,h).r * mask[1][2] +
			img(w+1,h+1).r * mask[2][0] + img(w,h+1).r * mask[2][1] + img(w+1,h+1).r * mask[2][2]
		);
		double b = (
			img(w+1,h-1).b * mask[0][0] + img(w,h-1).b * mask[0][1] + img(w+1,h-1).b * mask[0][2] +
			img(w+1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w+1,h).b * mask[1][2] +
			img(w+1,h+1).b * mask[2][0] + img(w,h+1).b * mask[2][1] + img(w+1,h+1).b * mask[2][2]
		);
		double g = (
			img(w+1,h-1).g * mask[0][0] + img(w,h-1).g * mask[0][1] + img(w+1,h-1).g * mask[0][2] +
			img(w+1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w+1,h).g * mask[1][2] +
			img(w+1,h+1).g * mask[2][0] + img(w,h+1).g * mask[2][1] + img(w+1,h+1).g * mask[2][2]
		);
		dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
		dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
		dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);
	}

	int w = 0;
	int h = 0;
	double r = (
		img(w+1,h+1).r * mask[0][0] + img(w,h+1).r * mask[0][1] + img(w+1,h+1).r * mask[0][2] +
		img(w+1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w+1,h).r * mask[1][2] +
		img(w+1,h+1).r * mask[2][0] + img(w,h+1).r * mask[2][1] + img(w+1,h+1).r * mask[2][2]
	);
	double b = (
		img(w+1,h+1).b * mask[0][0] + img(w,h+1).b * mask[0][1] + img(w+1,h+1).b * mask[0][2] +
		img(w+1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w+1,h).b * mask[1][2] +
		img(w+1,h+1).b * mask[2][0] + img(w,h+1).b * mask[2][1] + img(w+1,h+1).b * mask[2][2]
	);
	double g = (
		img(w+1,h+1).g * mask[0][0] + img(w,h+1).g * mask[0][1] + img(w+1,h+1).g * mask[0][2] +
		img(w+1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w+1,h).g * mask[1][2] +
		img(w+1,h+1).g * mask[2][0] + img(w,h+1).g * mask[2][1] + img(w+1,h+1).g * mask[2][2]
	);
	dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
	dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
	dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);

	w = 0;
	h = img._height - 1;
	r = (
		img(w+1,h-1).r * mask[0][0] + img(w,h-1).r * mask[0][1] + img(w+1,h-1).r * mask[0][2] +
		img(w+1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w+1,h).r * mask[1][2] +
		img(w+1,h-1).r * mask[2][0] + img(w,h-1).r * mask[2][1] + img(w+1,h-1).r * mask[2][2]
	);
	b = (
		img(w+1,h-1).b * mask[0][0] + img(w,h-1).b * mask[0][1] + img(w+1,h-1).b * mask[0][2] +
		img(w+1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w+1,h).b * mask[1][2] +
		img(w+1,h-1).b * mask[2][0] + img(w,h-1).b * mask[2][1] + img(w+1,h-1).b * mask[2][2]
	);
	g = (
		img(w+1,h-1).g * mask[0][0] + img(w,h-1).g * mask[0][1] + img(w+1,h-1).g * mask[0][2] +
		img(w+1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w+1,h).g * mask[1][2] +
		img(w+1,h-1).g * mask[2][0] + img(w,h-1).g * mask[2][1] + img(w+1,h-1).g * mask[2][2]
	);
	dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
	dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
	dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);

	w = img._width - 1;
	h = 0;
	r = (
		img(w-1,h+1).r * mask[0][0] + img(w,h+1).r * mask[0][1] + img(w-1,h+1).r * mask[0][2] +
		img(w-1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w-1,h).r * mask[1][2] +
		img(w-1,h+1).r * mask[2][0] + img(w,h+1).r * mask[2][1] + img(w-1,h+1).r * mask[2][2]
	);
	b = (
		img(w-1,h+1).b * mask[0][0] + img(w,h+1).b * mask[0][1] + img(w-1,h+1).b * mask[0][2] +
		img(w-1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w-1,h).b * mask[1][2] +
		img(w-1,h+1).b * mask[2][0] + img(w,h+1).b * mask[2][1] + img(w-1,h+1).b * mask[2][2]
	);
	g = (
		img(w-1,h+1).g * mask[0][0] + img(w,h+1).g * mask[0][1] + img(w-1,h+1).g * mask[0][2] +
		img(w-1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w-1,h).g * mask[1][2] +
		img(w-1,h+1).g * mask[2][0] + img(w,h+1).g * mask[2][1] + img(w-1,h+1).g * mask[2][2]
	);
	dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
	dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
	dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);

	w = img._width - 1;
	h = img._height - 1;
	r = (
		img(w-1,h-1).r * mask[0][0] + img(w,h-1).r * mask[0][1] + img(w-1,h-1).r * mask[0][2] +
		img(w-1,h).r * mask[1][0] + img(w,h).r * mask[1][1] + img(w-1,h).r * mask[1][2] +
		img(w-1,h-1).r * mask[2][0] + img(w,h-1).r * mask[2][1] + img(w-1,h-1).r * mask[2][2]
	);
	b = (
		img(w-1,h-1).b * mask[0][0] + img(w,h-1).b * mask[0][1] + img(w-1,h-1).b * mask[0][2] +
		img(w-1,h).b * mask[1][0] + img(w,h).b * mask[1][1] + img(w-1,h).b * mask[1][2] +
		img(w-1,h-1).b * mask[2][0] + img(w,h-1).b * mask[2][1] + img(w-1,h-1).b * mask[2][2]
	);
	g = (
		img(w-1,h-1).g * mask[0][0] + img(w,h-1).g * mask[0][1] + img(w-1,h-1).g * mask[0][2] +
		img(w-1,h).g * mask[1][0] + img(w,h).g * mask[1][1] + img(w-1,h).g * mask[1][2] +
		img(w-1,h-1).g * mask[2][0] + img(w,h-1).g * mask[2][1] + img(w-1,h-1).g * mask[2][2]
	);
	dest(w,h).r = (unsigned char) std::min((int) std::max((int) r, 0), 255);
	dest(w,h).g = (unsigned char) std::min((int) std::max((int) g, 0), 255);
	dest(w,h).b = (unsigned char) std::min((int) std::max((int) b, 0), 255);

	return dest;
}

Image32 Image32::scaleNearest( double scaleFactor ) const
{
	Image32 img(*this);
	Image32 dest;
	dest.setSize((int) img._width * scaleFactor, (int) img._height * scaleFactor);	

	for (int h = 1; h < dest._height - 1; h++) {
		for (int w = 1; w <dest._width - 1; w++) {
			int s_h = floor((h / scaleFactor) + 0.5);
			int s_w = floor((w / scaleFactor) + 0.5);
			dest(w,h) = img(s_w,s_h);
		}
	}

	return dest;	
}

Image32 Image32::scaleBilinear( double scaleFactor ) const
{
	Image32 img(*this);
	Image32 dest;
	dest.setSize((int) img._width * scaleFactor, (int) img._height * scaleFactor);	

	for (int h = 1; h < dest._height - 1; h++) {
		for (int w = 1; w <dest._width - 1; w++) {
			Point2D p;
			p[0] = w / scaleFactor;
			p[1] = h / scaleFactor;
			dest(w,h) = img.bilinearSample(p);
		}
	}

	return dest;
}

Image32 Image32::scaleGaussian( double scaleFactor ) const
{
	////////////////////////////////////////////
	// Do scaling with Gaussian sampling here //
	////////////////////////////////////////////
	Image32 img(*this);
	Image32 dest;
	dest.setSize((int) img._width * scaleFactor, (int) img._height * scaleFactor);	

	for (int h = 1; h < dest._height - 1; h++) {
		for (int w = 1; w <dest._width - 1; w++) {
			Point2D p;
			p[0] = w / scaleFactor;
			p[1] = h / scaleFactor;
			dest(w,h) = img.gaussianSample(p, 0.5, 0.25);
		}
	}

	return dest;
}


Image32 Image32::rotateNearest( double angle ) const
{
	//////////////////////////////////////////////////
	// Do rotation with nearest-point sampling here //
	//////////////////////////////////////////////////
	Image32 img(*this);
	double rad = angle * (M_PI / 180.0);
	double cosine = cos(rad);
	double sine = sin(rad);
	int width = int(img._height * sine + img._width * cosine);
	int height = int(img._height * cosine + img._width * sine);
	int the_longer_side = std::max(width, height);
	Image32 newimage; 
	newimage.setSize(width, height);
	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {
			double x = cosine * ( w - (width / 2)) - sine * (h - (height / 2)) + img._width / 2;
			double y = sine * ( w - (width / 2)) + cosine * (h - (height / 2)) + img._height / 2;
			if ( 0 < x && x < 199 && 0 < y && y < 199) {
				newimage(w,h) = img.nearestSample(Point2D((int)x, (int)y));
			}
		}
	}
	return newimage;
}

Image32 Image32::rotateBilinear( double angle ) const
{
	/////////////////////////////////////////////
	// Do rotation with bilinear sampling here //
	/////////////////////////////////////////////
	Image32 img(*this);
	double rad = angle * (M_PI / 180.0);
	double cosine = cos(rad);
	double sine = sin(rad);
	int width = int(img._height * sine + img._width * cosine);
	int height = int(img._height * cosine + img._width * sine);
	int the_longer_side = std::max(width, height);
	Image32 newimage; 
	newimage.setSize(width, height);
	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {
			double x = cosine * ( w - (width / 2)) - sine * (h - (height / 2)) + img._width / 2;
			double y = sine * ( w - (width / 2)) + cosine * (h - (height / 2)) + img._height / 2;
			if ( 0 < x && x < 199 && 0 < y && y < 199) {
				newimage(w,h) = img.bilinearSample(Point2D(floor(x), floor(y)));
			}
		}
	}
	return newimage;
}

Image32 Image32::rotateGaussian( double angle ) const
{
	/////////////////////////////////////////////
	// Do rotation with Gaussian sampling here //
	/////////////////////////////////////////////
	Image32 img(*this);
	double rad = angle * (M_PI / 180.0);
	double cosine = cos(rad);
	double sine = sin(rad);
	int width = int(img._height * sine + img._width * cosine);
	int height = int(img._height * cosine + img._width * sine);
	int the_longer_side = std::max(width, height);
	Image32 newimage; 
	newimage.setSize(width, height);
	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {
			double x = cosine * ( w - (width / 2)) - sine * (h - (height / 2)) + img._width / 2;
			double y = sine * ( w - (width / 2)) + cosine * (h - (height / 2)) + img._height / 2;
			if ( 0 < x && x < 199 && 0 < y && y < 199) {
				newimage(w,h) = img.gaussianSample(Point2D(floor(x), floor(y)), 0.25, 0.5);
			}
		}
	}
	return newimage;
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
	WARN( "BUG" );
	Image32 dest(destination);
	Image32 img(source);

	for (int h = 0; h < dest._height; h++) {
		for (int w = 0; w < dest._width; w++) {
			dest(w,h).a = (unsigned char) (blendWeight * 255.0);
		}
	}

	for (int h = 0; h < img._height; h++) {
		for (int w = 0; w < img._width; w++) {
			double frac = dest(w,h).a / 255.0;
			img(w,h).r = frac * dest(w,h).r + (1.0 - frac) * img(w,h).r;
			img(w,h).b = frac * dest(w,h).b + (1.0 - frac) * img(w,h).b;
			img(w,h).g = frac * dest(w,h).g + (1.0 - frac) * img(w,h).g;
		}
	}

	return img;
	
}

Image32 Image32::warp( const OrientedLineSegmentPairs& olsp ) const
{
	/////////////////////
	// Do warping here //
	/////////////////////

	Image32 img(*this);
	Image32 dest;
	dest.setSize(img._width,img._height);

	for (int h = 1; h < dest._height ; h++) {
		for (int w = 1; w < dest._width ; w++) {
			if (olsp.getSourcePosition(Point2D(w,h))[0] > 0 &&  olsp.getSourcePosition(Point2D(w,h))[1] > 0) { 
			 	dest(w,h) = img.nearestSample(olsp.getSourcePosition(Point2D(w,h)));
			}
		}
	}

	return dest;
}

Image32 Image32::funFilter( void ) const
{
	Image32 img(*this);
	Image32 dest;
	dest.setSize(img._width,img._height);
	for (int h = 0; h <= img._height - 1; h++) {
		for (int w = 0; w <= img._width - 1; w++) {
			dest(w,h) = img(img._width - 1 - w,img._height - 1 - h);
		}
	}
	return dest;
}

Image32 Image32::crop( int x1 , int y1 , int x2 , int y2 ) const
{
	//////////////////////
	// Do cropping here //
	//////////////////////
	Image32 img(*this);
	Image32 dest;
	dest.setSize((x2-x1+1),(y2-y1+1));
	for (int h = 0; h <= dest._height - 1; h++) {
		for (int w = 0; w <= dest._width - 1; w++) {
			dest(w,h) = img(x1+w,y1+h);
		}
	}
	return dest;
}

Pixel32 Image32::nearestSample( Point2D p ) const
{
	//////////////////////////////
	// Do nearest sampling here //
	//////////////////////////////
	Pixel32 newpixel;

	Image32 img(*this);

	int s_w = floor(p[0] + 0.5);
	int s_h = floor(p[1] + 0.5);

	newpixel = img(s_w,s_h);

	return newpixel;
}

Pixel32 Image32::bilinearSample( Point2D p ) const
{
	///////////////////////////////
	// Do bilinear sampling here //
	///////////////////////////////
	Pixel32 newpixel;

	Image32 img(*this);

	int s_w_u_1 = floor(p[0]);
	int s_h_v_1 = floor(p[1]);
	int s_w_u_2 = s_w_u_1 + 1;
	int s_h_v_2 = s_h_v_1 + 1;
	if (s_w_u_2 >= _width) s_w_u_2 = s_w_u_1 - 1;
	if (s_h_v_2 >= _height) s_h_v_2 = s_h_v_1 - 1;
	double du = p[0] - s_w_u_1;
	double dv = p[1] - s_h_v_1;

	double a_red = (double) (1-du) * img(s_w_u_1,s_h_v_1).r  + du * img(s_w_u_2,s_h_v_1).r;
	double a_green = (double) (1-du) * img(s_w_u_1,s_h_v_1).g  + du * img(s_w_u_2,s_h_v_1).g;
	double a_blue = (double) (1-du) * img(s_w_u_1,s_h_v_1).b  + du * img(s_w_u_2,s_h_v_1).b;

	double b_red = (double) (1-du) * img(s_w_u_1,s_h_v_2).r  + du * img(s_w_u_2,s_h_v_2).r;
	double b_green = (double) (1-du) * img(s_w_u_1,s_h_v_2).g  + du * img(s_w_u_2,s_h_v_2).g;
	double b_blue = (double) (1-du) * img(s_w_u_1,s_h_v_2).b  + du * img(s_w_u_2,s_h_v_2).b;

	double red = a_red * (1 - dv) + b_red * dv;
	double blue = a_blue * (1 - dv) + b_blue * dv;
	double green = a_green * (1 - dv) + b_green * dv;

	newpixel.r = (unsigned char) std::min((int) std::max((int) red, 0), 255);
	newpixel.b = (unsigned char) std::min((int) std::max((int) blue, 0), 255);
	newpixel.g = (unsigned char) std::min((int) std::max((int) green, 0), 255);

	return newpixel;
}

Pixel32 Image32::gaussianSample( Point2D p , double variance , double radius ) const
{
	///////////////////////////////
	// Do Gaussian sampling here //
	///////////////////////////////
	Pixel32 newpixel;
	Image32 img(*this);
	double red = 0.0;
	double green = 0.0;
	double blue = 0.0;
	int upper = ceil(p[1] + radius);
	int lower = floor(p[1] - radius);
	int left = floor(p[0] - radius);
	int right = ceil(p[0] + radius);
	double normality = 0.0;
	for (int h = lower; h < upper; h++){
		for (int w = left; w < right; w++){
			if ( pow(w - p[0],2) + pow(h - p[1],2) > pow(radius,2)) continue;
			double gaussian_Distri = (1.0 / (2.0 * M_PI * variance)) * exp( - (pow(w - p[0],2) + pow(h - p[1],2)) / (2 * variance) );
			red += (double) img(w,h).r * gaussian_Distri;
			blue += (double) img(w,h).b * gaussian_Distri;
			green += (double) img(w,h).g * gaussian_Distri;
			normality += gaussian_Distri;
		}
	}
	red = red / normality;
	green = green / normality;
	blue = blue / normality;
	newpixel.r = (unsigned char) std::min((int) std::max((int) red, 0), 255);
	newpixel.g = (unsigned char) std::min((int) std::max((int) green, 0), 255);
	newpixel.b = (unsigned char) std::min((int) std::max((int) blue, 0), 255);
	return newpixel;
}
