#include <cmath>
#include <Util/exceptions.h>
#include "box.h"
#include "scene.h"

using namespace std;
using namespace Ray;
using namespace Util;

/////////
// Box //
/////////
Box::Box( void ) : _material(NULL) {}

void Box::_read( istream &stream )
{
	if( !( stream >> _materialIndex >> center >> length ) ) THROW( "failed to parse " , Directive() );
}

void Box::_write( ostream &stream ) const
{
	Shape::WriteInset( stream );
	stream << "#" << Directive() << "  " << _materialIndex << "  " << center << "  " << length;
}
