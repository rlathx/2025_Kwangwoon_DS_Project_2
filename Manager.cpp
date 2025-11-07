#include "Manager.h"

#include <sstream>
#include <vector>

void Manager::run(const char* command) {
    // Stop if the command file is not open
    if (!this->fin.is_open()) return;

    string line;

    // Read each command line from the input file
    while (getline(this->fin, line)) {
        istringstream commandISS(line);
        string cmd;
        commandISS >> cmd;

        // ===== LOAD =====
        if (cmd == "LOAD") {
            this->LOAD();
            continue;
        }

        // ===== ADD_BP : Insert employee data into B+ Tree =====
        else if (cmd == "ADD_BP") {
            string name;
            int dept_no, id, income;

            // Missing arguments → error 200
            if (!(commandISS >> name >> dept_no >> id >> income)) {
                this->printErrorCode(200);
                continue;
            }

            // Extra arguments → error 200
            string extra;
            if (commandISS >> extra) {
                this->printErrorCode(200);
                continue;
            }

            // Valid input → perform insertion
            this->ADD_BP(name, dept_no, id, income);
            continue;
        }

        // ===== SEARCH_BP : Search employee(s) in B+ Tree =====
        else if (cmd == "SEARCH_BP") {
            string t1, t2;

            // No first argument → error 300
            if (!(commandISS >> t1)) {
                this->printErrorCode(300);
                continue;
            }

            // Two arguments → range search
            if (commandISS >> t2) {
                // More than two arguments → error 300
                string extra;
                if (commandISS >> extra) {
                    this->printErrorCode(300);
                    continue;
                }

                // Both args must be lowercase single letters
                if (t1.size() == 1 && t2.size() == 1 && islower((unsigned char)t1[0]) &&
                    islower((unsigned char)t2[0])) {
                    this->SEARCH_BP_RANGE(t1, t2);
                } else {
                    this->printErrorCode(300);
                }
            }
            // Single argument → name search
            else {
                string extra;
                if (commandISS >> extra) {
                    this->printErrorCode(300);
                    continue;
                }
                this->SEARCH_BP_NAME(t1);
            }

            continue;
        }

        // ===== PRINT_BP : Print all data in B+ Tree =====
        else if (cmd == "PRINT_BP") {
            this->PRINT_BP();
            continue;
        }

        // ===== ADD_ST : Add data to Selection Tree =====
        else if (cmd == "ADD_ST") {
            string target;
            if (!(commandISS >> target)) {
                this->printErrorCode(500);
                continue;
            }

            // ADD_ST dept_no {number}
            if (target == "dept_no") {
                int dept_no;
                if (!(commandISS >> dept_no)) {
                    this->printErrorCode(500);
                    continue;
                }
                string extra;
                if (commandISS >> extra) {
                    this->printErrorCode(500);
                    continue;
                }
                this->ADD_ST_DEPTNO(dept_no);
            }

            // ADD_ST name {string}
            else if (target == "name") {
                string name;
                if (!(commandISS >> name)) {
                    this->printErrorCode(500);
                    continue;
                }
                string extra;
                if (commandISS >> extra) {
                    this->printErrorCode(500);
                    continue;
                }
                this->ADD_ST_NAME(name);
            }

            // Invalid subcommand
            else {
                this->printErrorCode(500);
            }
            continue;
        }

        // ===== PRINT_ST : Print Selection Tree by department =====
        else if (cmd == "PRINT_ST") {
            int dept_no;
            commandISS >> dept_no;
            this->PRINT_ST(dept_no);
            continue;
        }

        // ===== DELETE : Delete top element in Selection Tree =====
        else if (cmd == "DELETE") {
            this->DELETE();
            continue;
        }

        // ===== EXIT : Terminate program, free memory and close files =====
        else if (cmd == "EXIT") {
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
        }

        // ===== Unknown command =====
        else {
            this->printErrorCode(800);
            continue;
        }
    }
}

void Manager::LOAD() {
    // Prevent duplicate load if B+ Tree already contains data
    if (this->bptree != NULL && this->bptree->getRoot() != NULL) {
        BpTreeNode* curNode = this->bptree->getRoot();
        while (curNode && curNode->getIndexMap() != NULL) {
            auto* indexMap = curNode->getIndexMap();
            if (!indexMap || indexMap->empty()) break;
            curNode = indexMap->begin()->second;
        }
        if (curNode && curNode->getDataMap() != NULL && !curNode->getDataMap()->empty()) {
            this->printErrorCode(100);
            return;
        }
    }

    // Try opening employee.txt
    ifstream dataFile("employee.txt");
    if (!dataFile.is_open()) {
        this->printErrorCode(100);
        return;
    }

    // Read and insert each record into B+ Tree
    string name;
    int dept_no = 0, id = 0, income = 0;
    int inserted = 0;
    while (dataFile >> name >> dept_no >> id >> income) {
        EmployeeData* data = new EmployeeData;
        data->setData(name, dept_no, id, income);
        if (this->bptree->Insert(data)) {
            ++inserted;
        } else {
            delete data;
            dataFile.close();
            this->printErrorCode(100);
            return;
        }
    }
    dataFile.close();

    // No data inserted → error
    if (inserted == 0) {
        this->printErrorCode(100);
        return;
    }

    // Success
    this->printSuccessCode("LOAD");
}

