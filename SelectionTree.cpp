#include "SelectionTree.h"

#include <algorithm>
#include <map>
#include <vector>

void SelectionTree::setTree() {
    // leaf node == leaf
    for (int i = 0; i < 8; i++) {
        if (this->run[i] == nullptr) {
            // SelectionTreeNode() makes pLeft/pRight/pParent nullptr
            // so there is no need to explicitly set nullptr here
            this->run[i] = new SelectionTreeNode();
        }

        this->run[i]->HeapInit();
    }

    // internal nodes
    SelectionTreeNode* n1 = new SelectionTreeNode();  // root
    SelectionTreeNode* n2 = new SelectionTreeNode();
    SelectionTreeNode* n3 = new SelectionTreeNode();
    SelectionTreeNode* n4 = new SelectionTreeNode();
    SelectionTreeNode* n5 = new SelectionTreeNode();
    SelectionTreeNode* n6 = new SelectionTreeNode();
    SelectionTreeNode* n7 = new SelectionTreeNode();

    // level 3
    n4->setLeftChild(this->run[0]);
    n4->setRightChild(this->run[1]);
    n4->setParent(n2);
    this->run[0]->setParent(n4);
    this->run[1]->setParent(n4);

    n5->setLeftChild(this->run[2]);
    n5->setRightChild(this->run[3]);
    n5->setParent(n2);
    this->run[2]->setParent(n5);
    this->run[3]->setParent(n5);

    n6->setLeftChild(this->run[4]);
    n6->setRightChild(this->run[5]);
    n6->setParent(n3);
    this->run[4]->setParent(n6);
    this->run[5]->setParent(n6);

    n7->setLeftChild(this->run[6]);
    n7->setRightChild(this->run[7]);
    n7->setParent(n3);
    this->run[6]->setParent(n7);
    this->run[7]->setParent(n7);

    // level 2
    n2->setLeftChild(n4);
    n2->setRightChild(n5);
    n2->setParent(n1);

    n3->setLeftChild(n6);
    n3->setRightChild(n7);
    n3->setParent(n1);

    // level 1 == root
    n1->setLeftChild(n2);
    n1->setRightChild(n3);
    n1->setParent(nullptr);
    setRoot(n1);
}

void SelectionTree::Insert(EmployeeData* newData) {
    // Add data to run and create heap
    int dept_no = newData->getDeptNo();
    int runNum = (dept_no / 100) - 1;

    if (this->run[runNum]->getHeap() == nullptr) {
        EmployeeHeap* newHeap = new EmployeeHeap;
        newHeap->Insert(newData);
        this->run[runNum]->setHeap(newHeap);
    } else {
        this->run[runNum]->getHeap()->Insert(newData);
    }

    // Synchronize leaf pData to heap top
    if (this->run[runNum]->getHeap() && this->run[runNum]->getHeap()->Top()) {
        this->run[runNum]->setEmployeeData(this->run[runNum]->getHeap()->Top());
    }

    // Reordering the selection tree
    SelectionTreeNode* temp = this->run[runNum]->getParent();

    while (temp != nullptr) {
        int leftIncome = 0;
        int rightIncome = 0;

        if (temp->getLeftChild() && temp->getLeftChild()->getEmployeeData()) {
            leftIncome = temp->getLeftChild()->getEmployeeData()->getIncome();
        }
        if (temp->getRightChild() && temp->getRightChild()->getEmployeeData()) {
            rightIncome = temp->getRightChild()->getEmployeeData()->getIncome();
        }

        if (leftIncome > rightIncome) {
            if (temp->getLeftChild() && temp->getLeftChild()->getEmployeeData()) {
                string name = temp->getLeftChild()->getEmployeeData()->getName();
                int l_dept_no = temp->getLeftChild()->getEmployeeData()->getDeptNo();
                int id = temp->getLeftChild()->getEmployeeData()->getID();
                temp->getEmployeeData()->setData(name, l_dept_no, id, leftIncome);
            }
        } else if (leftIncome < rightIncome) {
            if (temp->getRightChild() && temp->getRightChild()->getEmployeeData()) {
                string name = temp->getRightChild()->getEmployeeData()->getName();
                int r_dept_no = temp->getRightChild()->getEmployeeData()->getDeptNo();
                int id = temp->getRightChild()->getEmployeeData()->getID();
                temp->getEmployeeData()->setData(name, r_dept_no, id, rightIncome);
            }
        }

        temp = temp->getParent();
    }
}

