#pragma once
#include <iostream>
#include <string>
#include <map>
using namespace std;

class EmployeeData {
private:
    string name;
    int dept_no;
    int id;
    int income;

public:
    EmployeeData() {};
    ~EmployeeData() {};

    void setData(string name, int dept_no, int id, int income) {
        this->name = name;
        this->dept_no = dept_no;
        this->id = id;
        this->income = income;
    }

    void setName(string name) { this->name = name; }
    void setDeptNo(int dept_no) { this->dept_no = dept_no; }
    void setID(int id) { this->id = id; }
    void setIncome(int income) { this->income = income; }

    string getName() { return name; }
    int getDeptNo() { return dept_no; }
    int getID() { return id; }
    int getIncome() { return income; }
};