void Manager::ADD_BP(string name, int dept_no, int id, int income) {
    // Create new employee record
    EmployeeData* newData = new EmployeeData;
    newData->setData(name, dept_no, id, income);

    // Try inserting into B+ Tree
    if (this->bptree->Insert(newData)) {
        this->flog << "========ADD_BP========\n"
                   << name << '/' << dept_no << '/' << id << '/' << income << '\n'
                   << "=======================\n\n";
        return;
    }

    // Failed insertion → error 200
    this->printErrorCode(200);
}

void Manager::SEARCH_BP_NAME(string name) {
    // Locate the leaf node containing this name
    BpTreeNode* targetNode = this->bptree->searchDataNode(name);
    if (targetNode == nullptr) {
        this->printErrorCode(300);
        return;
    }

    // Access its data map
    map<string, EmployeeData*>* dataMap = targetNode->getDataMap();
    if (dataMap == nullptr) {
        this->printErrorCode(300);
        return;
    }

    // Find the target record
    auto it = dataMap->find(name);
    if (it == dataMap->end() || it->second == nullptr) {
        this->printErrorCode(300);
        return;
    }

    // Print result
    EmployeeData* data = it->second;
    this->flog << "========SEARCH_BP========\n"
               << data->getName() << '/' << data->getDeptNo() << '/' << data->getID() << '/'
               << data->getIncome() << '\n'
               << "=======================\n\n";
}

void Manager::SEARCH_BP_RANGE(string start, string end) {
    // Search for all employees whose names are in [start, end]
    bool found = this->bptree->searchRange(start, end);
    if (!found) {
        this->printErrorCode(300);
        return;
    }
    return;
}

void Manager::ADD_ST_DEPTNO(int dept_no) {
    // Traverse B+ Tree from root to the leftmost leaf
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

    // Insert all employees with matching dept_no into Selection Tree
    bool inserted = false;
    while (curNode && curNode->getDataMap() != nullptr) {
        map<string, EmployeeData*>* dataMap = curNode->getDataMap();
        for (const auto& pair : *dataMap) {
            EmployeeData* data = pair.second;
            if (data == nullptr) continue;
            if (data->getDeptNo() == dept_no) {
                this->stree->Insert(data);
                inserted = true;
            }
        }
        curNode = curNode->getNext();
    }

    if (!inserted) {
        this->printErrorCode(500);
        return;
    }

    this->printSuccessCode("ADD_ST");
}

void Manager::ADD_ST_NAME(string name) {
    // Find specific employee by name
    BpTreeNode* node = this->bptree->searchDataNode(name);
    if (node == nullptr) {
        this->printErrorCode(500);
        return;
    }

    map<string, EmployeeData*>* dataMap = node->getDataMap();
    if (dataMap == nullptr) {
        this->printErrorCode(500);
        return;
    }

    auto it = dataMap->find(name);
    if (it == dataMap->end() || it->second == nullptr) {
        this->printErrorCode(500);
        return;
    }

    // Insert found record into Selection Tree
    EmployeeData* record = it->second;
    this->stree->Insert(record);
    this->printSuccessCode("ADD_ST");
}

void Manager::PRINT_BP() {
    // Go down to the leftmost leaf node
    BpTreeNode* pCur = this->bptree->getRoot();
    if (pCur == nullptr) {
        this->printErrorCode(400);
        return;
    }

    while (pCur && pCur->getIndexMap() != nullptr) {
        auto* pIndex = pCur->getIndexMap();
        if (!pIndex || pIndex->empty()) break;
        pCur = pIndex->begin()->second;
    }

    // Collect all records from leaf chain
    size_t printed = 0;
    std::ostringstream buf;

    while (pCur != nullptr) {
        auto* mapData = pCur->getDataMap();
        if (mapData && !mapData->empty()) {
            for (const auto& pair : *mapData) {
                EmployeeData* val = pair.second;
                if (!val) continue;
                buf << val->getName() << "/" << val->getDeptNo() << "/" << val->getID() << "/"
                    << val->getIncome() << "\n";
                ++printed;
            }
        }
        pCur = pCur->getNext();
    }

    // If no data was printed → error 400
    if (printed == 0) {
        this->printErrorCode(400);
        return;
    }

    // Output to log file
    this->flog << "========PRINT_BP========\n";
    this->flog << buf.str();
    this->flog << "=======================\n\n";
}

void Manager::PRINT_ST(int dept_no) {
    // Print all employees in Selection Tree of given department
    if (!this->stree->printEmployeeData(dept_no)) {
        this->printErrorCode(600);
        return;
    }
}

void Manager::DELETE() {
    // Delete top record from Selection Tree
    if (this->stree->Delete()) {
        this->printSuccessCode("DELETE");
        return;
    }

    // Nothing to delete → error 700
    this->printErrorCode(700);
    return;
}

void Manager::printErrorCode(int n) {
    // Format error message to log file
    flog << "========ERROR========\n";
    flog << n << "\n";
    flog << "=====================\n\n";
    flog.flush();
}

void Manager::printSuccessCode(string success) {
    // Format success message to log file
    flog << "========" << success << "========\n";
    flog << "Success" << "\n";
    flog << "====================\n\n";
    flog.flush();
}
