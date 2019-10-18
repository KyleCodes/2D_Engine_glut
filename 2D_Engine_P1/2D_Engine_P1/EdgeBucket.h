#ifndef EDGEBUCKET_H
#define EDGEBUCKET_H
#include <iostream>
#include <string>


using namespace std;

class EdgeBucket
{
public:
	int yMax;
	int xOfyMin;
	float slopeInv;

	EdgeBucket() 
	{
		this->yMax = 0;
		this->xOfyMin = 0;
		this->slopeInv = 0.0;
	}

	EdgeBucket(int yMax, int xOfyMin, float slopeInv) {
		this->yMax = yMax;
		this->xOfyMin = xOfyMin;
		this->slopeInv = slopeInv;
	}

	~EdgeBucket() {}
};


#endif