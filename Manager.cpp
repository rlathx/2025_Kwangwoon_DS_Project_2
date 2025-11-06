#include "Manager.h"

#include <sstream>
#include <vector>

void Manager::run(const char* command) {
    if (!this->fin.is_open()) return;

    string line;

    while (getline(this->fin, line)) {
        istringstream commandISS(line);
        string cmd;
        commandISS >> cmd;

        // command
        if (cmd == "LOAD") {
            this->LOAD();
            continue;
        } else if (cmd == "ADD_BP") {
            string name;
            int dept_no, id, income;

            if (!(commandISS >> name >> dept_no >> id >> income)) {
                this->printErrorCode(200);
                continue;
            }

            string extra;
            if (commandISS >> extra) {
                this->printErrorCode(200);
                continue;
            }

            this->ADD_BP(name, dept_no, id, income);
            continue;
        } else if (cmd == "SEARCH_BP") {
            string t1, t2;

            // Error if first argument is missing
            if (!(commandISS >> t1)) {
                this->printErrorCode(300);
                continue;
            }

            // If there is a second argument => RANGE case candidate
            if (commandISS >> t2) {
                // Error if there is a third argument (exactly 2 are allowed)
                string extra;
                if (commandISS >> extra) {
                    this->printErrorCode(300);
                    continue;
                }

                if (t1.size() == 1 && t2.size() == 1 && islower((unsigned char)t1[0]) &&
                    islower((unsigned char)t2[0])) {
                    this->SEARCH_BP_RANGE(t1, t2);
                } else {
                    this->printErrorCode(300);
                }
            } else {
                this->SEARCH_BP_NAME(t1);
            }

            continue;
        } else if (cmd == "PRINT_BP") {
            this->PRINT_BP();
            continue;
        } else if (cmd == "ADD_ST") {
            string target;
            commandISS >> target;

            if (target == "dept_no") {
                int dept_no;
                commandISS >> dept_no;

                this->ADD_ST_DEPTNO(dept_no);
            } else if (target == "name") {
                string name;
                commandISS >> name;

                this->ADD_ST_NAME(name);
            }
            continue;
        } else if (cmd == "PRINT_ST") {
            int dept_no;
            commandISS >> dept_no;

            this->PRINT_ST(dept_no);
            continue;
        } else if (cmd == "DELETE") {
            this->DELETE();
            continue;
        } else if (cmd == "EXIT") {
            if (this->stree) {
                delete this->stree;
                this->stree = nullptr;
            }
            if (this->bptree) {
                delete this->bptree;
                this->bptree = nullptr;
            }

            if (this->fin.is_open()) this->fin.close();
            this->printSuccessCode("EXIT");
            if (this->flog.is_open()) this->flog.close();

            return;
        } else {
            this->printErrorCode(800);
            continue;
        }
    }
}

void Manager::LOAD() {
    // Error if there is even one data inside the B+ Tree
    if (this->bptree != NULL && this->bptree->getRoot() != NULL) {
        BpTreeNode* curNode = this->bptree->getRoot();

        while (curNode && curNode->getIndexMap() != NULL) {
            map<string, BpTreeNode*>* indexMap = curNode->getIndexMap();
            if (indexMap->empty()) break;
            curNode = indexMap->begin()->second;
        }

        if (curNode && curNode->getDataMap() != NULL && !curNode->getDataMap()->empty()) {
            this->printErrorCode(100);
            return;
        }
    }

    ifstream dataFile("employee.txt");
    if (!dataFile.is_open()) {
        this->printErrorCode(100);
        return;
    }

    string name;
    int dept_no = 0;
    int id = 0;
    int income = 0;

    while (dataFile >> name >> dept_no >> id >> income) {
        EmployeeData* data = new EmployeeData;
        data->setData(name, dept_no, id, income);

        this->bptree->Insert(data);
    }

    dataFile.close();

    this->printSuccessCode("LOAD");
}

void Manager::ADD_BP(string name, int dept_no, int id, int income) {
    EmployeeData* newData = new EmployeeData;

    newData->setData(name, dept_no, id, income);

    if (this->bptree->Insert(newData)) {
        this->flog << "========ADD_BP========\n"
                   << name << '/' << dept_no << '/' << id << '/' << income << '\n'
                   << "=======================\n\n";

        return;
    }

    this->printErrorCode(200);
}

