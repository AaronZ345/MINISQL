#include "API.h"
#include "IndexManager.h"
#include <cstring>

using namespace std;

#define UNKNOWN_FILE -10 //unexist file
#define TABLE_FILE -20 //file including table information
#define INDEX_FILE -30 //file including index information

CatalogManager *cm;
IndexManager* im;

Condition::Condition() {
    //default ctor
}

Condition::~Condition() {
    //default dtor
}

Condition::Condition(string attributeInput, string valueInput, int operateInput) :
attributeName(attributeInput),value(valueInput),operate(operateInput){
	//ctor with input
}

bool Condition::FitAttribute(int content) {
	stringstream ss;
	ss << value;
	int myContent;
	ss >> myContent;

    //judge which condition to implement
	switch (operate) {
	case Condition::OPERATOR_EQUAL:
		return content == myContent;
	case Condition::OPERATOR_NOT_EQUAL:
		return content != myContent;
	case Condition::OPERATOR_LESS:
		return content < myContent;
	case Condition::OPERATOR_MORE:
		return content > myContent;
	case Condition::OPERATOR_LESS_EQUAL:
		return content <= myContent;
	case Condition::OPERATOR_MORE_EQUAL:
		return content >= myContent;
	default:
		return true;
	}
}

bool Condition::FitAttribute(float content) {
	stringstream ss;
	ss << value;
	float myContent;
	ss >> myContent;

	switch (operate) {
	case Condition::OPERATOR_EQUAL:
		return content == myContent;
	case Condition::OPERATOR_NOT_EQUAL:
		return content != myContent;
	case Condition::OPERATOR_LESS:
		return content < myContent;
	case Condition::OPERATOR_MORE:
		return content > myContent;
	case Condition::OPERATOR_LESS_EQUAL:
		return content <= myContent;
	case Condition::OPERATOR_MORE_EQUAL:
		return content >= myContent;
	default:
		return true;
	}
}

bool Condition::FitAttribute(string content) {
	string myContent = value;
	switch (operate) {
	case Condition::OPERATOR_EQUAL:
		return content == myContent;
	case Condition::OPERATOR_NOT_EQUAL:
		return content != myContent;
	case Condition::OPERATOR_LESS:
		return content < myContent;
	case Condition::OPERATOR_MORE:
		return content > myContent;
	case Condition::OPERATOR_LESS_EQUAL:
		return content <= myContent;
	case Condition::OPERATOR_MORE_EQUAL:
		return content >= myContent;
	default:
		return true;
	}
}

string Condition::getValue() {
	return value;
}

string Condition::getAttributeName() {
	return attributeName;
}

int Condition::getOperate() {
	return operate;
}


API::API():length(0){
    //default ctor
}

API::~API(){
    //default dtor
}

void API::dropTable(string tableName){
    if (!tableExist(tableName)) 
        return;
    vector<string> indexNameVector;

    //get all indexs in the table, and drop them all
    getIndexNameList(tableName, &indexNameVector);
    for (int i = 0; i < indexNameVector.size(); i++){
        cout << indexNameVector[i];    
        dropIndex(indexNameVector[i]);
    }
    
    //delete a table file
    if(rm->dropTable(tableName)){
        //delete a table information
        cm->dropTable(tableName);
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "Query OK, table " << tableName << " affected.(" << duration << " s)"<< endl;
    }
}

void API::dropIndex(string indexName){
    if (cm->findIndex(indexName) != INDEX_FILE){
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "There is no index " << indexName <<".("<< duration <<" s)"<< endl;
        return;
    }
    
    //delete a index file
    if (rm->dropIndex(indexName)){
        //get type of index
        int indexType = cm->getIndexType(indexName);
        if (indexType == -2){
			clock_t finish = clock();
			double duration = (double)(finish - start) / CLOCKS_PER_SEC;
            cout << "Error!("<<duration <<" s)" << endl;
            return;
        }

        //delete a index information
        cm->dropIndex(indexName);
        
        //delete a index tree
        im->dropIndex(rm->getIndexFileName(indexName), indexType);

		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
		cout << "Query OK, index " << indexName << " affected.(" << duration << " s)" << endl;
    }
}

