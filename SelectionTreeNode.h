#pragma once
#include "EmployeeData.h"
#include "EmployeeHeap.h"

class SelectionTreeNode {
private:
    EmployeeData* pData;
    SelectionTreeNode* pLeft;
    SelectionTreeNode* pRight;
    SelectionTreeNode* pParent;
    EmployeeHeap* hRoot;

public:
    SelectionTreeNode() {
        this->pData = NULL;
        this->pLeft = NULL;
        this->pRight = NULL;
        this->pParent = NULL;
        this->hRoot = NULL;
    }
    ~SelectionTreeNode() {
        if (hRoot) delete hRoot;
    }

    void HeapInit() { this->hRoot = new EmployeeHeap; }

    void setEmployeeData(EmployeeData* data) { this->pData = data; }
    void setLeftChild(SelectionTreeNode* pL) { this->pLeft = pL; }
    void setRightChild(SelectionTreeNode* pR) { this->pRight = pR; }
    void setParent(SelectionTreeNode* pP) { this->pParent = pP; }
    void setHeap(EmployeeHeap* pHR) { this->hRoot = pHR; }

    EmployeeData* getEmployeeData() { return pData; }
    SelectionTreeNode* getLeftChild() { return pLeft; }
    SelectionTreeNode* getRightChild() { return pRight; }
    SelectionTreeNode* getParent() { return pParent; }
    EmployeeHeap* getHeap() { return hRoot; }

};