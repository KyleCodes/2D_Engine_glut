#include <Windows.h>
#include <GL/glut.h> 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "Point.h"
#include "Pointf.h"

using namespace std;

////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
////////////////////////////////////////////////////////////////////

	// Hold num of pixels in the grid,size of pixel
int gridWidth, gridHeight;
float pixelSize;

	// Store window info
int winHeight, winWidth;

	// hold points for line drawing 
Point start, end;

////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES
////////////////////////////////////////////////////////////////////

	/* OPENGL FUNCS */
void init();
void idle();
void display();
void drawPix(Point);
void reshape(int width, int height);
void key(unsigned char ch, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void check();

	/* My Funcs */
int roundFloat(const float);
void lineDDA();
void lineBresenham();
GLenum errorCheck();

	/* Menu Funcs */
void printMenu();


////////////////////////////////////////////////////////////////////
// START OF EXECUTION
////////////////////////////////////////////////////////////////////
void main(int argc, char** argv)
{
	////////////////////////////
	// WINDOW SETUP
	////////////////////////////

		// Assign number of grid elements
	gridWidth = 100;
	gridHeight = 100;

		// Assign dimension of single pixel
	pixelSize = 5.0;

		// Derive and assign Resolution
	winHeight = (int)(gridHeight * pixelSize);
	winWidth = (int)(gridWidth * pixelSize);

	////////////////////////////
	// GLUT SETUP
	////////////////////////////

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	/*initialize variables, allocate memory, create buffers, etc. */
	//create window of size (win_width x win_height
	glutInitWindowSize(winWidth, winHeight);
	//windown title is "glut demo"
	glutCreateWindow("2D Graphics Engine");

	/*defined glut callback functions*/
	glutDisplayFunc(display); //rendering calls here
	glutReshapeFunc(reshape); //update GL on window size change
	glutMouseFunc(mouse);     //mouse button events
	glutMotionFunc(motion);   //mouse movement events
	glutKeyboardFunc(key);    //Keyboard events
	glutIdleFunc(idle);       //Function called while program is sitting "idle"

	//initialize opengl variables
	init();
	//start glut event loop
	glutMainLoop();
}


////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS
////////////////////////////////////////////////////////////////////

////////////////////////////
// OPENGL ENGINE FUNCS
////////////////////////////
	
/* Set background color / projection mode */
void init()
{
	// Set default background color for resetting screen
	glClearColor(1.0, 1.0, 1.0, 0.0);
	
	// Check for errors
	check();
}

/* Define what happens when no work to be done */
void idle()
{
	// draw screen over and over
	glutPostRedisplay();
}

/* render the screen */
void display()
{
	//clears the screen
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	//clears the opengl Modelview transformation matrix
	glLoadIdentity();

	//draws every other pixel on the screen
	Point curr;
	for (int j = 0; j < gridHeight; j += 2) {
		for (int i = 0; i < gridWidth; i += 2) {
			curr = Point(i, j);
			drawPix(curr);
		}
	}

	//blits the current opengl framebuffer on the screen
	glutSwapBuffers();
	//checks for opengl errors
	check();
}

/* Draws single pixel given current grid size */
void drawPix(Point p)
{
	// Set Mode for drawing
	glBegin(GL_POINTS);

	// Set RGB color of the point
	glColor3f(.2, .2, 1.0);
	
	// Specify vertex location
	glVertex3f(p.x + .5, p.y + .5, 0);
	glEnd;

}

/* Is called when display size changes, including initial creation of the display */
void reshape(int width, int height)
{
	/* Set up projection matrix to define view port */
	// update global window width and height
	winWidth = width;
	winHeight = height;

	// Create rendering area across the window
	glViewport(0, 0, width, height);

	// Designate orthogonal matrix to map pixel space to grid space
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, gridWidth, 0, gridHeight);

	//clear the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// set pixel size based on width
	pixelSize = width / (float)gridWidth;

	// set pixel size relative to the grid cell size
	glPointSize(pixelSize);

	// check for errors
	check();
}

/* Gets current keyboard character being pressed */
void key(unsigned char ch, int x, int y)
{
	switch (ch)
	{
	default:
		//prints out which key the user hit
		printf("User hit the \"%c\" key\n", ch);
		break;
	}
	//redraw the scene after keyboard input
	glutPostRedisplay();
}

/* Get the states of the mouse buttons and cursor */
void mouse(int button, int state, int x, int y)
{
	//print the pixel location, and the grid location
	printf("MOUSE AT PIXEL: %d %d, GRID: %d %d\n", x, y, (int)(x / pixelSize), (int)((winHeight - y) / pixelSize));
	switch (button)
	{
	case GLUT_LEFT_BUTTON: //left button
		printf("LEFT ");
		break;
	case GLUT_RIGHT_BUTTON: //right button
		printf("RIGHT ");
	default:
		printf("UNKNOWN "); //any other mouse button
		break;
	}
	if (state != GLUT_DOWN)  //button released
		printf("BUTTON UP\n");
	else
		printf("BUTTON DOWN\n");  //button clicked

	//redraw the scene after mouse click
	glutPostRedisplay();
}

/* Get alerted when cursor moves acorss the screen */
//gets called when the curser moves accross the scene
void motion(int x, int y)
{
	//redraw the scene after mouse movement
	glutPostRedisplay();
}

/* Check for errors in previous calls and display them */
void check()
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("GLERROR: There was an error %s\n", gluErrorString(err));
		exit(1);
	}
}