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
// Never delete topNode
// Delete is only performed on BpTree (or a single, explicitly defined owner)
bool EmployeeHeap::Delete() {
    if (IsEmpty()) return false;

    if (this->datanum == 1) {
        this->heapArr[1] = nullptr;
        this->datanum = 0;
        return true;  // No delete (non-ownership)
    }

    EmployeeData* lastData = this->heapArr[this->datanum];
    this->heapArr[this->datanum] = nullptr;
    this->datanum--;

    int pos = DownHeap(lastData->getIncome());
    this->heapArr[pos] = lastData;

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
    int oldCapacity = this->maxCapacity;
    this->maxCapacity *= 2;
    int newCapacity = this->maxCapacity;

    EmployeeData** newHeapArr = new EmployeeData*[newCapacity + 1];

    for (int i = 1; i <= this->datanum; i++) {
        newHeapArr[i] = this->heapArr[i];
    }
    for (int i = this->datanum + 1; i <= newCapacity; i++) {
        newHeapArr[i] = nullptr;
    }

    delete[] this->heapArr;
    this->heapArr = newHeapArr;
}