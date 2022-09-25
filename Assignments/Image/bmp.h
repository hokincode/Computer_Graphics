#ifndef BMP_INCLUDED
#define BMP_INCLUDED

#include "image.h"

namespace Image
{
	/** This function read in a BMP file, returning 0 on failure.*/
	void BMPReadImage( std::string fileName , Image32& img );
	/** This function read in a BMP file, returning 0 on failure.*/
	void BMPReadImage( FILE *fp , Image32& img );

	/** This function writes out a BMP file, returning 0 on failure.*/
	void BMPWriteImage( const Image32& img , std::string fileName );
	/** This function writes out a BMP file, returning 0 on failure.*/
	void BMPWriteImage( const Image32& img , FILE *fp );
}
#endif // BMP_INCLUDED
