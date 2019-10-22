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
	char ID[256];
	Point* vertexList;
	int numOfVerticies;

	// Constructor

	Poly() {}

	Poly(char* IDold, Point* vertexBuf, int numOfVerticies)
	{
		strcpy(this->ID, IDold);
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