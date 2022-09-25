#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Util/exceptions.h>
#include <Util/geometry.h>
#include "spotLight.h"
#include "scene.h"

using namespace Ray;
using namespace Util;

///////////////
// SpotLight //
///////////////

void SpotLight::_read( std::istream &stream )
{
	stream >> _ambient >> _diffuse >> _specular >> _location >> _direction >> _constAtten >> _linearAtten >> _quadAtten >> _cutOffAngle >> _dropOffRate;
	if( !stream ) THROW( "Failed to parse " , Directive() );
	_direction = _direction.unit();
}

void SpotLight::_write( std::ostream &stream ) const
{
	stream << "#" << Directive() << "  " << _ambient << "  " << _diffuse << "  " << _specular << "  " << _location << "  " << _constAtten << " " << _linearAtten << " " << _quadAtten << "  " << _cutOffAngle << "  " << _dropOffRate;
}
