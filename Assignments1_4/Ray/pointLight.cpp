#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Util/exceptions.h>
#include <Util/geometry.h>
#include "pointLight.h"

using namespace Ray;
using namespace Util;

////////////////
// PointLight //
////////////////

void PointLight::_read( std::istream &stream )
{
	stream >> _ambient >> _diffuse >> _specular >> _location >> _constAtten >> _linearAtten >> _quadAtten;
	if( !stream ) THROW( "Failed to parse " , Directive() );
}

void PointLight::_write( std::ostream &stream ) const
{
	stream << "#" << Directive() << "  " << _ambient << "  " << _diffuse << "  " << _specular << "  " << _location << "  " << _constAtten << " " << _linearAtten << " " << _quadAtten;
}
