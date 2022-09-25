#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Util/geometry.h>
#include <Util/exceptions.h>
#include "directionalLight.h"

using namespace Ray;
using namespace Util;

//////////////////////
// DirectionalLight //
//////////////////////

void DirectionalLight::_read( std::istream &stream )
{
	stream >> _ambient >> _diffuse >> _specular >> _direction;
	if( !stream ) THROW( "Failed to parse " , Directive() );
	_direction = _direction.unit();
}

void DirectionalLight::_write( std::ostream &stream ) const
{
	stream << "#" << Directive() << "  " << _ambient << "  " << _diffuse << "  " << _specular << "  " << _direction;
}
