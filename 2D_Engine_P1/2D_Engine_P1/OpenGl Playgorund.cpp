#include <Windows.h>
#include <GL/glut.h> 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "Point.h"
#include "Pointf.h"

using namespace std;


Point START, END;

int roundFloat(const float a)
{
	return int(a + 0.5);
}


void init()
{
	// Set display color to white
	// Set projection parameters
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 500.0, 0.0, 500.0);
}

void drawPixel(int x, int y) {
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

void lineDDA(void)
{
	Point curr = START;
	Point d = Point();

	d.x = END.x - START.x;
	d.y = END.y - START.y;

	double steps;
	float xInc, yInc;
	/* Find out whether to increment x or y */
	steps = (fabs(d.x) > fabs(d.y)) ? (fabs(d.x)) : (fabs(d.y));
	xInc = d.x / (float)steps;
	yInc = d.y / (float)steps;

	/* Clears buffers to preset values */
	glClear(GL_COLOR_BUFFER_BIT);

	/* Plot the points */
	glBegin(GL_POINTS);
	/* Plot the first point */
	glVertex2d(curr.x, curr.y);
	int k;
	/* For every step, find an intermediate vertex */
	for (k = 0; k < steps; k++)
	{
		curr.x += xInc;
		curr.y += yInc;
		/* printf("%0.6lf %0.6lf\n",floor(x), floor(y)); */
		glVertex2d(curr.x, curr.y);
	}
	glEnd();

	glFlush();
}

void lineBresenham(void) // only works for m <= 1 && m > 0
						 // interchange roles of x-y values for other symmetries
{
	Point curr = Point();
	Point d = Point(fabs(END.x - START.x), fabs(END.y - START.y));
	int p = 2 * d.y - d.x;
	int twoDy = 2 * d.y;
	INT twoDyMinusDx = 2 * (d.y - d.x);

	if (START.x > END.x)
	{
		curr.x = END.x;
		curr.y = END.y;
		END = START;
	}
	else
	{
		curr.x = START.x;
		curr.y = START.y;
	}

	glClear(GL_COLOR_BUFFER_BIT);

	drawPixel(curr.x, curr.y);
	while (curr.x < END.x)
	{
		curr.x++;
		if (p < 0)
			p += twoDy;
		else
		{
			curr.y++;
			p += twoDyMinusDx;
		}
		drawPixel(curr.x, curr.y);
	}
	glFlush();
}

GLenum errorCheck()
{
	GLenum code;
	const GLubyte* string;
	code = glGetError();
	if (code != GL_NO_ERROR)
	{
		string = gluErrorString(code);
		fprintf(stderr, "OpenGL error: %s\n", string);
	}
	return code;
}

void printMenu()
{
	cout << "/////////////////////////////" << endl;
	cout << "     Pick Draw Algorithm     " << endl;
	cout << endl;
	cout << "    1) Bresenham Line Draw   " << endl;
	cout << "    2) DDA Line Draw         " << endl;
	cout << "    3) ..                    " << endl;
	cout << endl;
	cout << "    4) Quit                  " << endl;
	cout << endl;
	cout << "\t";
}

void printDebug(Point start, Point end)
{
	//int size = sizeof(arr) / sizeof(arr[0]);
	cout << "start point: x=" << start.x << " y=" << start.y << endl;
	cout << "end point  : x=" << end.x << " y=" << end.y << endl;
	cout << endl;
}

void main(int argc, char** argv)
{
	// Set Global Coordinates for line drawing algorithms
	START = Point(5, 495);
	END = Point(495, 5);

	////////////////////////
	//	OPENGL INITIALIZATION
	////////////////////////

	// Initialise GLUT library
	// Set the initial display mode
	// Set the initial window position and size 
	// Create the window with title "DDA_Line"
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(1000, 100);
	glutInitWindowSize(500, 500);
	glutCreateWindow("2D_Engine");
	init();

	/////////////////////////////////////////////////////////
	//				PROGRAM LOOP
	/////////////////////////////////////////////////////////

	int menuSelect = 0;

	while (menuSelect != 4)
	{
		menuSelect = 0;
		system("cls");
		printDebug(START, END);
		printMenu();
		cin >> menuSelect;

		if (menuSelect < 0 || menuSelect > 4)
			continue;

		if (menuSelect == 4)
			break;

		switch (menuSelect)
		{
		case 0:
			// ENTER COORDINATES?
			break;

		case 1:
			// BRESENHAM
			glutDisplayFunc(lineBresenham);
			break;

		case 2:
			// DDA
			glutDisplayFunc(lineDDA);
			break;

		case 3:
			// ...
			break;

		default:
			break;
		}

		////////////////////////
		//	OPENGL DRAWING
		////////////////////////

					/* DRAW SCENE */

		// Initialize drawing colors
		// Call the displaying function
		// Keep displaying untill the program is closed
		glutMainLoop();

	}



}