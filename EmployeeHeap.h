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
        delete[] this->heapArr;
        this->heapArr = nullptr;
        this->datanum = 0;
        this->maxCapacity = 0;
    }

    void Insert(EmployeeData* data);
    EmployeeData* Top();
    bool Delete();

    bool IsEmpty();

    int UpHeap(int index);    // bubbling up
    int DownHeap(int index);  // trickle down
    void ResizeArray();

    int getDataNum() const {
        return this->datanum;
    }
    // Return the pointer to the ith node
    EmployeeData* getAt(int i) const {
        return this->heapArr[i];
    }

    // EmployeeData* pointer is not deleted
    void clear() {
        if (!this->heapArr) return;
        // BpTree가 EmployeeData* 해제 담당. 여기서는 포인터만 비움.
        for (int i = 1; i <= this->datanum; ++i) {
            this->heapArr[i] = nullptr;
        }
        this->datanum = 0;
    }
};