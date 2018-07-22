#include <fstream>
#include <iostream>
#include <ctime>

#include "Interpreter.h"
#include "CatalogManager.h"
#include "RecordManager.h"	
#include "IndexManager.h"
#include "API.h"

using namespace std;

clock_t start; //calculate the time a operation costs

int main(){
    //open the whole index file
	FILE *fp;
	fp = fopen("INDEX_FILE", "r");
	if (fp == NULL) {
		fp = fopen("INDEX_FILE", "a+");
	}
	fclose(fp);
    
    API api;
    CatalogManager cm;
    RecordManager rm;
    
    //connect api and other modules
    api.setRecordManager(&rm);
    api.setCatalogManager(&cm);
    IndexManager im(&api);
    api.setIndexManager(&im);
    rm.setAPI(&api);
    int fileExec = 0;
    ifstream file;
    Interpreter interpreter;
    interpreter.setAPI(&api);
    string s;
    int status = 0;

	start = 0; //start the timing

	srand(time(NULL)); //get a random number for connection id

    //print basic information of minisql
	cout << "Welcome to the MiniSQL monitor.	Commands end with ;" << endl;
    cout << "Your MiniSQL connection id is " << rand() << endl;
	cout << "Server version: 2.01 MiniSQL Beta" << endl;
    cout << "\nCopyright 2018, Made by Aaron. All rights reserved." << endl;
	cout << "\nType 'help' for help." << endl;

    while(true){
        if(fileExec){
            //execute sql file
            file.open(interpreter.getFilename());
            if(!file.is_open()){
                cout<<"Fail to open "<<interpreter.getFilename()<<endl;
                fileExec = 0;
                continue;
            }
            while(getline(file,s,';')){
                interpreter.interpret(s);
            }
            file.close();
            fileExec = 0;
        }
        else{
            cout<<"\nminisql>";
            getline(cin,s,';');
			start = clock();
            status =  interpreter.interpret(s);
            if(status==2){
                //execute file
                fileExec = 1;
            }
			else if (status == -2) {
                //help information
				cout << "It's a little DBMS made by Aaron." << endl;
				cout << "Only  support int \ float \ char(n)." << endl;
				cout << "A table supports at most 32 attributes." << endl;
				cout << "You can use create, drop, delete, insert, select operations." << endl;
				cout << "You can input 'execfile' to implement a file." << endl;
				cout << "If you want to quit, input 'quit' or 'exit'." << endl;
			}
            else if(status==-1){
                //quit the minisql
                break;
            }
        }
    }

    return 0;
}
