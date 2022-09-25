#include <string.h>
#include <stdlib.h>
#include "image.h"
#include <Util/cmdLineParser.h>
#include <Util/exceptions.h>
#include <Image/bmp.h>
#include <Image/jpeg.h>

using namespace std;
using namespace Util;
using namespace Image;

/////////////
// Pixel32 //
/////////////
Pixel32::Pixel32( void ) : r(0) , g(0) , b(0) , a(255) {}

/////////////
// Image32 //
/////////////
Image32::Image32( void ) : _width(0) , _height(0) , _pixels(NULL) {}

Image32::Image32( const Image32& img ) : _width(0) , _height(0) , _pixels(NULL)
{
	setSize( img._width , img._height );
	memcpy( _pixels , img._pixels , sizeof(Pixel32)*_width*_height );
}

Image32& Image32::operator = ( const Image32& img )
{
	setSize( img._width , img._height );
	memcpy( _pixels , img._pixels , sizeof(Pixel32)*_width*_height );
	return *this;
}

Image32::Image32( Image32&& img )
{
	_width = img._width , _height = img._height;
	_pixels = img._pixels;
	img._width = img._height = 0;
	img._pixels = NULL;
}

Image32& Image32::operator = ( Image32&& img )
{
	swap( _width , img._width );
	swap( _height , img._height );
	swap( _pixels , img._pixels );
	return *this;
}

Image32::~Image32( void ){ setSize(0,0); }

void Image32::setSize( int width , int height )
{
	if( _width!=width || _height!=height )
	{
		if( _pixels ) delete[] _pixels;
		_pixels = NULL;
		_width = _height = 0;
		if( !width*height ) return;
		_pixels = new Pixel32[width*height];
		if( !_pixels ) THROW( "Failed to allocate memory for image: " , width , " x " , height );
	}
	_width = width;
	_height = height;
	memset( _pixels , 0 , sizeof(Pixel32)*_width*_height );
}

void Image32::_assertInBounds( int x , int y ) const
{
	if( x<0 || x>=_width || y<0 || y>=_height ) THROW( "Pixel index out of range: ( " , x , " , " , y , " ) no in [ 0 , " , _width , " ) x [ 0 , " , _height , " )" );
}

Pixel32& Image32::operator() ( int x , int y )
{
	_assertInBounds( x , y );
	return _pixels[x+y*_width];
}

const Pixel32& Image32::operator() ( int x , int y ) const
{
	_assertInBounds( x , y );
	return _pixels[x+y*_width];
}

int Image32::width( void ) const { return _width; }

int Image32::height( void ) const { return _height; }

Image32 Image32::BeierNeelyMorph( const Image32& source , const Image32& destination , const OrientedLineSegmentPairs& olsp , double timeStep )
{
	OrientedLineSegmentPairs olsp1 , olsp2;
	OrientedLineSegment ols;
	Image32 temp1 , temp2;

	// Generate the in-between line segment pairs
	olsp1.resize( olsp.size() );
	olsp2.resize( olsp.size() );

	for( int i=0 ; i<olsp.size() ; i++ )
	{
		olsp1[i].first = olsp[i].first;
		olsp2[i].first = olsp[i].second;
		ols = olsp[i].first * (1.-timeStep) + olsp[i].second * timeStep;
		olsp1[i].second = ols;
		olsp2[i].second = ols;
	}

	// Generate the in-between morphs
	temp1 = source.warp( olsp1 );
	temp2 = destination.warp( olsp2 );

	// Cross-dissolve to get the final image
	return CrossDissolve( temp1 , temp2 , timeStep );
}

void Image32::read( string fileName )
{
	string ext = ToLower( GetFileExtension( fileName ) );
	if     ( ext=="bmp" ) BMPReadImage( fileName , *this );
	else if( ext=="jpg" || ext=="jpeg" ) JPEGReadImage( fileName , *this );
	else THROW( "Unrecognized file extension: " , ext );
}

void Image32::write( string fileName ) const
{
	string ext = ToLower( GetFileExtension( fileName ) );
	if( !( width()*height() ) ) THROW( "Cannot write empty image: %s" , fileName.c_str() );
	if     ( ext=="bmp" ) BMPWriteImage( *this , fileName );
	else if( ext=="jpg" || ext=="jpeg" ) JPEGWriteImage( *this , fileName );
	else THROW( "Unrecognized file extension: " , ext );
}