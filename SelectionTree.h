#pragma once
#include <fstream>

#include "SelectionTreeNode.h"

class SelectionTree {
   private:
    SelectionTreeNode* root;
    std::ofstream& fout;
    SelectionTreeNode* run[8];

   public:
    SelectionTree(std::ofstream& fout) : fout(fout) {
        this->root = nullptr;
        for (int i = 0; i < 8; ++i) this->run[i] = nullptr;
    }

    ~SelectionTree() {
        this->clear();
    }

    void setRoot(SelectionTreeNode* pN) {
        this->root = pN;
    }
    SelectionTreeNode* getRoot() {
        return root;
    }

    void setTree();

    void Insert(EmployeeData* newData);
    bool Delete();
    bool printEmployeeData(int dept_no);

    void clear();
    void deleteSubtree(SelectionTreeNode* node);
};