void API::createIndex(string indexName, string tableName, string attributeName){
    if (cm->findIndex(indexName) == INDEX_FILE){
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "There is index " << indexName << " already.(" <<duration <<" s)"<< endl;
        return;
    }
    
    if (!tableExist(tableName)) 
        return;
    
    vector<Attribute> attributeVector;
    cm->getAttribute(tableName, &attributeVector);
    int i;
    int type = 0;
    for (i = 0; i < attributeVector.size(); i++){
        if (attributeName == attributeVector[i].getName()){
            if (!attributeVector[i].getUnique()){
				clock_t finish = clock();
				double duration = (double)(finish - start) / CLOCKS_PER_SEC;
                cout << "The attribute is not unique.("<<duration<<" s)" << endl;
                return;
            }
            type = attributeVector[i].getType();
            break;
        }
    }
    
    if (i == attributeVector.size()){
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "There is not this attribute in the table.("<<duration<<" s)" << endl;
        return;
    }
    
     //RecordManager to create a index file
    if (rm->createIndex(indexName)){
        //CatalogManager to add a index information
        cm->addIndex(indexName, tableName, attributeName, type);
        
        //get type of index
        int indexType = cm->getIndexType(indexName);
        if (indexType == -2){
			clock_t finish = clock();
			double duration = (double)(finish - start) / CLOCKS_PER_SEC;
            cout << "Error!("<<duration<<" s)" << endl;
            return;
        }
        
        //indexManager to create a index tress
        im->createIndex(rm->getIndexFileName(indexName), indexType);
        
        //recordManager insert already record to index
        rm->indexRecordAllAlreadyInsert(tableName, indexName);

		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
		cout << "Query OK, index " << indexName << " affected.(" << duration << " s)" << endl;
    }
    else{
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "Create index " << indexName << " fail.("<<duration<<" s)" << endl;
    }
}

void API::createTable(string tableName, vector<Attribute>* attributeVector, string primaryKeyName,int primaryKeyLocation){   
    if(cm->findTable(tableName) == TABLE_FILE){
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "There is a table " << tableName << " already.("<<duration<<" s)" << endl;
        return;
    }
    
    //RecordManager to create a table file
    if(rm->createTable(tableName)){
        //CatalogManager to create a table information
        cm->addTable(tableName, attributeVector, primaryKeyName, primaryKeyLocation);
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
		cout << "Query OK, table " << tableName << " affected.(" << duration << " s)" << endl;
    }
    
    if (primaryKeyName != ""){
        //get a primary key
        string indexName = getPrimaryIndex(tableName);
        createIndex(indexName, tableName, primaryKeyName);
    }
}

void API::showRecord(string tableName, vector<string>* attributeNameVector){
    vector<Condition> conditionVector;
    showRecord(tableName, attributeNameVector, &conditionVector);
}


void API::showRecord(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector){
    if (cm->findTable(tableName) == TABLE_FILE){
        int num = 0;
        vector<Attribute> attributeVector;
        getAttribute(tableName, &attributeVector);
        
        vector<string> allAttributeName;
        if (attributeNameVector == NULL) {
            for (Attribute attribute : attributeVector){
                allAttributeName.insert(allAttributeName.end(), attribute.getName());
            }
            attributeNameVector = &allAttributeName;
        }
        
        for (string name : (*attributeNameVector)){
            int i = 0;
            for (i = 0; i < attributeVector.size(); i++){
                if (attributeVector[i].getName() == name)
                    break;
            }
            if (i == attributeVector.size()){
				clock_t finish = clock();
				double duration = (double)(finish - start) / CLOCKS_PER_SEC;
                cout << "The attribute which you want to print is not exist in the table.("<<duration<<" s)" << endl;
                return;
            }
        }
        
        int blockOffset = -1;
        if (conditionVector != NULL){
            for (Condition condition : *conditionVector){
                int i = 0;
                for (i = 0; i < attributeVector.size(); i++){
                    if (attributeVector[i].getName() == condition.getAttributeName()){
                        if (condition.getOperate() == Condition::OPERATOR_EQUAL && attributeVector[i].getIndex() != ""){
                                blockOffset = im->searchIndex(rm->getIndexFileName(attributeVector[i].getIndex()), condition.getValue(), attributeVector[i].getType());
                        }
                        break;
                    }
                }
                
                if (i == attributeVector.size()){
					clock_t finish = clock();
					double duration = (double)(finish - start) / CLOCKS_PER_SEC;
                    cout << "The attribute is not exist in the table.("<< duration<<" s)"<< endl;
                    return;
                }
            }
        }
		//calculate the max length of the values and attributes
		length = rm->recordLength(tableName, attributeNameVector, conditionVector, blockOffset);	
		
        //print all the attributes you want to show
        tableAttributePrint(attributeNameVector);
    
        if (blockOffset == -1){
            //cout << "if we con't find the block by index,we need to find all block" << endl;
            num = rm->recordAllShow(tableName, attributeNameVector,conditionVector);
        }
        else{
            //find the block by index,search in the block
            num = rm->recordBlockShow(tableName, attributeNameVector, conditionVector, blockOffset);
        }

		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
		cout << "Query OK, "<<num<<"records affected. ("<< duration << " s)" << endl;
    }
    else{
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "There is no table named " << tableName <<".("<<duration<<" s)"<< endl;
    }

	length = 0; //prepare for next query
}

