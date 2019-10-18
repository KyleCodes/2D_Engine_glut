#ifndef EDGEBUCKET_H
#define EDGEBUCKET_H
#include <iostream>
#include <string>


using namespace std;

class EdgeInfo
{
public:
	int yMax;
	int xIntercept;
	float invSlope;

	EdgeInfo() 
	{
		this->yMax = 0;
		this->xIntercept = 0;
		this->invSlope = 0.0;
	}

	EdgeInfo(int yMax, int xOfyMin, float slopeInv) {
		this->yMax = yMax;
		this->xIntercept = xOfyMin;
		this->invSlope = slopeInv;
	}

	~EdgeInfo() {}
};


#endif