#include "IndexManager.h"
#include "API.h"

using namespace std;

Attribute::Attribute()
{
    //default ctor
}

Attribute::~Attribute()
{
    //default dtor
}

string Attribute::getIndex()
{
	return index;
}

void Attribute::setIndex(string other)
{
	index = other;
}

string Attribute::getName()
{
	return name;
}

bool Attribute::getUnique()
{
	return ifUnique;
}

void Attribute::setUnique(bool other)
{
	ifUnique = other;
}

int Attribute::getType()
{
	return type;
}

Attribute::Attribute(string nameInput, int typeInput, bool ifUniqueInput) : 
    name(nameInput),
    type(typeInput),
    ifUnique(ifUniqueInput),
    index("")
{}


IndexInfo::IndexInfo()
{
    //default ctor
}

IndexInfo::~IndexInfo()
{
    //default dtor
}

IndexInfo::IndexInfo(string indexInput, string tableInput, string attributeInput, int typeInput) : 
    indexName(indexInput),
    tableName(tableInput),
    Attribute(attributeInput),
    type(typeInput)
{}

string IndexInfo::getAttribute()
{
	return Attribute;
}

string IndexInfo::getIndexName()
{
	return indexName;
}

string IndexInfo::getTableName()
{
	return tableName;
}

int IndexInfo::getType()
{
	return type;
}

IndexManager::IndexManager()
{
    //default ctor
}

IndexManager::IndexManager(API *apiInput)
{
    api = apiInput;
    vector<IndexInfo> allIndexInfo;
    api->getAllIndexAddressInfo(&allIndexInfo);
    for(auto i = allIndexInfo.begin(); i != allIndexInfo.end(); i++)
    {
        createIndex(i->getIndexName(), i->getType());
    }
}

IndexManager::~IndexManager()
{
    //write back to the disk
    for(auto itInt = indexIntMap.begin(); itInt != indexIntMap.end(); itInt++)
    {
        if(itInt->second)
        {
            itInt->second->WrittenBackToDisk();
            delete itInt->second;
        }
    }
    for(auto itString = IndexstringMap.begin(); itString != IndexstringMap.end(); itString ++)
    {
        if(itString->second)
        {
            itString->second->WrittenBackToDisk();
            delete itString->second;
        }
    }
    for(auto itFloat = indexFloatMap.begin(); itFloat != indexFloatMap.end(); itFloat++)
    {
        if(itFloat->second)
        {
            itFloat->second->WrittenBackToDisk();
            delete itFloat->second;
        }
    }
}

void IndexManager::createIndex(string filePath, int type)
{
    int keySize = getKeySize(type);
    int degree = getDegree(type);

    if(type == TYPE_INT)
    {
        BPlusTree<int> *tree = new BPlusTree<int>(degree, keySize, filePath);
        indexIntMap.insert(intMap::value_type(filePath, tree));
    }
    else if(type == TYPE_FLOAT)
    {
        BPlusTree<float> *tree = new BPlusTree<float>(degree, keySize, filePath);
        indexFloatMap.insert(floatMap::value_type(filePath, tree));
    }
    else
    {
        BPlusTree<string> *tree = new BPlusTree<string>(degree, keySize, filePath);
        IndexstringMap.insert(stringMap::value_type(filePath, tree));
    }
}

void IndexManager::dropIndex(string filePath, int type)
{
    if(type == TYPE_INT)
    {
        auto itInt = indexIntMap.find(filePath);
        if(itInt == indexIntMap.end())
        {
            cout << "Error:in drop index, no index " << filePath <<" exits" << endl;
            return;
        }
        else
        {
            delete itInt->second;
            indexIntMap.erase(itInt);
        }
    }
    else if(type == TYPE_FLOAT)
    {
        auto itFloat = indexFloatMap.find(filePath);
        if(itFloat == indexFloatMap.end())
        {
            cout << "Error:in drop index, no index " << filePath <<" exits" << endl;
            return;
        }
        else
        {
            delete itFloat->second;
            indexFloatMap.erase(itFloat);
        }
    }
    else
    {
        auto itString = IndexstringMap.find(filePath);
        if(itString == IndexstringMap.end())
        {
            cout << "Error:in drop index, no index " << filePath <<" exits" << endl;
            return;
        }
        else
        {
            delete itString->second;
            IndexstringMap.erase(itString);
        }
    }
}

