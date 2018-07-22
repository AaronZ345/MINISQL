#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include<iostream>
#include <string>
#include <vector>
#include "API.h"
#include "IndexManager.h"

using namespace std;
class Interpreter{
public:
	Interpreter();
	~Interpreter();
	int interpret(string s); //interprete the commmand to API
	string getFilename();
	void setAPI(API* apiInput);

private:
    API* api;
	string fileName;
	string getCommand(string s, int &st); //get each word in the command
};

#endif
