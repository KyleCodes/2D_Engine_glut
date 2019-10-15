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
void draw_pix(int x, int y);
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
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	/*initialize variables, allocate memory, create buffers, etc. */
	//create window of size (win_width x win_height
	glutInitWindowSize(winWidth, winHeight);
	//windown title is "glut demo"
	glutCreateWindow("@D Graphics Engine");

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