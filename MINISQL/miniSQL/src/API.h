#ifndef API_H_
#define API_H_

#include <string>
#include <vector>
#include <iostream>

#include "BufferManager.h"
#include "CatalogManager.h"
#include "RecordManager.h"
#include "IndexManager.h"

using namespace std;

class Attribute;

// class for judging query conditions like '=' or '>'.
class Condition {
public:
	Condition();
	~Condition();
	Condition(string attributeInput, string valueInput, int operateInput);
	bool FitAttribute(int content); //judge conditions for int
	bool FitAttribute(float content); //judge conditions for float
	bool FitAttribute(string content); //judge conditions for string
	string getValue();
	string getAttributeName();
	int getOperate();

	const static int OPERATOR_EQUAL = 0; //"="
	const static int OPERATOR_NOT_EQUAL = 1; //"<>"
	const static int OPERATOR_LESS = 2; //"<"
	const static int OPERATOR_MORE = 3; //">"
	const static int OPERATOR_LESS_EQUAL = 4; //"<="
	const static int OPERATOR_MORE_EQUAL = 5; //">="

private:
	string attributeName;
	string value; //the value to be compared
	int operate; //the type to be compared
};


//class for API interefere
class API{
public:
    API();
	~API();
    void dropTable(string tableName); //drop table API interfere
    void createTable(string tableName, vector<Attribute>* attributeVector, string primaryKeyName,int primaryKeyLocation); //create table API interfere
    void dropIndex(string indexName); //drop index API interfere
	void createIndex(string indexName, string tableName, string attributeName); //create index API interfere
    void showRecord(string tableName, vector<string>* attributeNameVector = NULL); //show records for query
	void showRecord(string tableName,  vector<string>* attributeNameVector, vector<Condition>* conditionVector); //show records for designed condition
	void insertRecord(string tableName,vector<string>* recordContent); //insert records into table
	void deleteRecord(string tableName); //delete records in the table
	void deleteRecord(string tableName, vector<Condition>* conditionVector); //delete records for designed condition
	int getRecordNum(string tableName); //return the records number
	int getRecordSize(string tableName); //get the record size
	int getTypeSize(int type); //get a attribute type size
    void getAllIndexAddressInfo(vector<IndexInfo> *indexNameVector); //get all index file names
    int getAttribute(string tableName, vector<Attribute>* attributeVector); //get attributes of the table
    void insertIndex(string indexName, char* value, int type, int blockOffset); //insert index API interfere
    void deleteRecordIndex(char* recordBegin,int recordSize, vector<Attribute>* attributeVector, int blockOffset); //delete reocrds in an index
    void insertRecordIndex(char* recordBegin,int recordSize, vector<Attribute>* attributeVector, int blockOffset); //insert records in an index
    void setRecordManager(RecordManager *rmInput);
    void setCatalogManager(CatalogManager *cmInput);
    void setIndexManager(IndexManager *imInput);
	int getLength();

private:
    RecordManager *rm;
    CatalogManager *cm;
    IndexManager *im;
	int length; //the length of the longest value or attribute in a table

    int tableExist(string tableName); //judge if the table exist
    int getIndexNameList(string tableName, vector<string>* indexNameVector); //get all the index names
    string getPrimaryIndex(string tableName); //get index on primary attribute
    void tableAttributePrint(vector<string>* name); //print the attribute of a table
};

#endif
