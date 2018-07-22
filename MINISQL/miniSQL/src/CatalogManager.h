#ifndef CATALOGMANAGER_H_
#define CATALOGMANAGER_H_

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "BufferManager.h"
#include "IndexManager.h"

using namespace std;

class CatalogManager {
public:
	BufferManager bm;
	CatalogManager();
	virtual ~CatalogManager();
	int addIndex(string indexName, string tableName, string attributeName, int type); //add index of a table
	int revokeIndexOnAttribute(string tableName, string AttributeName, string indexName); //revoke index of table
	int findTable(string tableName); //find table of designed tablename
	int findIndex(string indexName); //find index of designed indexname
	int dropTable(string tableName); //drop a designed table
	int dropIndex(string index); //drop a designed index
	int deleteValue(string tableName, int deleteNum);// delete the number of record
	int insertRecord(string tableName, int recordNum);  // increment the number of record
	int getRecordNum(string tableName); //get the records number of a table
	int getIndexNameList(string tableName, vector<string>* indexNameVector); //get index name list of a table
	int getAllIndex(vector<IndexInfo> * indexs); //get all indexs of a table
	int setIndexOnAttribute(string tableName, string AttributeName, string indexName); //set index on attribute of a table
	int addTable(string tableName, vector<Attribute>* attributeVector, string primaryKeyName, int primaryKeyLocation); //add table of attribute
	int getIndexType(string indexName);
	int getAttribute(string tableName, vector<Attribute>* attributeVector);
	int calcuteLenth(string tableName); //calculate the length of a table
	int calcuteLenth(int type); //calculate the length of a type
	void getRecordString(string tableName, vector<string>* recordContent, char* recordResult); //get the record in string
};

#endif
