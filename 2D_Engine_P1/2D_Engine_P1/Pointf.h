#ifndef POINTF_H
#define POINTF_H
#include <iostream>
#include <string>
using namespace std;

class PointF
{
public:
	// Data
	float x, y;

	// Constructor
	PointF()
	{
		x = -1.0;
		y = -1.0;
	}

	PointF(float xN, float yN)
	{
		x = xN;
		y = yN;
	}

	~PointF() {}
};
#endif