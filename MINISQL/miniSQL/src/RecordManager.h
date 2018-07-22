#ifndef RECORDMANAGER_H_
#define RECORDMANAGER_H_

#include <string>
#include <vector>
#include <iostream>

#include "IndexManager.h"
#include "RecordManager.h"
#include "BufferManager.h"

using namespace std;

class Condition;
class API;

class RecordManager{
public:
	RecordManager();
	~RecordManager();
    
    int createTable(string tableName); //create table to initialize the records 
    int dropTable(string tableName); //drop a table and all reocrds
    int dropIndex(string indexName); //drop a index on attribute
    int createIndex(string indexName); //create a index on attribute
	int insertRecord(string tableName, char* record, int recordSize); //insert reocrds into table
    int recordAllShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector); //show all records for select
    int recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int blockOffset); //show designed records for select
    int recordAllFind(string tableName, vector<Condition>* conditionVector); //find records of a table
    int recordAllDelete(string tableName, vector<Condition>* conditionVector); //delete all records of a table
    int recordBlockDelete(string tableName,  vector<Condition>* conditionVector, int blockOffset); //delete records block in the memory
    int indexRecordAllAlreadyInsert(string tableName,string indexName); //make index for records already in table
    string getTableFileName(string tableName); //return the file name of table information
    string getIndexFileName(string indexName); //return the file name of index information
	int recordLength(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int blockOffset); //calculate the max record length of a table
	int recordRowLength(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, vector<string> *attributeNameVector); //calculate the max record length of a row in the table
	void setAPI(API* apiInput);

private:
	BufferManager bm;
	API *api;

    int recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, blockNode* block); //show records in a block
    int recordBlockFind(string tableName, vector<Condition>* conditionVector, blockNode* block); //find the record block position
    int recordBlockDelete(string tableName,  vector<Condition>* conditionVector, blockNode* block); //delete a record block from memory
    int indexRecordBlockAlreadyInsert(string tableName,string indexName, blockNode* block); //make index for block already based
    bool recordConditionFit(char* recordBegin,int recordSize, vector<Attribute>* attributeVector,vector<Condition>* conditionVector); //fit the condition of blocks
    void recordPrint(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, vector<string> *attributeNameVector); //print record information
    bool contentConditionFit(char* content, int type, Condition* condition); //fit the condition of a content 
    void contentPrint(char * content, int type); //print the content in its type
};

#endif