bool SelectionTree::Delete() {
    if (this->root == nullptr) {
        return false;
    }

    int dept_no = this->root->getEmployeeData()->getDeptNo();
    int runNum = (dept_no / 100) - 1;

    this->root->getEmployeeData()->setData("", 0, 0, 0);

    this->run[runNum]->getHeap()->Delete();

    if (this->run[runNum]->getHeap() && this->run[runNum]->getHeap()->Top()) {
        this->run[runNum]->setEmployeeData(this->run[runNum]->getHeap()->Top());
    } else {
        this->run[runNum]->setEmployeeData(nullptr);
    }

    // Reordering the selection tree
    SelectionTreeNode* temp = this->run[runNum]->getParent();

    while (temp != nullptr) {
        int leftIncome = 0;
        int rightIncome = 0;

        if (temp->getLeftChild() && temp->getLeftChild()->getEmployeeData()) {
            leftIncome = temp->getLeftChild()->getEmployeeData()->getIncome();
        }
        if (temp->getRightChild() && temp->getRightChild()->getEmployeeData()) {
            rightIncome = temp->getRightChild()->getEmployeeData()->getIncome();
        }

        if (leftIncome > rightIncome) {
            if (temp->getLeftChild() && temp->getLeftChild()->getEmployeeData()) {
                string name = temp->getLeftChild()->getEmployeeData()->getName();
                int l_dept_no = temp->getLeftChild()->getEmployeeData()->getDeptNo();
                int id = temp->getLeftChild()->getEmployeeData()->getID();
                temp->getEmployeeData()->setData(name, l_dept_no, id, leftIncome);
            }
        } else if (leftIncome < rightIncome) {
            if (temp->getRightChild() && temp->getRightChild()->getEmployeeData()) {
                string name = temp->getRightChild()->getEmployeeData()->getName();
                int r_dept_no = temp->getRightChild()->getEmployeeData()->getDeptNo();
                int id = temp->getRightChild()->getEmployeeData()->getID();
                // 오타 수정: rightIncome 사용
                temp->getEmployeeData()->setData(name, r_dept_no, id, rightIncome);
            }
        }

        temp = temp->getParent();
    }

    return true;
}

bool SelectionTree::printEmployeeData(int dept_no) {
    int runNum = (dept_no / 100) - 1;
    if (runNum < 0 || runNum >= 8) return false;

    SelectionTreeNode* leaf = this->run[runNum];
    if (leaf == nullptr) return false;

    EmployeeHeap* heap = leaf->getHeap();
    if (heap == nullptr) return false;

    vector<EmployeeData*> employees;
    employees.reserve(heap->getDataNum() > 0 ? heap->getDataNum() : 0);

    for (int i = 1; i <= heap->getDataNum(); i++) {
        EmployeeData* e = heap->getAt(i);
        if (e && e->getDeptNo() == dept_no) {
            employees.push_back(e);
        }
    }

    if (employees.empty()) return false;

    // Sort by descending salary
    sort(employees.begin(), employees.end(),
         [](EmployeeData* a, EmployeeData* b) { return a->getIncome() > b->getIncome(); });

    // print
    fout << "========PRINT_ST========\n";
    for (EmployeeData* data : employees) {
        fout << data->getName() << '/' << data->getDeptNo() << '/' << data->getID() << '/'
             << data->getIncome() << '\n';
    }
    fout << "=======================\n\n";

    return true;
}

void SelectionTree::clear() {
    for (int i = 0; i < 8; i++) {
        if (this->run[i]) {
            deleteSubtree(this->run[i]);
            this->run[i] = nullptr;
        }
    }
}

void SelectionTree::deleteSubtree(SelectionTreeNode* node) {
    if (!node) return;
    deleteSubtree(node->getLeftChild());
    deleteSubtree(node->getRightChild());

    delete node;
}
