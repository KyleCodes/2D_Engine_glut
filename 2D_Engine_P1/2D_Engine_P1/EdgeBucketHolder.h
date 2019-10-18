#ifndef EDGEBUCKETHOLDER_H
#define EDGEBUCKETHOLDER_H
#include <iostream>
#include <string>
#include "EdgeBucket.h"

using namespace std;

class EdgeBucketHolder
{
public:
	int numOfBuckets;
	EdgeBucket *buckets;

	EdgeBucketHolder() 
	{
		this->numOfBuckets = 0;
		buckets = NULL;
	}

	EdgeBucketHolder(int numOfBuckets)
	{
		this->numOfBuckets = numOfBuckets;
		buckets = new EdgeBucket[this->numOfBuckets];
		for (int i = 0; i < this->numOfBuckets; i++)
		{
			buckets[i] = EdgeBucket();
		}
	}

	~EdgeBucketHolder() {}
};


#endif
