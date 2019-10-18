#ifndef EDGEBUCKETHOLDER_H
#define EDGEBUCKETHOLDER_H
#include <iostream>
#include <string>
#include "EdgeInfo.h"

using namespace std;

class ScanlineEdges
{
public:
	int BucketCount;
	EdgeInfo *buckets;

	ScanlineEdges() 
	{
		this->BucketCount = 0;
		buckets = NULL;
	}

	ScanlineEdges(int numOfBuckets)
	{
		this->BucketCount = numOfBuckets;
		buckets = new EdgeInfo[this->BucketCount];
		for (int i = 0; i < this->BucketCount; i++)
		{
			buckets[i] = EdgeInfo();
		}
	}

	~ScanlineEdges() {}
};


#endif
