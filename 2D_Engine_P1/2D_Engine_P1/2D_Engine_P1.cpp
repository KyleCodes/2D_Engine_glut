/*
	TODO
	- add all cases for slope in lineBres
	- add user interactivity
	- read about other draw algs
	- design classes for cfg, picture processing
*/

////////////////////////////
// COLLECT CFG INFO 
////////////////////////////

// collect line draw info
// select shapes to load from file
// select clipping info
// select transformations for each shape
// package these options into a cfg object
// use fio to load polygon info into cfg object
// pass cfg object into drawing engine

#include <Windows.h>
#include <GL/glut.h> 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "Point.h"
#include "Pointf.h"
#include "EdgeTable.h"


using namespace std;

////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES -- all drawable objects will be stored here
////////////////////////////////////////////////////////////////////

	// Hold num of pixels in the grid,size of pixel
int gridWidth, gridHeight;
float pixelSize;

// Store window info
int winHeight, winWidth;

// hold points for line drawing 
Point START, END;
//Point pointArr[4] = { Point(5,5), Point(5,95), Point(95,95), Point(95,5) };
Point pointArr[5] = { Point(20,5), Point(5,55), Point(50,90), Point(95,55), Point(80,5) };
//Point pointArr[3] = { Point(5,5), Point(95,95), Point(95,5) };

EdgeTable edgeTable;
ScanlineEdges activeScanline;
////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES
////////////////////////////////////////////////////////////////////

// Window
void setupWindow();
void init();


// EDGE
void insertionSort(ScanlineEdges*);
void storeEdgeInScanlineArr(ScanlineEdges*, EdgeInfo);
void storeEdgeInTable(Point, Point);
void removeEdgeByYmax(ScanlineEdges*, int);
void updateXbySlopeInv(ScanlineEdges*);
void scanlineFill();

// LINE
void setPointsForLine(Point&, Point&);
void lineDDA();
void lineBres();
void lineBresN();
void polyLine();


/* OPENGL FUNCS */
void draw();
void idle();
void drawPix(Point);
void reshape(int width, int height);
void check();

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

	setupWindow();



	////////////////////////////
	// EDGES SETUP
	////////////////////////////
	edgeTable = EdgeTable(winHeight);
	activeScanline = ScanlineEdges();


	////////////////////////////
	// GLUT SETUP
	////////////////////////////

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("2D Graphics Engine");

	/*defined glut callback functions*/
	glutDisplayFunc(draw); //rendering calls here
	glutReshapeFunc(reshape); //update GL on window size change
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
// SETUP
////////////////////////////

void draw()
{
	//glClear(GL_COLOR_BUFFER_BIT);
	polyLine();
	scanlineFill();
	//glutPostRedisplay();
	//glutSwapBuffers();
	glFlush();
}


void setupWindow()
{
	gridWidth = 100;
	gridHeight = 100;

	// Assign dimension of single pixel
	pixelSize = 5.0;

	// Derive and assign Resolution
	winHeight = (int)(gridHeight * pixelSize);
	winWidth = (int)(gridWidth * pixelSize);
}

/* Set background color*/
void init()
{
	// Set default background color for resetting screen
	glClearColor(1.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, winHeight, 0, winWidth);
	// Check for errors
	//glClear(GL_COLOR_BUFFER_BIT);
	check();
}

////////////////////////////
// Edge ALGORITHMS
////////////////////////////

/* Sorts edges on current scanline by smallest y value of each edge*/
void insertionSort(ScanlineEdges* currLineEdges)
{
	EdgeInfo temp = EdgeInfo();
	int j;

	for (int i = 0; i < currLineEdges->EdgeNum; i++)
	{
		temp = currLineEdges->edges[i];
		j = i - 1;

		while ((temp.xIntercept < currLineEdges->edges[j].xIntercept) && (j >= 0))
		{
			currLineEdges->edges[j + 1] = currLineEdges->edges[j];
			j = j - 1;
		}
		currLineEdges->edges[j + 1] = temp;
	}
}

/* Stores information about an edge inside of an array of edges for a given scanline */
void storeEdgeInScanlineArr(ScanlineEdges* targetScanline, EdgeInfo src)
{
	targetScanline->edges[targetScanline->EdgeNum] = src;
	insertionSort(targetScanline);
	targetScanline->EdgeNum++;
}

