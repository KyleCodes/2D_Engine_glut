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
	int maxNumOfEdges = 100;
	EdgeInfo *edges;

	ScanlineEdges() 
	{
		this->EdgeNum = 0;
		edges = new EdgeInfo[this->maxNumOfEdges];
		for (int i = 0; i < this->maxNumOfEdges; i++)
			edges[i] = EdgeInfo();
	}


	~ScanlineEdges() {}
};


#endif