void API::insertRecord(string tableName, vector<string>* recordContent){
    if (!tableExist(tableName)) return;
    
    string indexName;
    vector<Attribute> attributeVector; 
    vector<Condition> conditionVector;
    
    getAttribute(tableName, &attributeVector);
    for (int i = 0 ; i < attributeVector.size(); i++){
        indexName = attributeVector[i].getIndex();
        if (indexName != ""){
            //if the attribute has a index
            int blockoffest = im->searchIndex(rm->getIndexFileName(indexName), (*recordContent)[i], attributeVector[i].getType());
            if (blockoffest != -1){
                //if the value has exist in index tree then fail to insert the record
				clock_t finish = clock();
				double duration = (double)(finish - start) / CLOCKS_PER_SEC;
                cout << "insert fail because index value exist.("<<duration<<" s)" << endl;
                return;
            }
        }
        else if (attributeVector[i].getUnique()){
            //if the attribute is unique but not index
            Condition condition(attributeVector[i].getName(), (*recordContent)[i], Condition::OPERATOR_EQUAL);
            conditionVector.insert(conditionVector.end(), condition);
        }
    }
    
    if (conditionVector.size() > 0){
        for (int i = 0; i < conditionVector.size(); i++) {
            vector<Condition> conditionTmp;
            conditionTmp.insert(conditionTmp.begin(), conditionVector[i]);
            int recordConflictNum =  rm->recordAllFind(tableName, &conditionTmp);
            if (recordConflictNum > 0) {
				clock_t finish = clock();
				double duration = (double)(finish - start) / CLOCKS_PER_SEC;
                cout << "insert fail because unique value exist.("<<duration<<" s)" << endl;
                return;
            }
        }
    }
    
    char recordString[2000];
    memset(recordString, 0, 2000);
    
    //CatalogManager to get the record string
    cm->getRecordString(tableName, recordContent, recordString);
    
    //RecordManager to insert the record into file; and get the position of block being insert
    int recordSize = cm->calcuteLenth(tableName);
    int blockOffset = rm->insertRecord(tableName, recordString, recordSize);
    
    if(blockOffset >= 0){
        insertRecordIndex(recordString, recordSize, &attributeVector, blockOffset);
        cm->insertRecord(tableName, 1);
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
		cout << "Query OK, table " << tableName << " affected.(" << duration << " s)" << endl;
    }
    else{
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "Insert record into table " << tableName << " fail.("<<duration<<" s)" << endl;
    }
}

void API::deleteRecord(string tableName){
    vector<Condition> conditionVector;
    deleteRecord(tableName, &conditionVector);
}

void API::deleteRecord(string tableName, vector<Condition>* conditionVector){
    if (!tableExist(tableName)) return;
    
    int num = 0;
    vector<Attribute> attributeVector;
    getAttribute(tableName, &attributeVector);

    int blockOffset = -1;
    if (conditionVector != NULL){
        for (Condition condition : *conditionVector){
            if (condition.getOperate() == Condition::OPERATOR_EQUAL){
                for (Attribute attribute : attributeVector){
                    if (attribute.getIndex() != "" && attribute.getName() == condition.getAttributeName()){
                        blockOffset = im->searchIndex(rm->getIndexFileName(attribute.getIndex()), condition.getValue(), attribute.getType());
                    }
                }
            }
        }
    }
    if (blockOffset == -1){
        //if we con't find the block by index,we need to find all block
        num = rm->recordAllDelete(tableName, conditionVector);
    }
    else{
        //find the block by index,search in the block
        num = rm->recordBlockDelete(tableName, conditionVector, blockOffset);
    }
    
    //delete the number of record in in the table
    cm->deleteValue(tableName, num);
	clock_t finish = clock();
	double duration = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "Query OK, " << num << " records in table "<< tableName<<"affected.(" << duration << " s)" << endl;
}

int API::getRecordNum(string tableName){
    if (!tableExist(tableName)) 
        return 0;

    return cm->getRecordNum(tableName);
}

int API::getRecordSize(string tableName){
    if (!tableExist(tableName)) 
        return 0;
    
    return cm->calcuteLenth(tableName);
}

int API::getTypeSize(int type){
    return cm->calcuteLenth(type);
}