/* Configures an EdgeInfo object to be stored in the table based on geometric props */
void storeEdgeInTable(Point p1, Point p2)
{
	EdgeInfo tempBucket = EdgeInfo();
	Point d = Point();
	d.x = p2.x - p1.x;
	d.y = p2.y - p1.y;
	int currLine;
	float slope;

	// If both points are on a colinear vertical line
	if (p2.x == p1.x)
	{
		tempBucket.invSlope = 0.0000000;
	}
	// Get the x increment amount, invSlope
	else
	{
		slope = (float)d.y / (float)d.x;


		if (p2.y == p1.y)
			return;

		tempBucket.invSlope = (float)1.0 / slope;
		//printf("\nSlope is %f", (slope));
	}
	//case for different symmetries
	if (p1.y > p2.y)
	{
		currLine = p2.y;
		tempBucket.yMax = p1.y;
		tempBucket.xIntercept = p2.x;
	}
	else
	{
		currLine = p1.y;
		tempBucket.yMax = p2.y;
		tempBucket.xIntercept = p1.x;
	}
	storeEdgeInScanlineArr(&edgeTable.table[currLine], tempBucket);
}

/* Removes an edge from the scanline in the table once that scanline has been drawn */
void removeEdgeByYmax(ScanlineEdges* currScanline, int yMax)
{
	for (int i = 0; i < currScanline->EdgeNum; i++)
	{
		if (currScanline->edges[i].yMax == yMax)
		{
			//printf("\nRemoved at %d", yMax);
			for (int j = i; j < currScanline->EdgeNum - 1; j++)
			{
				currScanline->edges[j] = currScanline->edges[j + 1];
			}
			currScanline->EdgeNum--;
			i--;
		}
	}
}

/* Move x bound of fill line according to the slope of the line */
void updateXbySlopeInv(ScanlineEdges* currScanline)
{
	for (int i = 0; i < currScanline->EdgeNum; i++)
	{
		currScanline->edges[i].xIntercept =
			currScanline->edges[i].xIntercept + currScanline->edges[i].invSlope;
	}
}

void scanlineFill()
{
	// 1. Do not draw fill lines on horizontal edge lines
	// 2. If scanline passes thru vertex and the edges are located on just one side of the line,
	//				then treat that vertex as 2 line crossings (AKA local max/min)
	// 3. If scanline passes thru vertex and the edges cross it, treat as one line crossing.

	// Draw from bottom of the screen to the top

	int coordCount = 0;
	int xL;
	int xR;
	bool fillFlag = 0;
	int yMax1 = 0;
	int yMax2 = 0;


	for (int i = 0; i < winHeight; i++)
	{
		// Move edge from edge table into the Active edge table
		for (int j = 0; j < edgeTable.table[i].EdgeNum; j++)
		{
			storeEdgeInScanlineArr(&activeScanline, edgeTable.table[i].edges[j]);
		}

		//printScanline(&activeScanline);

		// Remove edges who's y = ymax. This is a redundant draw.
		removeEdgeByYmax(&activeScanline, i);

		// Sort active edge table 
		insertionSort(&activeScanline);

		//printScanline(&activeScanline);

		// Generate fill line bounds for given scanline based on AET

		int j = 0;
		fillFlag = 0;
		coordCount = 0;
		xL = 0;
		xR = 0;
		yMax1 = 0;
		yMax2 = 0;

		// Implement-odd even rule by keeping track of remainer of numOfCoordinate bounds
		while (j < activeScanline.EdgeNum)
		{
			// left bound of a fill line
			if (coordCount % 2 == 0)
			{
				xL = (int)activeScanline.edges[j].xIntercept;
				yMax1 = activeScanline.edges[j].yMax;
				if (xL == xR)
				{
					if (((xL == yMax1) && (xR != yMax2)) || ((xL != yMax1) && (xR == yMax2)))
					{
						xR = xL;
						yMax2 = yMax1;
					}
					else
					{
						coordCount++;
					}
				}
				else
				{
					coordCount++;
				}
			}
			// right bound of a fill line
			else
			{
				xR = (int)activeScanline.edges[j].xIntercept;
				yMax2 = activeScanline.edges[j].yMax;
				fillFlag = 0;



				if (xL == xR)
				{
					if (((xL == yMax1) && (xR != yMax2)) || ((xL != yMax1) && (xR == yMax2)))
					{
						xL = xR;
						yMax1 = yMax2;
					}
					else
					{
						coordCount++;
						fillFlag = 1;
					}
				}
				else
				{
					coordCount++;
					fillFlag = 1;
				}


				if (fillFlag)
				{
					START = Point(xL, i);
					END = Point(xR, i);
					lineBresN();
					//glFlush();
				}
			}
			j++;
		}
		updateXbySlopeInv(&activeScanline);
	}


//	int x;
//	cin >> x;
}

