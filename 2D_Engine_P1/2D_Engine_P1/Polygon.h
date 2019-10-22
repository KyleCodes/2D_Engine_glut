#ifndef POLY_H
#define POLY_H

#include <iostream>
#include <string>
#include <vector>
#include "Point.h"

//using namespace std;

class Poly
{
public:
	Point* vertexList;
	int numOfVerticies;

	// Constructor

	Poly() {}

	Poly(Point* vertexBuf, int numOfVerticies)
	{
		this->numOfVerticies = numOfVerticies;
		vertexList = new Point[this->numOfVerticies];
		for (int i = 0; i < this->numOfVerticies; i++)
		{
			vertexList[i] = vertexBuf[i];
		}
	}

	// Destructor
	~Poly() {}

};

#endif