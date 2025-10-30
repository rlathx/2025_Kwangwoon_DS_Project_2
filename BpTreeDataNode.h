#pragma once
#ifndef _BpTreeDataNode_H_
#define _BpTreeDataNode_H_

#include "BpTreeNode.h"

class BpTreeDataNode : public BpTreeNode {
private:
	map <string, EmployeeData*> mapData;
	BpTreeNode* pNext;
	BpTreeNode* pPrev;
public:
	BpTreeDataNode() {
		pNext = NULL;
		pPrev = NULL;
	}
	~BpTreeDataNode() {

	}

	void setNext(BpTreeNode* pN) { pNext = pN; }
	void setPrev(BpTreeNode* pN) { pPrev = pN; }
	BpTreeNode* getNext() { return pNext; }
	BpTreeNode* getPrev() { return pPrev; }

	void insertDataMap(string name, EmployeeData* pN) {
		mapData.insert(map<string, EmployeeData*>::value_type(name, pN));
	}

	void deleteMap(string name) {
		mapData.erase(name);
	}
	map<string, EmployeeData*>* getDataMap() { return &mapData; }
};

#endif