void Manager::SEARCH_BP_NAME(string name) {
    BpTreeNode* targeBpTreeDataNode = this->bptree->searchDataNode(name);
    if (targeBpTreeDataNode == nullptr) {
        this->printErrorCode(300);
        return;
    }

    map<string, EmployeeData*>* targetDataMap = targeBpTreeDataNode->getDataMap();
    if (targetDataMap == nullptr) {
        this->printErrorCode(300);
        return;
    }

    auto it = targetDataMap->find(name);
    if (it == targetDataMap->end() || it->second == nullptr) {
        this->printErrorCode(300);
        return;
    }

    EmployeeData* targetEmployeeData = it->second;

    this->flog << "========SEARCH_BP========\n"
               << targetEmployeeData->getName() << '/' << targetEmployeeData->getDeptNo() << '/'
               << targetEmployeeData->getID() << '/' << targetEmployeeData->getIncome() << '\n'
               << "=======================\n\n";
}

void Manager::SEARCH_BP_RANGE(string start, string end) {
    vector<string> targetDatas = this->bptree->searchRange(start, end);

    if (targetDatas.empty()) {
        this->printErrorCode(300);
        return;
    }

    this->flog << "========SEARCH_BP========\n";
    for (int i = 0; i < targetDatas.size(); i++) {
        this->flog << targetDatas[i] << '\n';
    }
    this->flog << "=======================\n\n";
}

void Manager::ADD_ST_DEPTNO(int dept_no) {
    BpTreeNode* curNode = this->bptree ? this->bptree->getRoot() : nullptr;
    if (curNode == nullptr) {
        this->printErrorCode(500);
        return;
    }

    while (curNode && curNode->getIndexMap() != nullptr) {
        map<string, BpTreeNode*>* indexMap = curNode->getIndexMap();
        if (indexMap->empty()) break;
        curNode = indexMap->begin()->second;
    }

    if (curNode == nullptr || curNode->getDataMap() == nullptr) {
        this->printErrorCode(500);
        return;
    }

    bool isInserted = false;

    while (curNode && curNode->getDataMap() != nullptr) {
        map<string, EmployeeData*>* dataMap = curNode->getDataMap();

        for (const auto& pair : *dataMap) {
            EmployeeData* data = pair.second;
            if (data == nullptr) continue;

            if (data->getDeptNo() == dept_no) {
                this->stree->Insert(data);
                isInserted = true;
            }
        }

        curNode = curNode->getNext();
    }

    if (isInserted == false) {
        this->printErrorCode(500);
        return;
    }

    this->printSuccessCode("ADD_ST");
}

void Manager::ADD_ST_NAME(string name) {
    BpTreeNode* targetBpTreeNode = this->bptree->searchDataNode(name);
    if (targetBpTreeNode == nullptr) {
        this->printErrorCode(500);
        return;
    }

    map<string, EmployeeData*>* targetDataMap = targetBpTreeNode->getDataMap();
    if (targetDataMap == nullptr) {
        this->printErrorCode(500);
        return;
    }

    auto it = targetDataMap->find(name);
    if (it == targetDataMap->end() || it->second == nullptr) {
        this->printErrorCode(500);
        return;
    }

    EmployeeData* newNode = it->second;

    this->stree->Insert(newNode);
    this->printSuccessCode("ADD_ST");
}

void Manager::PRINT_BP() {
    BpTreeNode* pCur = this->bptree->getRoot();
    if (pCur == nullptr) {
        this->printErrorCode(400);
        return;
    }

    // 1) If it is an index node, go down to the leftmost child to reach the leaf.
    while (pCur && pCur->getIndexMap() != nullptr) {
        auto* pIndex = pCur->getIndexMap();
        if (pIndex->empty()) break;
        pCur = pIndex->begin()->second;
    }

    // 2) Traverse the leaf chain and print everything
    this->flog << "========PRINT_BP========\n";

    while (pCur != nullptr) {
        map<string, EmployeeData*>* mapData = pCur->getDataMap();
        if (mapData && !mapData->empty()) {
            for (const auto& pair : *mapData) {
                EmployeeData* val = pair.second;
                if (!val) continue;

                this->flog << val->getName() << "/" << val->getDeptNo() << "/" << val->getID()
                           << "/" << val->getIncome() << "\n";
            }
        }
        pCur = pCur->getNext();
    }

    this->flog << "=======================\n\n";
}

void Manager::PRINT_ST(int dept_no) {
    if (!this->stree->printEmployeeData(dept_no)) {
        this->printErrorCode(600);
        return;
    }
}

void Manager::DELETE() {
    if (this->stree->Delete()) {
        this->printSuccessCode("DELETE");
        return;
    }

    this->printErrorCode(700);
    return;
}

void Manager::printErrorCode(int n) {
    flog << "========ERROR========\n";
    flog << n << "\n";
    flog << "=====================\n\n";
    flog.flush();
}

void Manager::printSuccessCode(string success) {
    flog << "========" << success << "========\n";
    flog << "Success" << "\n";
    flog << "====================\n\n";
    flog.flush();
}