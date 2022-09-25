#include <string>
#include <Util/exceptions.h>
#include "shape.h"
#include "camera.h"

using namespace Ray;
using namespace Util;

namespace Ray
{
	////////////
	// Camera //
	////////////

	std::istream &operator >> ( std::istream &stream , Camera& camera )
	{
		stream >> camera.position >> camera.forward >> camera.up >> camera.heightAngle;
		if( !stream ) THROW( "Failed to parse camera" );
		camera.forward = camera.forward.unit();
		camera.up = camera.up.unit();
		camera.right = Point3D::CrossProduct( camera.forward , camera.up );
		return stream;
	}

	std::ostream &operator << ( std::ostream &stream , const Camera &camera )
	{
		return stream << "#camera  " << camera.position << "  " << camera.forward << "  " << camera.up << "  " << camera.heightAngle;
	}
}