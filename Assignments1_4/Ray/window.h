#ifndef RAY_WINDOW_INCLUDED
#define RAY_WINDOW_INCLUDED

#include <Util/timer.h>
#include <Ray/mouse.h>
#include <Ray/scene.h>

namespace Ray
{
	/** This class represents the OpenGL window within which the 3D models are drawn. */
	class Window
	{
		/** Main menu selection options */
		enum
		{
			QUIT ,
			RAY_TRACE ,
			WRITE_SCENE
		};

		/** The dimensions of the window */
		static int _width , _height;

	public:
		/** The ascii value of the escape character */
		const static char KEY_ESCAPE;

		/** Culling selection options */
		enum
		{
			// No culling
			NO_CULL,
			// Back-face culling
			CULL_BACK_FACE,
			// Front-face culling
			CULL_FRONT_FACE
		};

		/** A pointer to the scene that is drawn.*/
		static Scene* scene;

		/** The current state of the mouse */
		static Mouse mouse;

		/** The center of the bounding box of the drawn Scene */
		static Util::Point3D center;

		/** The radius of the bounding box of the drawn Scene */
		static double radius;

		/** Is the window visible */
		static bool isVisible;

		/** The number of frames that have been drawn since the last updated frame rate */
		static int frameCount;

		/** The time of the last updated frame rate */
		static Util::Timer frameCountTimer;

		/** The current frame rate */
		static double frameRate;

		/** A timer for tracking the frame type */
		static Util::Timer timer;

		/** The type of interpolation to be used for animation */
		static int interpolationType;

		/** The type of parametrization to be used for animation */
		static int parametrizationType;

		/** This function prints out the state of the OpenGL error. */
		static int PrintError( int showNoError=0 );

		/** This function writes out the specified string, left-aligned, at the specified location, onto the OpenGL window. */
		static void WriteLeftString( int x , int y , const std::string &str );

		/** This function writes out the specified string, right-aligned, at the specified location, onto the OpenGL window. */
		static void WriteRightString( int x , int y , const std::string &str );	

		/** This function reads the current frame buffer and sets the pixels of the image accordingly. */
		static Image::Image32 TakeSnapshot( void );

		/** This function is called when no events need to be processed. */
		static void IdleFunction( void );

		/** This function is called when the visibility state of the window changes. */
		static void VisibilityFunction( int state );

		/** This function (re)sets the evaluators */
		static void SetEvaluators( void );

		//////////////////////////
		// Mouse event handlers //
		//////////////////////////
		/** This function is called when the state of a mouse button is changed */
		static void MouseFunction( int button, int state, int x, int y );

		/** This function is called when one of the mouse buttons is depressed and the mouse is moved. */
		static void MotionFunction( int x, int y );

		/** This function is called when the mouse is moved moved but no buttons are depressed. */
		static void PassiveMotionFunction( int x, int y );

		/////////////////////////////
		// Keyboard event handlers //
		/////////////////////////////
		/** This function is called when the user presses a key. */
		static void KeyboardFunction( unsigned char c, int x, int y );

		/** This function is called when the user presses one of the special keys. */
		static void SpecialFunction( int key , int x , int y );

		/////////////////////////
		// Menu event handlers //
		/////////////////////////
		/** This function is called when the user updates the draw mode in the drop-down menu. */
		static void DrawModeMenu( int entry );

		/** This function is called when the user updates the cull mode in the drop-down menu. */
		static void CullModeMenu( int entry );

		/** This function is called when the user updates the curve fitting method in the drop-down menu. */
		static void InterpolationTypeMenu( int entry );

		/** This function is called when the user updates the rotation parametrization method in the drop-down menu. */
		static void ParametrizationTypeMenu( int entry );

		/** This function is called when the user selects one of the main menu options in the drop-down menu. */
		static void MainMenu( int entry );

		/**  This function draws the OpenGL window. */
		static void DisplayFunction( void );

		/** This function is called when the window is resized. */
		static void ReshapeFunction( int width , int height );

		/** This function instantiates the OpenGL window, reading in the Scene from the specified file and setting the initial OpenGL window size.
		*** The function never returns. */
		static void View( Scene& scene , int width , int height );
	};
}
#endif // RAY_WINDOW_INCLUDED