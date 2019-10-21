#ifndef POLYGON_H
#define POLYGON_H

#include <iostream>
#include <string>
#include <vector>
#include "Point.h"

using namespace std;

class Polygon
{
public:
	Point* vertexList;

	// Constructor
	Polygon() {}

	Polygon(Point *vertexList)
	{
		this->vertexList = vertexList;
	}


	// Destructor
	~Polygon() {}

	// Functions

};


#endif