offsetNum IndexManager::searchIndex(string filePath, string key, int type)
{
    setKey(type, key);
    if(type == TYPE_INT)
    {
        auto itInt = indexIntMap.find(filePath);
        if(itInt == indexIntMap.end())
        {
            cout << "Error:in search index, no index " << filePath <<" exits" << endl;
            return -1;
        }
        else
        {
            int index = 0;
            BPlusTreeNode<int> *node = itInt->second->SearchInTree(kt.intTmp, index);
            return (node->keySet[index] == kt.intTmp) ? node->valSet[index] : -1;
        }
    }
    else if(type == TYPE_FLOAT)
    {
        auto itFloat = indexFloatMap.find(filePath);
        if(itFloat == indexFloatMap.end())
        {
            cout << "Error:in search index, no index " << filePath <<" exits" << endl;
            return -1;
        }
        else
        {
            int index = 0;
            BPlusTreeNode<float> *node = itFloat->second->SearchInTree(kt.floatTmp, index);
            return (node->keySet[index] == kt.floatTmp) ? node->valSet[index] : -1;
        }
    }
    else
    {
        auto itString = IndexstringMap.find(filePath);
        if(itString == IndexstringMap.end())
        {
            cout << "Error:in search index, no index " << filePath <<" exits" << endl;
            return -1;
        }
        else
        {
            int index = 0;
            BPlusTreeNode<string> *node = itString->second->SearchInTree(key, index);
            return (node->keySet[index] == key) ? node->valSet[index] : -1;
        }
    }
}

void IndexManager::insertIndex(string filePath, string key, offsetNum blockOffset, int type)
{
    setKey(type, key);
    if(type == TYPE_INT)
    {
        auto itInt = indexIntMap.find(filePath);
        if(itInt == indexIntMap.end())
        {
            cout << "Error:in search index, no index " << filePath <<" exits" << endl;
            return;
        }
        else
        {
            itInt->second->InsertKey(kt.intTmp, blockOffset);
        }
    }
    else if(type == TYPE_FLOAT)
    {
        auto itFloat = indexFloatMap.find(filePath);
        if(itFloat == indexFloatMap.end())
        {
            cout << "Error:in search index, no index " << filePath <<" exits" << endl;
            return;
        }
        else
        {
            itFloat->second->InsertKey(kt.floatTmp, blockOffset);
        }
    }
    else
    {
        auto itString = IndexstringMap.find(filePath);
        if(itString == IndexstringMap.end())
        {
            cout << "Error:in search index, no index " << filePath <<" exits" << endl;
            return;
        }
        else
        {
            itString->second->InsertKey(key, blockOffset);
        }
    }
}

void IndexManager::deleteIndexByKey(string filePath, string key, int type)
{
    setKey(type, key);
    if(type == TYPE_INT)
    {
        auto itInt = indexIntMap.find(filePath);
        if(itInt == indexIntMap.end())
        {
            cout << "Error:in search index, no index " << filePath <<" exits" << endl;
            return;
        }
        else
        {
            itInt->second->DeleteKey(kt.intTmp);
        }
    }
    else if(type == TYPE_FLOAT)
    {
        auto itFloat = indexFloatMap.find(filePath);
        if(itFloat == indexFloatMap.end())
        {
            cout << "Error:in search index, no index " << filePath <<" exits" << endl;
            return;
        }
        else
        {
            itFloat->second->DeleteKey(kt.floatTmp);
        }
    }
    else
    {
        auto itString = IndexstringMap.find(filePath);
        if(itString == IndexstringMap.end())
        {
            cout << "Error:in search index, no index " << filePath <<" exits" << endl;
            return;
        }
        else
        {
			cout << "in delete index by key function. the key = " << key << endl;
            itString->second->DeleteKey(key);
        }
    }
}

int IndexManager::getDegree(int type)
{
    int degree = bm.getBlockSize() / (getKeySize(type) + sizeof(offsetNum));
    return (degree % 2 == 0) ? degree - 1 : degree;
}

int IndexManager::getKeySize(int type)
{
    if(type == TYPE_FLOAT)
        return sizeof(float);
    else if(type == TYPE_INT)
        return sizeof(int);
    else if(type > 0)
        return type + 1;
    else
    {
        cout << "ERROR: in getKeySize: invalid type" << endl;
        return -100;
    }
}

void IndexManager::setKey(int type,string key)
{
    stringstream ss;
    ss << key;

    if(type == this->TYPE_INT)
        ss >> this->kt.intTmp;
    else if(type == this->TYPE_FLOAT)
        ss >> this->kt.floatTmp;
    else if(type > 0)
        ss >> this->kt.stringTmp;
    else
        cout << "Error: in setKey: invalid type" << endl;
}
