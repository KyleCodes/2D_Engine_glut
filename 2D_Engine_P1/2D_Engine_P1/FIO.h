// TODO: COPY FROM OLD C++ PROJECT

#ifndef FIO_H
#define FIO_H
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class FIO
{
public:
	// Data
	string inputfileName;
	string outputFileName;

	// Constructor
	FIO();
	FIO(string);
	~FIO();

	// SETTERS 
	void changeFilePath(string);
	void changeOutputName(string);

	// FILE OPERATIONS

};

FIO::FIO()
{
	inputfileName = "testfile";
}

FIO::FIO(string newName)
{
	inputfileName = newName;
}

FIO::~FIO() {}


#endif