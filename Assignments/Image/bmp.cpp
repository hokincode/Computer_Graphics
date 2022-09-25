#include <stdlib.h>
#include <Util/exceptions.h>
#include "bmp.h"

typedef char BYTE;					/* 8 bits */
typedef unsigned short int WORD;	/* 16-bit unsigned integer. */
typedef unsigned int DWORD;			/* 32-bit unsigned integer */
typedef int LONG;					/* 32-bit signed integer */

typedef struct tagBITMAPFILEHEADER
{
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
	DWORD biSize;
	LONG biWidth;
	LONG biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG biXPelsPerMeter;
	LONG biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
} BITMAPINFOHEADER;

/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

typedef struct tagRGBTRIPLE
{
	BYTE rgbtBlue;
	BYTE rgbtGreen;
	BYTE rgbtRed;
} RGBTRIPLE;

typedef struct tagRGBQUAD
{
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
	BYTE rgbReserved;
} RGBQUAD;

/* Some magic numbers */

#define BMP_BF_TYPE 0x4D42
/* word BM */

#define BMP_BF_OFF_BITS 54
/* 14 for file header + 40 for info header (not sizeof(), but packed size) */

#define BMP_BI_SIZE 40
/* packed size of info header */

/* Reads a WORD from a file in little endian format */
static WORD WordReadLE( FILE *fp )
{
	WORD lsb , msb;

	lsb = getc(fp);
	msb = getc(fp);
	return ( msb<<8 ) | lsb;
}

/* Writes a WORD to a file in little endian format */
static void WordWriteLE( WORD x , FILE *fp )
{
	BYTE lsb , msb;

	lsb = (BYTE) ( x & 0x00FF);
	msb = (BYTE) ( x>>8 );
	putc( lsb , fp );
	putc( msb , fp );
}

/* Reads a DWORD word from a file in little endian format */
static DWORD DWordReadLE( FILE *fp )
{
	DWORD b1 , b2 , b3 , b4;

	b1 = getc(fp);
	b2 = getc(fp);
	b3 = getc(fp);
	b4 = getc(fp);
	return ( b4<<24 ) | ( b3<<16 ) | ( b2<<8 ) | b1;
}

/* Writes a DWORD to a file in little endian format */
static void DWordWriteLE( DWORD x , FILE *fp )
{
	unsigned char b1 , b2 , b3 , b4;

	b1 = (   x      & 0x000000FF );
	b2 = ( ( x>> 8) & 0x000000FF );
	b3 = ( ( x>>16) & 0x000000FF );
	b4 = ( ( x>>24) & 0x000000FF );
	putc( b1 , fp );
	putc( b2 , fp );
	putc( b3 , fp );
	putc( b4 , fp );
}

/* Reads a LONG word from a file in little endian format */
static LONG LongReadLE( FILE *fp )
{
	LONG b1, b2, b3, b4;

	b1 = getc( fp );
	b2 = getc( fp );
	b3 = getc( fp );
	b4 = getc( fp );
	return ( b4<<24 ) | ( b3<<16 ) | ( b2<<8 ) | b1;
}

/* Writes a LONG to a file in little endian format */
static void LongWriteLE( LONG x , FILE *fp )
{
	char b1 , b2 , b3 , b4;

	b1 = (  x      & 0x000000FF );
	b2 = ( (x>> 8) & 0x000000FF );
	b3 = ( (x>>16) & 0x000000FF );
	b4 = ( (x>>24) & 0x000000FF );
	putc( b1 , fp );
	putc( b2 , fp );
	putc( b3 , fp );
	putc( b4 , fp );
}

