/*

*/

#include <GL/glut.h> // (or others, depending on the system in use)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double X1, Y1, X2, Y2;

float round_value(float v)
{
	return floor(v + 0.5);
}

void LineDDA(void)
{
	double dx = (X2 - X1);
	double dy = (Y2 - Y1);
	double steps;
	float xInc, yInc, x = X1, y = Y1;
	/* Find out whether to increment x or y */
	steps = (fabs(dx) > fabs(dy)) ? (fabs(dx)) : (fabs(dy));
	xInc = dx / (float)steps;
	yInc = dy / (float)steps;

	/* Clears buffers to preset values */
	glClear(GL_COLOR_BUFFER_BIT);

	/* Plot the points */
	glBegin(GL_POINTS);
	/* Plot the first point */
	glVertex2d(x, y);
	int k;
	/* For every step, find an intermediate vertex */
	for (k = 0; k < steps; k++)
	{
		x += xInc;
		y += yInc;
		/* printf("%0.6lf %0.6lf\n",floor(x), floor(y)); */
		glVertex2d(round_value(x), round_value(y));
	}
	glEnd();

	glFlush();
}

void Init()
{
	/* Set clear color to white */
	glClearColor(1.0, 1.0, 1.0, 0);
	/* Set fill color to black */
	glColor3f(0.0, 0.0, 0.0);
	/* glViewport(0 , 0 , 640 , 480); */
	/* glMatrixMode(GL_PROJECTION); */
	/* glLoadIdentity(); */
	gluOrtho2D(0, 640, 0, 480);
}

void main(int argc, char** argv)
{
	X1 = 50;
	X2 = 300;
	Y1 = 50;
	Y2 = 300;

	/* Initialise GLUT library */
	glutInit(&argc, argv);
	/* Set the initial display mode */
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	/* Set the initial window position and size */
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(640, 480);
	/* Create the window with title "DDA_Line" */
	glutCreateWindow("DDA_Line");
	/* Initialize drawing colors */
	Init();
	/* Call the displaying function */
	glutDisplayFunc(LineDDA);
	/* Keep displaying untill the program is closed */
	glutMainLoop();
}

int round(const float a)
{
	return int(a + 0.5);
}

void drawPixel(int x, int y)
{
	glBegin(GL_POINT);
	glColor3f(0.0, 0.4, 0.2); // Set line segment color to green.
	glVertex2i(x, y);
	glEnd();
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

