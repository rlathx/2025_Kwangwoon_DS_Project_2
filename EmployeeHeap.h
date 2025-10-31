#pragma once

#include "EmployeeData.h"

class EmployeeHeap {
   private:
    int datanum;  // heapsize
    EmployeeData** heapArr;
    int maxCapacity = 10;

   public:
    EmployeeHeap() {
        this->datanum = 0;
        // heap[0] is not used
        int initialSize = this->maxCapacity + 1;
        this->heapArr = new EmployeeData*[initialSize];
    }
    ~EmployeeHeap() {
        for (int i = 1; i <= this->datanum; ++i) {
            delete this->heapArr[i];
            this->heapArr[i] = nullptr;
        }
        delete[] this->heapArr;
        this->heapArr = nullptr;
    }

    void Insert(EmployeeData* data);
    EmployeeData* Top();
    bool Delete();

    bool IsEmpty();

    int UpHeap(int index);    // bubbling up
    int DownHeap(int index);  // trickle down
    void ResizeArray();
};