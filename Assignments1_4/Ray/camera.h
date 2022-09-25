#ifndef CAMERA_INCLUDED
#define CAMERA_INCLUDED
#include <stdio.h>
#include <Util/geometry.h>

namespace Ray
{
	/** This class stores information about the camera. */
	class Camera
	{
	public:
		/** The field of view of the camera (in radians) */
		double heightAngle;

		/** The position of the camera in world coordinates */
		Util::Point3D position;

		/** The direction that the camera is looking */
		Util::Point3D forward;

		/** The up direction of the camera */
		Util::Point3D up;

		/** The right direction of the camera */
		Util::Point3D right;

		/** This method calls the OpenGL commands for setting up the camera. */
		void drawOpenGL( void ) const;

		/** This call rotates the camera, about the axis which is parallel to the up direction of the camera, and passes through the specified point. */
		void rotateUp( Util::Point3D center , float angle );

		/** This call rotates the camera, about the axis which is parallel to the right direction of the camera, and passes through the specified point. */
		void rotateRight( Util::Point3D center , float angle );

		/** This call moves the camera in the forward direction by the specified distance.*/
		void moveForward( float dist );

		/** This call moves the camera in the right direction by the specified distance.*/
		void moveRight( float dist );

		/** This call moves the camera in the up direction by the specified distance.*/
		void moveUp( float dist );

		/** This function returns the ray that leaves the camera and goes through pixel (i,j) of the view plane */
		Util::Ray3D getRay( int i , int j , int width , int height ) const;
	};

	/** This operator writes the camera out to a stream. */
	std::ostream &operator << ( std::ostream &stream , const Camera &camera );

	/** This operator reads the camera in from a stream. */
	std::istream &operator >> ( std::istream &stream ,       Camera &camera );
}
#endif // CAMERA_INCLUDED
  

