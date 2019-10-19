#ifndef EDGETABLE_H
#define EDGETABLE_H
#include <iostream>
#include <string>
#include "ScanlineEdges.h"

using namespace std;

class EdgeTable
{
public:
	int maxHeight;
	ScanlineEdges* table;

	EdgeTable() 
	{
		maxHeight = 0;
		table = NULL;
	}

	EdgeTable(int maxHeight) {
		this->maxHeight = maxHeight;
		table = new ScanlineEdges[this->maxHeight];
		for (int i = 0; i < this->maxHeight; i++)
		{
			table[i] = ScanlineEdges();
		}
	}

	~EdgeTable() {}
};



#endif