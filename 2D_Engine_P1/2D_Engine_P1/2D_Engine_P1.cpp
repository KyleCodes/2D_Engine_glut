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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <Windows.h>
#include <GL/glut.h> 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "EdgeTable.h"
#include "Polygon.h"


using namespace std;

////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES -- all drawable objects will be stored here
////////////////////////////////////////////////////////////////////

	// Holds file pointer
FILE* fpR;
FILE* fpW;

bool outputtedImage = false;

	// Hold num of pixels in the grid,size of pixel
int gridWidth, gridHeight;
float pixelSize;

// Store window info
int winHeight, winWidth;

// hold points for line drawing 
Point START, END;

int lineDrawSelection;

EdgeTable edgeTable;
ScanlineEdges activeScanline;

int numOfPolygons = 100;
Poly* polyList;

int selectedPolygon = 0;


////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES
////////////////////////////////////////////////////////////////////

// FILE
void readPolysFromTxt();
void writePolysToText();

// Window
void setupWindow();
void init();

// Translation
void translatePoly(int, int, int);
void scalePoly(int, float, float);
void rotatePoly(int, float);

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
void metaMenu();
void mainMenu();
void actionMenu();
void transformMenu();
void optionsMenu();

////////////////////////////////////////////////////////////////////
// START OF EXECUTION
////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	////////////////////////////
	// FILE SETUP
	////////////////////////////

	int menuSelect = 0;
	
	metaMenu();
	cin >> menuSelect;
	if (menuSelect == 1)
	{
		fpR = fopen("polyInfo.txt", "r");
		if (fpR == NULL)
		{
			cout << "Error opening file" << endl;
			return 1;
		}
		readPolysFromTxt();
		fclose(fpR);
	}
	else if (menuSelect == 2)
	{
		fpR = fopen("working.txt", "r");
		if (fpR == NULL)
		{
			cout << "Error opening file" << endl;
			return 1;
		}
		readPolysFromTxt();
		fclose(fpR);
	}
	
	mainMenu();
	cin >> menuSelect;
	lineDrawSelection = menuSelect;

	// Get transformation info
	actionMenu();
	cin >> menuSelect;

	if (menuSelect == 1)
	{
		// transformation menu
		transformMenu();
		cin >> menuSelect;
		selectedPolygon = menuSelect;
	}

	if (selectedPolygon != 0)
	{
		optionsMenu();
		cin >> menuSelect;
		switch (menuSelect)
		{
		case 1:
			// translate;
			int tx, ty;

			cout << endl;
			cout << "Enter quantities to translate Polygon:" << endl;
			cout << endl;
			cout << "\tX-axis: ";
			cin >> tx;
			cout << "\n\tY-axis: ";
			cin >> ty;
			translatePoly(selectedPolygon - 1, tx, ty);
			break;

		case 2:
			// scale
			float sx;
			float sy;
			cout << "Enter scale factor for polygon:" << endl;
			cout << endl;
			cout << "\tScale-factor-X: ";
			cin >> sx;
			cout << "\n\tScale-factor-Y: ";
			cin >> sy;

			scalePoly(selectedPolygon - 1, sx, sy);
			break;

		case 3:
			// rotate
			float theta;
			cout << "Enter theta to rotate polygon" << endl;
			cout << endl;
			cout << "\tTheta: ";
			cin >> theta;

			rotatePoly(selectedPolygon - 1, theta);

			break;

		}
	}


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
	return 0;
}


////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS
////////////////////////////////////////////////////////////////////


////////////////////////////
// SETUP
////////////////////////////

void draw()
{
	glColor3f(1.0, 1.0, 1.0);

	polyLine();
	if (outputtedImage == false)
	{
		fpW = fopen("working.txt", "w");
		if (fpW == NULL)
		{
			cout << "Error opening file" << endl;
			return 1;
		}
		writePolysToText();
		outputtedImage = true;
		fclose(fpW);
	}
	scanlineFill();
	glutSwapBuffers();
}


