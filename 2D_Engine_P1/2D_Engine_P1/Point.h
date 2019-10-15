#ifndef POINT_H
#define POINT_H
#include <iostream>
#include <string>
using namespace std;

class Point
{
public:
	// Data
	int x, y;

	// Constructor
	Point()
	{
		x = -1;
		y = -1;
	}

	Point(int xN, int yN)
	{
		x = xN;
		y = yN;
	}

	~Point() {}
};
#endif