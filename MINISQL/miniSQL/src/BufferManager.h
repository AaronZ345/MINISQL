#ifndef __Minisql__BufferManager__
#define __Minisql__BufferManager__

#include <queue>
#include <string>
#include <iostream>
#include <ctime>

#define MAX_FILE_NUM 100
#define MAX_BLOCK_NUM 500
#define MAX_FILE_NAME 100

using namespace std;

static int replaced_block = -1;

struct blockNode{
	int offsetNum; // the offset number in the block list
	bool pin;  // the flag that this block is locked
	bool ifbottom; // flag that this is the end of the file node
	char* fileName; // the file which the block node belongs to

	char *address; // the content address
	blockNode * preBlock;
	blockNode * nextBlock;
	bool reference; // the LRU replacement flag
	bool dirty; // the flag that this block is dirty, which needs to written back to the disk later
	size_t usingSize; // the byte size that the block have used. The total size of the block is BLOCK_SIZE . This value is stored in the block head.
};


struct fileNode{
	char *fileName;
	bool pin; // the flag that this file is locked
	blockNode *blockHead;
	fileNode * nextFile;
	fileNode * preFile;
};


class BufferManager{
public:
	BufferManager();
	~BufferManager();
	void deleteFileNode(const char * fileName); //delete the file in the buffer
	fileNode* getFile(const char* fileName, bool if_pin = false); //get file from the buffer
	void setDirty(blockNode & block); //set the block dirty to remind to writeback to disk
	void setPin(blockNode & block, bool pin); //set the locked file on block
	void setPin(fileNode & file, bool pin);  //set the locked file on file
	void setUsingSize(blockNode & block, size_t usage);
	size_t getUsingSize(blockNode & block);
	char* getContent(blockNode& block);
	static int getBlockSize();

	blockNode* getNextBlock(fileNode * file, blockNode* block);
	blockNode* getBlockHead(fileNode* file);
	blockNode* getBlockByOffset(fileNode* file, int offestNumber);

private:
    fileNode *fileHead;
    fileNode file_pool[MAX_FILE_NUM];
    blockNode block_pool[MAX_BLOCK_NUM];
    int totalBlock; // the number of block that have been used, which means the block is in the list.
    int totalFile; // the number of file that have been used, which means the file is in the list.
    void initBlock(blockNode & block); //initialize the block
    void initFile(fileNode & file); //initi
    blockNode* getBlock(fileNode * file,blockNode* position,bool if_pin = false);
    void writtenBackToDiskAll(); //write all blocks in the memory back to disk
    void writtenBackToDisk(const char* fileName, blockNode* block); //write designed blocks to disk
    void cleanDirty(blockNode &block); //make dirty block clean by writting back to disk
    size_t getUsingSize(blockNode* block);
    static const int BLOCK_SIZE = 4096;

};

extern clock_t start;

#endif
