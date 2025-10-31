#pragma once
#include "EmployeeHeap.h"

void EmployeeHeap::Insert(EmployeeData* data) {
    if (this->datanum == this->maxCapacity) {
        ResizeArray();
    }

    int dataIncome = data->getIncome();
    dataIncome = UpHeap(dataIncome);

    this->heapArr[dataIncome] = data;
}
// Insert ST's run
EmployeeData* EmployeeHeap::Top() {
    if (IsEmpty()) {
        return nullptr;
    }
    return this->heapArr[1];
}
// Pop the top and rearrange the heap
bool EmployeeHeap::Delete() {
    if (IsEmpty()) return false;
    EmployeeData* topNode = Top();

    if (this->datanum == 1) {
        this->heapArr[1] = nullptr;
        this->datanum = 0;
        delete topNode;
        return true;
    }

    EmployeeData* lastData = this->heapArr[this->datanum];
    this->heapArr[this->datanum] = nullptr;
    this->datanum--;

    int lastIncome = lastData->getIncome();
    int pos = DownHeap(lastIncome);

    this->heapArr[pos] = lastData;

    delete topNode;

    return true;
}

bool EmployeeHeap::IsEmpty() {
    if (this->datanum == 0) {
        return true;
    }
    return false;
}

int EmployeeHeap::UpHeap(int index) {
    int currentNode = ++this->datanum;
    while ((currentNode != 1) && (this->heapArr[currentNode / 2]->getIncome() < index)) {
        this->heapArr[currentNode] = this->heapArr[currentNode / 2];
        currentNode /= 2;
    }

    return currentNode;
}

int EmployeeHeap::DownHeap(int index) {
    int currentNode = 1;  // root
    int child = 2;        // a child of currentNode

    while (child <= this->datanum) {
        // set chile to larger child of currentNode
        if ((child < this->datanum) &&
            (this->heapArr[child]->getIncome() < this->heapArr[child + 1]->getIncome())) {
            child++;
        }
        // can we put lastData in currentNode?
        if (index >= this->heapArr[child]->getIncome()) {
            break;  // yes
        }
        // no
        this->heapArr[currentNode] = this->heapArr[child];
        currentNode = child;
        child *= 2;
    }

    return currentNode;
}

void EmployeeHeap::ResizeArray() {
    this->maxCapacity *= 2;
    int initialSize = this->maxCapacity + 1;
    EmployeeData** newHeapArr = new EmployeeData*[initialSize];

    for (int i = 1; i <= this->datanum - 1; i++) {
        newHeapArr[i] = this->heapArr[i];
    }

    delete[] this->heapArr;
    this->heapArr = newHeapArr;
}