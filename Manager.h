#pragma once
#include <fstream>

#include "BpTree.h"
#include "SelectionTree.h"

class Manager {
   private:
    char* cmd;
    BpTree* bptree;
    SelectionTree* stree;

   public:
    Manager(int bpOrder) {  // constructor
        this->cmd = NULL;

        this->fin.open("command.txt");
        this->flog.open("log.txt");

        this->bptree = new BpTree(this->flog, bpOrder);
        this->stree = new SelectionTree(this->flog);
        this->stree->setTree();
    }

    ~Manager() {  // destructor
        if (this->stree) {
            delete this->stree;
            this->stree = NULL;
        }
        if (this->bptree) {
            delete this->bptree;
            this->bptree = NULL;
        }

        if (this->fin.is_open()) this->fin.close();
        if (this->flog.is_open()) this->flog.close();
    }

    ifstream fin;
    ofstream flog;

    void run(const char* command);
    void LOAD();
    void ADD_BP(string name, int dept_no, int id, int income);
    void SEARCH_BP_NAME(string name);
    void SEARCH_BP_RANGE(string start, string end);
    void PRINT_BP();
    void ADD_ST_DEPTNO(int dept_no);
    void ADD_ST_NAME(string name);
    void PRINT_ST(int dept_no);
    void DELETE();

    void printErrorCode(int n);
    void printSuccessCode(string success);
};