void readPolysFromTxt()
{ 
	// Control vars
	const int maxSize = 256;
	int currLine = 0;
	bool readingPoly = false;
	Point currPoint = Point();
	char buf[maxSize];



	// Polygon stuff
	char ID[maxSize];
	int numOfVerticies = 0;
	Point vertexList[maxSize];
	int currVertex = 0;
	int currPolygon = 0;
	Poly polyArr[maxSize];
	char newBuf[maxSize];


	// Read txt file to initialize list of polygons
	while (currPolygon <= numOfPolygons)
	{
		// Gets number of polygons to be read
		if (currLine == 0)
		{
			fscanf(fpR, "%d", &numOfPolygons);
			//cout << "num of polygons = " << numOfPolygons << endl;
		}
		// Gets string ID of polygon to be read
		else if (currLine == 1)
		{
			currPolygon++;
			fscanf(fpR, "%s", &ID);
			currVertex = 0;
			//cout << "currPolygon = " << currPolygon << endl;
			//cout << "ID = " << ID << endl;
		}
		// Get the number of verticies of the polygon, initialize vertex list for it
		else if (currLine == 2)
		{
			fscanf(fpR, "%d", &numOfVerticies);
			//vertexList = new Point[numOfVerticies];
			for (int i = 0; i < numOfVerticies; i++)
			{
				vertexList[i] = Point();
			}
			//cout << "numOfVerticies = " << numOfVerticies << endl;
		}
		// get a point of the polygon
		else if (currLine == 3)
		{
			char t1[maxSize];
			char t2[maxSize];

			fscanf(fpR, "%s %s", &t1, &t2);
			sscanf(t1, "%d", &currPoint.x);
			sscanf(t2, "%d", &currPoint.y);
			vertexList[currVertex] = currPoint;
			currVertex++;
			currLine--;
			//cout << "currVertex = " << currVertex << endl;

			if (currVertex == numOfVerticies)
				currLine++;
		}
		else if(currLine == 4)
		{
			// polygon finished reading, store vertex list, add to array, and move to next line
			// initialize polygon
			// add to array index currPolygon - 1 (conventional)
			fgets(buf, maxSize, fpR);
			currLine = 0;



			polyArr[currPolygon - 1] = Poly(ID, vertexList, numOfVerticies);
		}

		currLine++;
	}
	polyList = new Poly[numOfPolygons];
	for (int i = 0; i < numOfPolygons; i++)
	{
		polyList[i] = polyArr[i];
	}
}


void writePolysToText()
{
	int currPolygon = 0;

	while (currPolygon < numOfPolygons)
	{
		fprintf(fpW, "%d\n", numOfPolygons);

		for (int i = 0; i < numOfPolygons; i++)
		{
			// print the id
			fprintf(fpW, "%s\n", polyList[i].ID);

			// print number of verts
			fprintf(fpW, "%d\n", polyList[i].numOfVerticies);

			// inner loop to print points
			for (int j = 0; j < polyList[i].numOfVerticies; j++)
			{
				//char t1[256];
				//char t2[256];
				
				//strcpy(t1, polyList[i].vertexList[j].x);

				fprintf(fpW, "%d %d\n", polyList[i].vertexList[j].x, polyList[i].vertexList[j].y);
			}
			fprintf(fpW, "\n");
			currPolygon++;
		}

	}
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
// TRANSFORMATION ALGORITHMS
////////////////////////////
void translatePoly(int polyNum, int tx, int ty)
{
	for (int i = 0; i < polyList[polyNum].numOfVerticies; i++)
	{
		polyList[polyNum].vertexList[i].x += tx;
		polyList[polyNum].vertexList[i].y += ty;
	}
}

void scalePoly(int polyNum, float sx, float sy)
{
	int centX;
	int centY;
	int totX = 0;
	int totY = 0;

	for (int i = 0; i < polyList[polyNum].numOfVerticies; i++)
	{
		totX += polyList[polyNum].vertexList[i].x;
		totY += polyList[polyNum].vertexList[i].y;
	}

	centX = (int)totX/ polyList[polyNum].numOfVerticies;
	centY = (int)totY / polyList[polyNum].numOfVerticies;

	for (int i = 0; i < polyList[polyNum].numOfVerticies; i++)
	{
		polyList[polyNum].vertexList[i].x = (polyList[polyNum].vertexList[i].x * sy) + (centX * (1 - sx));
		polyList[polyNum].vertexList[i].y = (polyList[polyNum].vertexList[i].y * sy) + (centY * (1 - sy));
	}
}

void rotatePoly(int polyNum, float theta)
{
	int centX;
	int centY;
	int totX = 0;
	int totY = 0;

	for (int i = 0; i < polyList[polyNum].numOfVerticies; i++)
	{
		totX += polyList[polyNum].vertexList[i].x;
		totY += polyList[polyNum].vertexList[i].y;
	}

	centX = (int)totX / polyList[polyNum].numOfVerticies;
	centY = (int)totY / polyList[polyNum].numOfVerticies;

	for (int i = 0; i < polyList[polyNum].numOfVerticies; i++)
	{
		polyList[polyNum].vertexList[i].x = centX + ((polyList[polyNum].vertexList[i].x - centX) * cos(theta)) 
			- ((polyList[polyNum].vertexList[i].y - centY) * sin(theta));

		polyList[polyNum].vertexList[i].y = centY + ((polyList[polyNum].vertexList[i].x - centX) * sin(theta))
			+ ((polyList[polyNum].vertexList[i].y - centY) * cos(theta));
	}

}

////////////////////////////
// Edge ALGORITHMS
////////////////////////////

/* Sorts edges on current scanline by smallest y value of each edge*/
void insertionSort(ScanlineEdges* currLineEdges)
{
	EdgeInfo temp = EdgeInfo();
	int j;

	for (int i = 1; i < currLineEdges->EdgeNum; i++)
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
	// redesign for polyogn array

	int j;
	for (int i = 0; i < numOfPolygons; i++)
	{
		j = 0;
		for (j = 0; j < polyList[i].numOfVerticies; j++)
		{
			if ( j != polyList[i].numOfVerticies - 1)
			{
				START = polyList[i].vertexList[j];
				END = polyList[i].vertexList[j + 1];
			}
			else {
				START = polyList[i].vertexList[j];
				END = polyList[i].vertexList[0];
			}

			if (lineDrawSelection == 1)
				lineDDA;
			if (lineDrawSelection == 2)
				lineBresN();
			storeEdgeInTable(START, END);
		}
	}
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
		return 1;
	}
}

