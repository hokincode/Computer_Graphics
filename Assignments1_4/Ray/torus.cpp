#include <cmath>
#include <Util/exceptions.h>
#include "torus.h"
#include "scene.h"

using namespace Ray;
using namespace Util;

///////////
// Torus //
///////////

Torus::Torus( void ) : iRadius(0) , oRadius(0) , _material(NULL) , _elementBufferID(0) , _vertexBufferID(0) {}

void Torus::_read( std::istream &stream )
{
	if( !( stream >> _materialIndex >> center >> iRadius >> oRadius ) ) THROW( "failed to parse " , Directive() );
}

void Torus::_write( std::ostream &stream ) const
{
	Shape::WriteInset( stream );
	stream << "#" << Directive() << "  " << _materialIndex << "  " << center << "  " << iRadius << "  " << oRadius;
}
