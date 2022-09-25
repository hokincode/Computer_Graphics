#ifndef JPEG_INCLUDED
#define JPEG_INCLUDED

#include "image.h"

namespace Image
{
	/** This function read in a JPEG file, returning 0 on failure.*/
	void JPEGReadImage( std::string fileName , Image32& img );
	/** This function read in a JPEG file, returning 0 on failure.*/
	void JPEGReadImage( FILE *fp , Image32& img );

	/** This function writes out a JPEG file, returning 0 on failure.*/
	void JPEGWriteImage( const Image32& img , std::string , int quality=100 );
	/** This function writes out a JPEG file, returning 0 on failure.*/
	void JPEGWriteImage( const Image32& img , FILE *fp , int quality );
}
#endif // JPEG_INCLUDED
