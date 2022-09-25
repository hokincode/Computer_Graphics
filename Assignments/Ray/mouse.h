#ifndef MOUSE_INCLUDED
#define MOUSE_INCLUDED

#include "Util/geometry.h"

namespace Ray
{
	/** This class represents the current state of the mouse */
	class Mouse
	{
	public:
		/** Was the shift button depressed when the mouse state was updated */
		bool shiftDown;

		/** Was the control button depressed when the mouse state was updated */
		bool ctrlDown;

		/** Was the alt button depressed when the mouse state was updated */
		bool altDown;

		/** Was the left mouse button depressed when the mouse state was updated */
		bool leftDown;

		/** Was the middle mouse button depressed when the mouse state was updated */
		bool middleDown;

		/** Was the right mouse button depressed when the mouse state was updated */
		bool rightDown;

		/** Was the scroll wheel depressed when the mouse state was updated */
		bool scrollDown;

		/** The screen coordinates when the mouse state was updated */
		int startX , startY;

		/** The screen coordinates when the mouse was last moved */
		int endX , endY;

		/** This constructor instantiates the mouse members */
		Mouse( void );

		/** This method updates the state of the mouse when the state of a mouse button is changed */
		void update( int button , int state , int x , int y );

		/** This method udpates the state of the mouse when the mouse is moved.
		*** The returned value indicates the distance (in screen coordinates) that the mouse has moved */
		Util::Point2D move( int x , int y );
	};
}
#endif // MOUSE_INCLUDED