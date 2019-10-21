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

	// Constructor

	Poly() {}

	Poly(Point* vertexList)
	{
		this->vertexList = vertexList;
	}

	// Destructor
	~Poly() {}

};

#endif