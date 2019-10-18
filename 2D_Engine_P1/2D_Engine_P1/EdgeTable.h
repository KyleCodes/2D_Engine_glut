#ifndef EDGETABLE_H
#define EDGETABLE_H
#include <iostream>
#include <string>
#include "EdgeBucketHolder.h"

using namespace std;

class EdgeTable
{
public:
	int maxHeight;
	EdgeBucketHolder* table;

	EdgeTable() {}

	EdgeTable(int maxHeight) {
		this->maxHeight = maxHeight;
		table = new EdgeBucketHolder[this->maxHeight];
		for (int i = 0; i < this->maxHeight; i++)
		{
			table[i] = EdgeBucketHolder();
		}
	}
};



#endif