namespace Image
{
	void BMPReadImage( FILE *fp , Image32& img )
	{
		BITMAPFILEHEADER bmfh;
		BITMAPINFOHEADER bmih;
		int x, y;
		int lineLength;

		if( !fp ) THROW( "Empty file pointer" );

		/* Read file header */

		/* fread(&bmfh, sizeof(bmfh), 1, fp); */
		/* fread won't work on different platforms because of endian
		* issues.  Sigh... */
		bmfh.bfType = WordReadLE(fp);
		bmfh.bfSize = DWordReadLE(fp);
		bmfh.bfReserved1 = WordReadLE(fp);
		bmfh.bfReserved2 = WordReadLE(fp);
		bmfh.bfOffBits = DWordReadLE(fp);

		/* Check file header */
		if( bmfh.bfType!=BMP_BF_TYPE ) THROW( "Inavlid header" );
		/* ignore bmfh.bfSize */
		/* ignore bmfh.bfReserved1 */
		/* ignore bmfh.bfReserved2 */
		if( bmfh.bfOffBits!=BMP_BF_OFF_BITS ) THROW( "Inavlid header" );

		/* Read info header */

		/* fread(&bmih, sizeof(bmih), 1, fp); */
		/* same problem as above... */

		bmih.biSize = DWordReadLE(fp);
		bmih.biWidth = LongReadLE(fp);
		bmih.biHeight = LongReadLE(fp);
		bmih.biPlanes = WordReadLE(fp);
		bmih.biBitCount = WordReadLE(fp);
		bmih.biCompression = DWordReadLE(fp);
		bmih.biSizeImage = DWordReadLE(fp);
		bmih.biXPelsPerMeter = LongReadLE(fp);
		bmih.biYPelsPerMeter = LongReadLE(fp);
		bmih.biClrUsed = DWordReadLE(fp);
		bmih.biClrImportant = DWordReadLE(fp);
		/* Check info header */
		if( bmih.biSize!=BMP_BI_SIZE ) THROW( "Bad size: " , bmih.biSize , " != " , BMP_BI_SIZE );
		if( bmih.biWidth<=0 ) THROW( "Bad width: " , bmih.biWidth , " <= 0" );
		if( bmih.biHeight<=0 ) THROW( "Bad height: " , bmih.biHeight , " <= 0" );
		if( bmih.biPlanes!=1 ) THROW( "Bad number of planes: " , bmih.biPlanes , " != 1" );
		if( bmih.biBitCount!=24 ) THROW( "Bad bit count: " , bmih.biBitCount , " != 24" );
		if( bmih.biCompression!=BI_RGB ) THROW( "Bad compression type: " , bmih.biCompression , " != " , BI_RGB );
		lineLength = bmih.biWidth * 3;	/* RGB */
		if( (lineLength % 4)!=0 ) lineLength = (lineLength / 4 + 1) * 4;
		if( bmih.biSizeImage!=(DWORD) lineLength * (DWORD) bmih.biHeight ) THROW( "Image size doesn't match line-length times height: " , bmih.biSizeImage , " != " , bmih.biHeight , " x " , bmih.biHeight );

		/* ignore bmih.biXPelsPerMeter */
		/* ignore bmih.biYPelsPerMeter */
		/* ignore bmih.biClrUsed - we assume a true color display, and
		* won't use palettes */
		/* ignore bmih.biClrImportant - same reason */

		/* Creates the image */
		//    img = new Image(bmih.biWidth, bmih.biHeight);
		img.setSize( bmih.biWidth , bmih.biHeight );
		/* Read triples */
		/* RGB */
		{
			RGBTRIPLE *triples;
			triples = new RGBTRIPLE[lineLength];
			if( !triples ) THROW( "Could not allocate triples[" , lineLength , "]" );
			fseek( fp , (long) bmfh.bfOffBits , SEEK_SET );

			for( y=0 ; y<img.height() ; y++ )
			{
				if( fread( triples , 1 , lineLength , fp )!=lineLength )
				{
					delete[] triples;
					THROW( "Could not read triples" );
				}
				if( ferror(fp) )
				{
					delete[] triples;
					THROW( "Failed to read triples" );
				}

				/* Copy triples */
				for( x=0 ; x<img.width() ; x++ )
				{
					Pixel32 p;
					p.r = triples[x].rgbtRed;
					p.g = triples[x].rgbtGreen;
					p.b = triples[x].rgbtBlue;
					p.a = 255;
					img( x , img.height()-y-1 ) = p;
				}
			}
			delete[] triples;
		}
	}

