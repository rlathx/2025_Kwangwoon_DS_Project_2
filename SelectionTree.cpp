#include "SelectionTree.h"

#include <algorithm>
#include <map>
#include <vector>

void SelectionTree::setTree() {
    // Initialize 8 leaf nodes (runs). If a leaf is missing, create it and
    // initialize its heap area. Each leaf represents one department bucket.
    for (int i = 0; i < 8; i++) {
        if (this->run[i] == nullptr) {
            // SelectionTreeNode() sets pLeft/pRight/pParent to nullptr by default
            this->run[i] = new SelectionTreeNode();
        }
        this->run[i]->HeapInit();
    }

    // Create internal nodes (full binary tree of height 3)
    SelectionTreeNode* n1 = new SelectionTreeNode();  // root (level 1)
    SelectionTreeNode* n2 = new SelectionTreeNode();  // level 2
    SelectionTreeNode* n3 = new SelectionTreeNode();  // level 2
    SelectionTreeNode* n4 = new SelectionTreeNode();  // level 3
    SelectionTreeNode* n5 = new SelectionTreeNode();  // level 3
    SelectionTreeNode* n6 = new SelectionTreeNode();  // level 3
    SelectionTreeNode* n7 = new SelectionTreeNode();  // level 3

    // Connect level-3 internal nodes to leaves and set parent pointers
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

    // Connect level-2 nodes to level-3 nodes and set parents
    n2->setLeftChild(n4);
    n2->setRightChild(n5);
    n2->setParent(n1);

    n3->setLeftChild(n6);
    n3->setRightChild(n7);
    n3->setParent(n1);

    // Connect root to level-2 nodes and finalize the tree root
    n1->setLeftChild(n2);
    n1->setRightChild(n3);
    n1->setParent(nullptr);
    setRoot(n1);
}

void SelectionTree::Insert(EmployeeData* newData) {
    // Map department number to run index (100~800 → 0~7)
    int dept_no = newData->getDeptNo();
    int runNum = (dept_no / 100) - 1;

    // Create a heap for the leaf if missing, then insert the new record
    if (this->run[runNum]->getHeap() == nullptr) {
        EmployeeHeap* newHeap = new EmployeeHeap;
        newHeap->Insert(newData);
        this->run[runNum]->setHeap(newHeap);
    } else {
        this->run[runNum]->getHeap()->Insert(newData);
    }

    // Synchronize the leaf node’s displayed data with the heap’s top (max by income)
    if (auto top = this->run[runNum]->getHeap()->Top()) {
        this->run[runNum]->getEmployeeData()->setData(top->getName(), top->getDeptNo(),
                                                      top->getID(), top->getIncome());
    }

    // Propagate winners upward: at each internal node, keep the child with higher income
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

        // Select higher-income child and copy its data to the current node
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
        // If equal, keep existing (no change)

        temp = temp->getParent();
    }
}

bool SelectionTree::Delete() {
    // Nothing to delete if the root is absent
    if (this->root == nullptr) {
        return false;
    }

    // Determine the run corresponding to the current global winner (root)
    int dept_no = this->root->getEmployeeData()->getDeptNo();
    int runNum = (dept_no / 100) - 1;

    // Clear the root’s displayed data (visual reset)
    this->root->getEmployeeData()->setData("", 0, 0, 0);

    // Pop the winner from the corresponding heap
    this->run[runNum]->getHeap()->Delete();

    // Refresh the leaf’s displayed data with the new heap top (or clear if empty)
    if (auto heap = this->run[runNum]->getHeap()) {
        if (auto top = heap->Top()) {
            this->run[runNum]->getEmployeeData()->setData(top->getName(), top->getDeptNo(),
                                                          top->getID(), top->getIncome());
        } else {
            this->run[runNum]->getEmployeeData()->setData("", 0, 0, 0);
        }
    }

    // Recompute winners up to the root after deletion
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
        // If equal, keep existing (no change)

        temp = temp->getParent();
    }

    return true;
}

bool SelectionTree::printEmployeeData(int dept_no) {
    // Compute the run index and validate range
    int runNum = (dept_no / 100) - 1;
    if (runNum < 0 || runNum >= 8) return false;

    // Access the corresponding leaf and its heap
    SelectionTreeNode* leaf = this->run[runNum];
    if (leaf == nullptr) return false;

    EmployeeHeap* heap = leaf->getHeap();
    if (heap == nullptr) return false;

    // Collect all employees of the given department from the heap array
    vector<EmployeeData*> employees;
    employees.reserve(heap->getDataNum() > 0 ? heap->getDataNum() : 0);

    for (int i = 1; i <= heap->getDataNum(); i++) {
        EmployeeData* e = heap->getAt(i);
        if (e && e->getDeptNo() == dept_no) {
            employees.push_back(e);
        }
    }

    // Nothing to print if empty
    if (employees.empty()) return false;

    // Sort by income in descending order for output
    sort(employees.begin(), employees.end(),
         [](EmployeeData* a, EmployeeData* b) { return a->getIncome() > b->getIncome(); });

    // Print with the required banner format
    fout << "========PRINT_ST========\n";
    for (EmployeeData* data : employees) {
        fout << data->getName() << '/' << data->getDeptNo() << '/' << data->getID() << '/'
             << data->getIncome() << '\n';
    }
    fout << "=======================\n\n";

    return true;
}

void SelectionTree::clear() {
    // Delete the entire tree starting from the root
    if (this->root) {
        deleteSubtree(this->root);
        this->root = nullptr;
    }
    // Invalidate leaf references (the nodes were deleted by deleteSubtree)
    for (int i = 0; i < 8; ++i) {
        this->run[i] = nullptr;
    }
}

void SelectionTree::deleteSubtree(SelectionTreeNode* node) {
    // Post-order delete: clear children first, then delete the current node
    if (!node) return;
    deleteSubtree(node->getLeftChild());
    deleteSubtree(node->getRightChild());
    delete node;
}
