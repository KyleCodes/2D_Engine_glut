#ifndef POLYGON_H
#define POLYGON_H

#include <iostream>
#include <string>
#include <vector>
#include "Pointf.h"

using namespace std;

class Polygon
{
public:
	// Data
	// array of pointfs used to describe polygon
	vector<PointF> pointVect;

	// Constructor
	Polygon()
	{
	}

	Polygon(vector<PointF> pList)
	{
		pointVect = pList;
	}

	// Destructor
	~Polygon() {}

	// Functions
	void addPoint(PointF p)
	{
		pointVect.push_back(p);
	}

};


#endif