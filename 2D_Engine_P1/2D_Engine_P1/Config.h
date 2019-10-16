#ifndef CONFIG_H
#define CONFIG_H
#include <iostream>
#include <string>
#include "FIO.h"

using namespace std;

class Config
{
public: 
	// Data
	string lineDrawType;
	string *shapeIDArr;
	string clip;
	string* transformations;
	FIO fileUtil;


	// Constructor
	Config() {}

	// Destructor 
	~Config() {}

	// methods to fill these values with ease 
};

#endif