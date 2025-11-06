#include "BpTree.h"

#include <queue>
#include <sstream>
#include <vector>

bool BpTree::Insert(EmployeeData* newData) {
    if (this->root == nullptr) {
        BpTreeNode* newNode = new BpTreeDataNode;
        string name = newData->getName();
        newNode->insertDataMap(name, newData);
        newNode->setParent(this->root);  // nullptr
        this->root = newNode;
        return true;
    }

    BpTreeNode* temp = this->root;
    const string name = newData->getName();

    // Go down the index node and find the data node to insert
    while (temp && temp->getDataMap() ==
                       nullptr) {  // If there is no data map, it is considered an index node.
        map<string, BpTreeNode*>* tempIndex = temp->getIndexMap();
        if (!tempIndex || tempIndex->empty()) break;

        bool moved = false;
        // Traverse left to right and move to the child of the first key where name <= key
        for (const auto& pair : *tempIndex) {
            const string& key = pair.first;
            BpTreeNode* child = pair.second;
            if (name <= key) {
                temp = child;
                moved = true;
                break;
            }
        }
        // If greater than all keys: move to the rightmost child
        if (!moved) {
            temp = tempIndex->rbegin()->second;
        }
    }

    // When we reach here, temp is a data node
    if (!temp) return false;

    temp->insertDataMap(name, newData);

    // Split/promote if necessary (continue calling according to your logic)
    if (excessDataNode(temp)) {
        splitDataNode(temp);
    }
    return true;
}

bool BpTree::excessDataNode(BpTreeNode* pDataNode) {
    // If the number of keys in the data node exceeds order-1, it is considered a split target.
    if (!pDataNode || pDataNode->getDataMap() == nullptr) return false;
    map<string, EmployeeData*>* dmap = pDataNode->getDataMap();
    return (int)dmap->size() > (this->order - 1);
}

bool BpTree::excessIndexNode(BpTreeNode* pIndexNode) {
    if (pIndexNode == nullptr) return false;

    map<string, BpTreeNode*>* indexMap = pIndexNode->getIndexMap();
    if (indexMap == nullptr) return false;

    if ((int)indexMap->size() >= this->order) {
        return true;
    }
    return false;
}