////////////////////////////
// DRAWING ALGORITHMS
////////////////////////////

/* DDA Line Draw */
void lineDDA()
{
	Point curr = START;
	Point d = Point();

	d.x = END.x - START.x;
	d.y = END.y - START.y;

	double steps;
	float xInc, yInc;
	if (fabs(d.x) > fabs(d.y))
		steps = fabs(d.x);
	else
		steps = fabs(d.y);

	xInc = d.x / (float)steps;
	yInc = d.y / (float)steps;


	//clears the screen
	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	//clears the opengl Modelview transformation matrix
	//glLoadIdentity();
	drawPix(curr);

	for (int i = 0; i < steps; i++)
	{
		curr.x += xInc;
		curr.y += yInc;
		drawPix(curr);
	}

	//glutSwapBuffers();
	//check();
}

void lineBresN()
{
	int err;
	int incX, incY, inc1, inc2;
	Point d = Point();
	Point curr = Point();

	d.x = END.x - START.x;
	d.y = END.y - START.y;

	if (d.x < 0)
		d.x = -d.x;
	if (d.y < 0)
		d.y = -d.y;
	incX = 1;

	if (END.x < START.x)
		incX = -1;
	incY = 1;
	if (END.y < START.y)
		incY = -1;

	curr = START;


	if (d.x > d.y)
	{
		drawPix(curr);
		err = 2 * d.y - d.x;
		inc1 = 2 * (d.y - d.x);
		inc2 = 2 * d.y;

		for (int i = 0; i < d.x; i++)
		{
			if (err > 0)
			{
				curr.y += incY;
				err += inc1;
			}
			else
				err += inc2;
			curr.x += incX;
			drawPix(curr);
		}
	}
	else
	{
		drawPix(curr);
		err = 2 * d.x - d.y;
		inc1 = 2 * (d.x - d.y);
		inc2 = 2 * d.x;
		for (int i = 0; i < d.y; i++)
		{
			if (err >= 0)
			{
				curr.x += incX;
				err += inc1;
			}
			else
				err += inc2;
			curr.y += incY;
			drawPix(curr);
		}
	}
	//glutSwapBuffers();
}

/* Bresenham Line Draw */
void lineBres()
{
	Point curr = Point();
	Point d = Point();

	d.x = END.x - START.x;
	d.y = END.y - START.y;

	int p = 2 * d.y - d.x;
	int twoDy = 2 * d.y;
	int twoDyMinusDx = 2 * (d.y - d.x);


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

	//clears the screen
	glClear(GL_COLOR_BUFFER_BIT);
	//clears the opengl Modelview transformation matrix
	glLoadIdentity();
	drawPix(curr);
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
		drawPix(curr);
	}
	//glutSwapBuffers();
	//check();
}

void polyLine()
{
	int n = sizeof(pointArr) / sizeof(pointArr[0]);
	for (int i = 0; i < n; i++)
	{
		if (i != n - 1)
		{
			START = pointArr[i];
			END = pointArr[i + 1];
		}
		else {
			START = pointArr[i];
			END = pointArr[0];
		}
		lineBresN();
		//lineDDA();
		storeEdgeInTable(START, END);
	}
	//check();
}

////////////////////////////
// OPENGL ENGINE FUNCS
////////////////////////////


/* Define what happens when no work to be done */
void idle()
{
	// draw screen over and over
	glutPostRedisplay();
}

/* render the screen */

/* Draws single pixel given current grid size */
void drawPix(Point p)
{
	// Set Mode for drawing
	glBegin(GL_POINTS);

	// Set RGB color of the point
	glColor3f(1.0, 1.0, 1.0);

	// Specify vertex location
	glVertex3f(p.x + .5, p.y + .5, 0);
	glEnd();
	//glFlush();
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
	//check();
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

////////////////////////////
// MENU FUNCS
////////////////////////////
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