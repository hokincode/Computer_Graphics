#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Util/exceptions.h>
#include <Util/geometry.h>
#include "sphereLight.h"
#include "scene.h"

using namespace Ray;
using namespace Util;

/////////////////
// SphereLight //
/////////////////

void SphereLight::_read( std::istream &stream )
{
	stream >> _ambient >> _diffuse >> _specular >> _location >> _radius >> _constAtten >> _linearAtten >> _quadAtten;
	if( !stream ) THROW( "Failed to parse " , Directive() );
}

void SphereLight::_write( std::ostream &stream ) const
{
	stream << "#" << Directive() << "  " << _ambient << "  " << _diffuse << "  " << _specular << "  " << _location << "  " << _radius << " " << _constAtten << " " << _linearAtten << " " << _quadAtten;
}