void BpTree::splitDataNode(BpTreeNode* pDataNode) {
    if (!pDataNode || pDataNode->getDataMap() == nullptr) return;
    BpTreeDataNode* leftNode = static_cast<BpTreeDataNode*>(pDataNode);
    map<string, EmployeeData*>* leftMap = leftNode->getDataMap();
    if (leftMap->empty()) return;

    // 1) Calculate the division criteria (left is the front ceil(n/2), right is the remainder)
    const int n = (int)leftMap->size();
    const int leftKeep = (n + 1) / 2;  // ceil
    int idx = 0;

    // First, collect the keys to be moved to the right
    vector<string> moveKeys;
    for (const auto& pair : *leftMap) {
        if (idx >= leftKeep) moveKeys.push_back(pair.first);
        ++idx;
    }

    // 2) Create right node & move data
    BpTreeDataNode* rightNode = new BpTreeDataNode;

    for (const string& k : moveKeys) {
        // insert to right
        EmployeeData* val = (*leftMap)[k];
        rightNode->insertDataMap(k, val);
    }
    // Delete keys moved from the left
    for (const string& k : moveKeys) {
        leftNode->deleteMap(k);
    }

    // 3) Leaf connection (pPrev/pNext) organization
    // Move the next leftNode to the rightNode, preserving the chains on both sides
    BpTreeNode* oldNext = leftNode->getNext();
    rightNode->setPrev(leftNode);
    rightNode->setNext(oldNext);
    leftNode->setNext(rightNode);
    if (oldNext && oldNext->getDataMap() != nullptr) {
        // Restore prev connection only when oldNext is a leaf
        static_cast<BpTreeDataNode*>(oldNext)->setPrev(rightNode);
    }

    // 4) Register two children to the parent index
    // - Use each child's "maximum key" as the parent mapIndex key.
    // - Map the value to the corresponding child pointer.
    // When traversing, if name <= key, go to that child. Otherwise, go to rbegin()->second.

    // Find the maximum left/right height
    auto leftMaxIt = leftNode->getDataMap()->empty() ? leftNode->getDataMap()->end()
                                                     : prev(leftNode->getDataMap()->end());
    auto rightMaxIt = rightNode->getDataMap()->empty() ? rightNode->getDataMap()->end()
                                                       : prev(rightNode->getDataMap()->end());

    // It is normal for empty pages not to occur, but just in case, it is a defense
    string leftMaxKey = (leftMaxIt == leftNode->getDataMap()->end()) ? string() : leftMaxIt->first;
    string rightMaxKey =
        (rightMaxIt == rightNode->getDataMap()->end()) ? string() : rightMaxIt->first;

    BpTreeNode* parent = pDataNode->getParent();

    auto erase_child_from_parent = [](BpTreeNode* parentNode, BpTreeNode* child) {
        if (!parentNode || parentNode->getIndexMap() == nullptr) return;
        map<string, BpTreeNode*>* imap = parentNode->getIndexMap();
        string targetKey;
        bool found = false;
        for (const auto& pr : *imap) {
            if (pr.second == child) {
                targetKey = pr.first;
                found = true;
                break;
            }
        }
        if (found) imap->erase(targetKey);
    };

    if (!parent) {
        // 4-A) Create a new index root if there is no parent
        BpTreeIndexNode* newRoot = new BpTreeIndexNode;

        newRoot->insertIndexMap(leftMaxKey, leftNode);
        newRoot->insertIndexMap(rightMaxKey, rightNode);

        leftNode->setParent(newRoot);
        rightNode->setParent(newRoot);

        this->root = newRoot;
    } else {
        // 4-B) Reflection on existing parents
        erase_child_from_parent(parent, pDataNode);

        map<string, BpTreeNode*>* pIndex = parent->getIndexMap();
        if (pIndex) {
            pIndex->insert(map<string, BpTreeNode*>::value_type(leftMaxKey, leftNode));
            pIndex->insert(map<string, BpTreeNode*>::value_type(rightMaxKey, rightNode));
        }
        leftNode->setParent(parent);
        rightNode->setParent(parent);

        if (excessIndexNode(parent)) {
            splitIndexNode(parent);
        }
    }
}

void BpTree::splitIndexNode(BpTreeNode* pIndexNode) {
    if (!pIndexNode || pIndexNode->getIndexMap() == nullptr) return;
    BpTreeIndexNode* leftNode = static_cast<BpTreeIndexNode*>(pIndexNode);
    map<string, BpTreeNode*>* leftMap = leftNode->getIndexMap();
    if (leftMap->empty()) return;

    // 1) Split criteria (keep the front half, move the back half)
    const int n = (int)leftMap->size();
    const int leftKeep = (n + 1) / 2;  // ceil
    int idx = 0;

    vector<string> moveKeys;  // Keys to move to the right
    for (const auto& pair : *leftMap) {
        if (idx >= leftKeep) moveKeys.push_back(pair.first);
        ++idx;
    }

    // 2) Create right index node & move entry
    BpTreeIndexNode* rightNode = new BpTreeIndexNode;
    map<string, BpTreeNode*>* rightMap = rightNode->getIndexMap();

    for (const string& k : moveKeys) {
        BpTreeNode* child = (*leftMap)[k];
        rightMap->insert(map<string, BpTreeNode*>::value_type(k, child));
    }
    for (const string& k : moveKeys) {
        leftMap->erase(k);
    }

    // Re-parenting of children (left/right each parent itself)
    for (const auto& pair : *leftMap) {
        if (pair.second) pair.second->setParent(leftNode);
    }
    for (const auto& pair : *rightMap) {
        if (pair.second) pair.second->setParent(rightNode);
    }

    // 3) Calculate the maximum left/right key (for indexing)
    auto leftMaxIt = leftMap->empty() ? leftMap->end() : prev(leftMap->end());
    auto rightMaxIt = rightMap->empty() ? rightMap->end() : prev(rightMap->end());

    string leftMaxKey = (leftMaxIt == leftMap->end()) ? string() : leftMaxIt->first;
    string rightMaxKey = (rightMaxIt == rightMap->end()) ? string() : rightMaxIt->first;

    // 4) Parent update (root promotion or upward propagation)
    BpTreeNode* parent = pIndexNode->getParent();

    auto erase_child_from_parent = [](BpTreeNode* parentNode, BpTreeNode* child) {
        if (!parentNode || parentNode->getIndexMap() == nullptr) return;
        map<string, BpTreeNode*>* imap = parentNode->getIndexMap();
        string eraseKey;
        bool found = false;
        for (const auto& pr : *imap) {
            if (pr.second == child) {
                eraseKey = pr.first;
                found = true;
                break;
            }
        }
        if (found) imap->erase(eraseKey);
    };

    if (!parent) {
        // 4-A) Splitting the root: Creating a new root index
        BpTreeIndexNode* newRoot = new BpTreeIndexNode;
        newRoot->insertIndexMap(leftMaxKey, leftNode);
        newRoot->insertIndexMap(rightMaxKey, rightNode);

        leftNode->setParent(newRoot);
        rightNode->setParent(newRoot);
        this->root = newRoot;
    } else {
        // 4-B) Reflection on existing parents
        erase_child_from_parent(parent, pIndexNode);  // Remove old entries

        map<string, BpTreeNode*>* pIndex = parent->getIndexMap();
        if (pIndex) {
            pIndex->insert(map<string, BpTreeNode*>::value_type(leftMaxKey, leftNode));
            pIndex->insert(map<string, BpTreeNode*>::value_type(rightMaxKey, rightNode));
        }
        leftNode->setParent(parent);
        rightNode->setParent(parent);

        // When the number of parents exceeds the number of parents, it is transmitted to the upper
        // level.
        if (excessIndexNode(parent)) {
            splitIndexNode(parent);
        }
    }
}

