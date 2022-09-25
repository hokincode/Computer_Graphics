#include <cmath>
#include <Util/exceptions.h>
#include "cone.h"
#include "scene.h"

using namespace Ray;
using namespace Util;

//////////
// Cone //
//////////

Cone::Cone( void ) : height(0) , radius(0) , _material(NULL) , _elementBufferID(0) , _vertexBufferID(0) {}

void Cone::_read( std::istream &stream )
{
	if( !( stream >> _materialIndex >> center >> radius >> height ) ) THROW( "failed to parse " , Directive().c_str() );
}

void Cone::_write( std::ostream &stream ) const
{
	Shape::WriteInset( stream );
	stream << "#" << Directive() << "  " << _materialIndex << "  " << center << "  " << radius << "  " << height;
}