int API::getIndexNameList(string tableName, vector<string>* indexNameVector){
    if (!tableExist(tableName))
        return 0;

    return cm->getIndexNameList(tableName, indexNameVector);
}

void API::getAllIndexAddressInfo(vector<IndexInfo> *indexNameVector){
    cm->getAllIndex(indexNameVector);
    for (int i = 0; i < (*indexNameVector).size(); i++){
        (*indexNameVector)[i].getIndexName() = rm->getIndexFileName((*indexNameVector)[i].getIndexName());
    }
}

int API::getAttribute(string tableName, vector<Attribute>* attributeVector){
    if (!tableExist(tableName))
        return 0;

    return cm->getAttribute(tableName, attributeVector);
}

void API::insertRecordIndex(char* recordBegin,int recordSize, vector<Attribute>* attributeVector,  int blockOffset){
    char* contentBegin = recordBegin;
    for (int i = 0; i < (*attributeVector).size() ; i++){
        int type = (*attributeVector)[i].getType();
        int typeSize = getTypeSize(type);
        if ((*attributeVector)[i].getIndex() != ""){
            insertIndex((*attributeVector)[i].getIndex(), contentBegin, type, blockOffset);
        }
        
        contentBegin += typeSize;
    }
}

void API::insertIndex(string indexName, char* contentBegin, int type, int blockOffset){
    string content= "";
    stringstream tmp;
    //if the attribute has index
    if (type == Attribute::TYPE_INT){
        int value = *((int*)contentBegin);
        tmp << value;
    }
    else if (type == Attribute::TYPE_FLOAT){
        float value = *((float* )contentBegin);
        tmp << value;
    }
    else{
        char value[255];
        memset(value, 0, 255);
        memcpy(value, contentBegin, sizeof(type));
        string stringTmp = value;
        tmp << stringTmp;
    }
    tmp >> content;
    im->insertIndex(rm->getIndexFileName(indexName), content, blockOffset, type);
}

void API::deleteRecordIndex(char* recordBegin,int recordSize, vector<Attribute>* attributeVector, int blockOffset){
    char* contentBegin = recordBegin;
    for (int i = 0; i < (*attributeVector).size() ; i++){
        int type = (*attributeVector)[i].getType();
        int typeSize = getTypeSize(type);
        
        string content= "";
        stringstream tmp;
        
        if ((*attributeVector)[i].getIndex() != ""){
            //if the attribute has index
            if (type == Attribute::TYPE_INT){
                int value = *((int*)contentBegin);
                tmp << value;
            }
            else if (type == Attribute::TYPE_FLOAT){
                float value = *((float* )contentBegin);
                tmp << value;
            }
            else{
                char value[255];
                memset(value, 0, 255);
                memcpy(value, contentBegin, sizeof(type));
                string stringTmp = value;
                tmp << stringTmp;
            }
            
            tmp >> content;
            im->deleteIndexByKey(rm->getIndexFileName((*attributeVector)[i].getIndex()), content, type);

        }
        contentBegin += typeSize;
    }

}

int API::tableExist(string tableName){
    if (cm->findTable(tableName) != TABLE_FILE){
		clock_t finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "There is no table named " << tableName<<".("<<duration<<" s)" << endl;
        return 0;
    }
    
    return 1;
}

string API::getPrimaryIndex(string tableName){
    return  "PRIMARY_" + tableName;
}

void API::tableAttributePrint(vector<string>* attributeNameVector){
	int i = 0;
	for (i = 0; i < (*attributeNameVector).size(); i++) {
		if ((*attributeNameVector)[i].size() > length)
			length = (*attributeNameVector)[i].size();
	}
	if (length % 2)
		length = length + 1;

	for (i = 0; i < (*attributeNameVector).size(); i++) {
		for (int j = 0; j < length + 4; j += 2)
			cout << "* ";
	}
	cout << "*\n* ";
    for ( i = 0; i < (*attributeNameVector).size(); i++){
        cout << (*attributeNameVector)[i];
		for (int j = 0; j < length + 2 - (*attributeNameVector)[i].size(); j++)
			cout << " ";
		cout << "* ";
    }
	if (i != 0) {
		cout << "\n";
		for (i = 0; i < (*attributeNameVector).size(); i++) {
			for (int j = 0; j < length + 4; j += 2)
				cout << "* ";
		}
		cout << "*";
	}
	cout << "\n";
}

void API::setRecordManager(RecordManager *rmInput){
    rm = rmInput;
}

void API::setCatalogManager(CatalogManager *cmInput){
    cm = cmInput;
}

void API::setIndexManager(IndexManager *imInput){
    im = imInput;
}

int API::getLength() {
	return length;
}