////////////////////////////
// MENU FUNCS
////////////////////////////
void metaMenu()
{
	cout << "//////////////////////////////////" << endl;
	cout << "         CONFIG MENU     " << endl;
	cout << "//////////////////////////////////" << endl;
	cout << endl;
	cout << "Pick whether to start with original or modified polgons" << endl;
	cout << "\nWARNING: cannot access modified polygons until after first run" << endl;
	cout << endl;
	cout << "\t1) Load original Polygons" << endl;
	cout << "\t2) Load modified Polygons" << endl;
	cout << "\t";
}

void mainMenu()
{
	cout << "//////////////////////////////////" << endl;
	cout << "  Successfully Loaded Polygons     " << endl;
	cout << "//////////////////////////////////" << endl;
	cout << endl;
	cout << "Avaliable Entities:" << endl << endl;
	for (int i = 0; i < numOfPolygons; i++)
	{
		cout << "\t-" << polyList[i].ID << endl;
	}
	cout << endl << endl;
	cout << "SELECT LINE DRAW ALGORITHM" << endl << endl;
	cout << "\t1) DDA Line Draw" << endl;
	cout << "\t2) Bresenham Line Draw" << endl;
	cout << endl;
	cout << "\t";

}

void actionMenu()
{
	cout << "//////////////////////////////////" << endl;
	cout << "          ACTION MENU     " << endl;
	cout << "//////////////////////////////////" << endl;
	cout << endl;
	cout << "\t1) Apply Transformations to a Polygon" << endl;
	cout << "\t2) Draw Polygons" << endl;
	cout << endl;
	cout << "\t";
}

void transformMenu()
{
	cout << "//////////////////////////////////" << endl;
	cout << "       TRANSFORMATION MENU     " << endl;
	cout << "//////////////////////////////////" << endl;
	cout << endl;
	cout << "Select a Polygon to modify:" << endl;
	cout << endl;
	for (int i = 0; i < numOfPolygons; i++)
	{
		cout << "\t" << (i + 1) << ") " << polyList[i].ID << endl;
	}
	cout << endl;
	cout << "\t";
}

void optionsMenu()
{
	cout << "//////////////////////////////////" << endl;
	cout << "       OPTIONS MENU     " << endl;
	cout << "//////////////////////////////////" << endl;
	cout << endl;
	cout << "Select a transformation to apply to: " << polyList[selectedPolygon].ID << endl;
	cout << endl;
	cout << "\t1) Translate" << endl;
	cout << "\t2) Scale" << endl;
	cout << "\t3) Rotate" << endl;
}