BpTreeNode* BpTree::searchDataNode(string name) {
    if (!this->root) return nullptr;

    // 1) Go down from the root
    BpTreeNode* cur = this->root;

    // Through the index node to the data node
    while (cur && cur->getDataMap() == nullptr) {
        map<string, BpTreeNode*>* imap = cur->getIndexMap();
        if (!imap || imap->empty()) return nullptr;

        bool moved = false;
        for (const auto& pair : *imap) {
            const string& key = pair.first;
            BpTreeNode* child = pair.second;
            if (name <= key) {
                cur = child;
                moved = true;
                break;
            }
        }
        if (!moved) {
            cur = imap->rbegin()->second;  // If it's bigger than all the keys, right end
        }
    }

    return cur;
}

vector<string> BpTree::searchRange(string start, string end) {
    vector<string> targetDatas;

    if (start > end) std::swap(start, end);

    // 1) Start from the leaf where start is entered
    BpTreeNode* p = searchDataNode(start);
    if (!p) return targetDatas;

    // 2) Traverse the leaf chain to the end
    // Collect [start, end] by scanning the mapData of each leaf in the forward direction

    while (p && p->getDataMap() != nullptr) {
        map<string, EmployeeData*>* dmap = p->getDataMap();

        for (const auto& pair : *dmap) {
            const string& key = pair.first;
            EmployeeData* val = pair.second;

            if (key < start) continue;
            if (key > end) break;

            ostringstream oss;
            oss << val->getName() << '/' << val->getDeptNo() << '/' << val->getID() << '/'
                << val->getIncome();
            string target = oss.str();

            targetDatas.push_back(target);
        }

        p = static_cast<BpTreeDataNode*>(p)->getNext();
    }

    return targetDatas;
}

void BpTree::clear() {
    if (!this->root) return;

    std::queue<BpTreeNode*> q;
    q.push(this->root);

    while (!q.empty()) {
        BpTreeNode* node = q.front();
        q.pop();

        if (node->getMostLeftChild()) {  // index node
            auto* imap = node->getIndexMap();
            if (imap) {
                for (auto& pair : *imap) {
                    if (pair.second) q.push(pair.second);
                }
            }
        } else {  // data node
            auto* dmap = node->getDataMap();
            if (dmap) {
                for (auto& pair : *dmap) {
                    delete pair.second;  // EmployeeData* delete only here
                }
            }
        }
        delete node;
    }
    this->root = nullptr;
}
