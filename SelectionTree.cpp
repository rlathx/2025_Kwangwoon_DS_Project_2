#include "SelectionTree.h"

void SelectionTree::setTree() {
    // leaf node == leaf
    this->run[0]->HeapInit();
    this->run[1]->HeapInit();
    this->run[2]->HeapInit();
    this->run[3]->HeapInit();
    this->run[4]->HeapInit();
    this->run[5]->HeapInit();
    this->run[6]->HeapInit();
    this->run[7]->HeapInit();

    // internal nodes
    SelectionTreeNode* n1;
    SelectionTreeNode* n2;
    SelectionTreeNode* n3;
    SelectionTreeNode* n4;
    SelectionTreeNode* n5;
    SelectionTreeNode* n6;
    SelectionTreeNode* n7;

    // level 3
    n4->setLeftChild(this->run[0]);
    n4->setRightChild(this->run[1]);
    n4->setParent(n2);

    n5->setLeftChild(this->run[2]);
    n5->setRightChild(this->run[3]);
    n5->setParent(n2);

    n6->setLeftChild(this->run[4]);
    n6->setRightChild(this->run[5]);
    n6->setParent(n3);

    n7->setLeftChild(this->run[6]);
    n7->setRightChild(this->run[7]);
    n7->setParent(n3);

    // level 2
    n2->setLeftChild(n4);
    n2->setRightChild(n4);
    n2->setParent(n1);

    n3->setLeftChild(n6);
    n3->setRightChild(n7);
    n3->setParent(n1);

    // level 1 == root
    n1->setLeftChild(n2);
    n1->setRightChild(n3);
    setRoot(n1);
}

void SelectionTree::Insert(EmployeeData* newData) {
    // Add data to run and create heap
    int dept_no = newData->getDeptNo();

    if (this->run[(dept_no / 100) - 1]->getHeap() == nullptr) {
        EmployeeHeap* newHeap;
        newHeap->Insert(newData);
        this->run[(dept_no / 100) - 1]->setHeap(newHeap);
    } else {
        this->run[(dept_no / 100) - 1]->getHeap()->Insert(newData);
    }

    // Reordering the selection tree
}

bool SelectionTree::Delete() {
}

bool SelectionTree::printEmployeeData(int dept_no) {
}