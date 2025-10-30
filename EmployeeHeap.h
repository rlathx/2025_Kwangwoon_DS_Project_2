#pragma once

#include "EmployeeData.h"

class EmployeeHeap {
private:
    int datanum;
    EmployeeData** heapArr;
    int maxCapacity = 10;

public:
    EmployeeHeap() {
        
    }
    ~EmployeeHeap() {
        
    }
    
    void Insert(EmployeeData* data);
    EmployeeData* Top();
    void Delete();

    bool IsEmpty();

    void UpHeap(int index);
    void DownHeap(int index);
    void ResizeArray();
};