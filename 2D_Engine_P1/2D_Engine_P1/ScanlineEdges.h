#ifndef EDGEBUCKETHOLDER_H
#define EDGEBUCKETHOLDER_H
#include <iostream>
#include <string>
#include "EdgeInfo.h"

using namespace std;

class ScanlineEdges
{
public:
	int EdgeNum;
	int numOfEdges;
	EdgeInfo *edges;

	ScanlineEdges() 
	{
		this->EdgeNum = 0;
		edges = NULL;
	}

	ScanlineEdges(int numOfBuckets)
	{
		this->EdgeNum = numOfBuckets;
		edges = new EdgeInfo[this->EdgeNum];
		for (int i = 0; i < this->EdgeNum; i++)
		{
			edges[i] = EdgeInfo();
		}
	}

	~ScanlineEdges() {}
};


#endif