	void BMPWriteImage( const Image32& img , FILE *fp )
	{
		BITMAPFILEHEADER bmfh;
		BITMAPINFOHEADER bmih;
		int x, y;
		int lineLength;
		Pixel32 p;

		lineLength = img.width() * 3;	/* RGB */
		if( (lineLength % 4)!=0 ) lineLength = (lineLength / 4 + 1) * 4;

		/* Write file header */

		bmfh.bfType = BMP_BF_TYPE;
		bmfh.bfSize = BMP_BF_OFF_BITS + lineLength * img.height();
		bmfh.bfReserved1 = 0;
		bmfh.bfReserved2 = 0;
		bmfh.bfOffBits = BMP_BF_OFF_BITS;

		WordWriteLE( bmfh.bfType , fp );
		DWordWriteLE( bmfh.bfSize , fp );
		WordWriteLE( bmfh.bfReserved1 , fp );
		WordWriteLE( bmfh.bfReserved2 , fp );
		DWordWriteLE( bmfh.bfOffBits , fp );

		/* Write info header */

		bmih.biSize = BMP_BI_SIZE;
		bmih.biWidth = img.width();
		bmih.biHeight = img.height();
		bmih.biPlanes = 1;
		bmih.biBitCount = 24;		/* RGB */
		bmih.biCompression = BI_RGB;	/* RGB */
		bmih.biSizeImage = lineLength * (DWORD) bmih.biHeight;	/* RGB */
		bmih.biXPelsPerMeter = 2925;
		bmih.biYPelsPerMeter = 2925;
		bmih.biClrUsed = 0;
		bmih.biClrImportant = 0;

		DWordWriteLE( bmih.biSize , fp );
		LongWriteLE ( bmih.biWidth , fp );
		LongWriteLE ( bmih.biHeight , fp );
		WordWriteLE ( bmih.biPlanes , fp );
		WordWriteLE ( bmih.biBitCount , fp );
		DWordWriteLE( bmih.biCompression , fp );
		DWordWriteLE( bmih.biSizeImage , fp );
		LongWriteLE ( bmih.biXPelsPerMeter , fp );
		LongWriteLE ( bmih.biYPelsPerMeter , fp );
		DWordWriteLE( bmih.biClrUsed , fp );
		DWordWriteLE( bmih.biClrImportant , fp );

		/* Write pixels */
		for( y=0 ; y<img.height() ; y++ )
		{
			int nbytes = 0;
			for( x=0 ; x<img.width() ; x++ )
			{
				p=img(x,img.height()-y-1);
				putc(p.b, fp), nbytes++;
				putc(p.g, fp), nbytes++;
				putc(p.r, fp), nbytes++;
				/* putc(p->a, fp), nbytes++; */
				/* RGB */
			}
			/* Padding for 32-bit boundary */
			while ( (nbytes % 4)!=0 )
			{
				putc(0, fp);
				nbytes++;
			}
		}
	}

	void BMPReadImage( std::string fileName , Image32& img )
	{
		FILE *fp;

		fp = fopen( fileName.c_str() , "rb" );
		if( !fp ) THROW( "Could not open file for reading: " , fileName );
		BMPReadImage( fp , img );
		fclose(fp);
	}

	void BMPWriteImage( const Image32& img , std::string fileName )
	{
		FILE *fp;

		fp = fopen( fileName.c_str() , "wb" );
		if( !fp ) THROW( "Could not open file for writing: " , fileName );
		BMPWriteImage( img , fp );
		fclose(fp);
	}
}