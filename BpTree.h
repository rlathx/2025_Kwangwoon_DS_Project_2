#pragma once
#ifndef _BpTree_H_
#define _BpTree_H_

#include <fstream>
#include <vector>

#include "BpTreeDataNode.h"
#include "BpTreeIndexNode.h"
#include "EmployeeData.h"
#include "SelectionTree.h"

class BpTree {
   private:
    BpTreeNode* root;
    int order;  // m children
    ofstream* fout;

   public:
    BpTree(ofstream* fout, int order = 3) {
        this->root = nullptr;
        this->order = order;
        this->fout = fout;
    }

    ~BpTree() {
        this->clear();
    }

    void clear();

    /* essential */
    bool Insert(EmployeeData* newData);
    bool excessDataNode(BpTreeNode* pDataNode);
    bool excessIndexNode(BpTreeNode* pIndexNode);
    void splitDataNode(BpTreeNode* pDataNode);
    void splitIndexNode(BpTreeNode* pIndexNode);
    BpTreeNode* getRoot() {
        return root;
    }
    BpTreeNode* searchDataNode(string name);
    vector<string> searchRange(string start, string end);
};

#endif
