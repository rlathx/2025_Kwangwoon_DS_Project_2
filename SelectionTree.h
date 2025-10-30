#pragma once
#include "SelectionTreeNode.h"

class SelectionTree {
private:
    SelectionTreeNode* root;
    ofstream* fout;
    SelectionTreeNode* run[8];

public:
    SelectionTree(ofstream* fout) {
        
    }

    ~SelectionTree() {

    }

    void setRoot(SelectionTreeNode* pN) { this->root = pN; }
    SelectionTreeNode* getRoot() { return root; }

    void setTree();

    bool Insert(EmployeeData* newData);
    bool Delete();
    bool printEmployeeData(int dept_no);
};