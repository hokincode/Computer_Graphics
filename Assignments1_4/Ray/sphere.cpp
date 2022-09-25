#include <cmath>
#include <Util/exceptions.h>
#include "sphere.h"
#include "scene.h"

using namespace Ray;
using namespace Util;

////////////
// Sphere //
////////////
Sphere::Sphere( void ) : radius(0) , _material(NULL) , _elementBufferID(0) , _vertexBufferID(0) {}

void Sphere::_read( std::istream &stream )
{
	if( !( stream >> _materialIndex >> center >> radius ) ) THROW( "failed to parse " , Directive() );
}

void Sphere::_write( std::ostream &stream ) const
{
	Shape::WriteInset( stream );
	stream << "#" << Directive() << "  " << _materialIndex << "  " << center << "  " << radius;
}
