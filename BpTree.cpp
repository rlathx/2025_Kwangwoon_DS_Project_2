#include "BpTree.h"

#include <queue>
#include <sstream>
#include <vector>

bool BpTree::Insert(EmployeeData* newData) {
    const string name = newData->getName();

    // If the tree is empty, create a new leaf node and set it as root
    if (this->root == nullptr) {
        BpTreeNode* newLeaf = new BpTreeDataNode;
        newLeaf->insertDataMap(name, newData);
        newLeaf->setParent(nullptr);
        this->root = newLeaf;
        return true;
    }

    // Locate the leaf node where this key should be inserted
    BpTreeNode* leaf = searchDataNode(name);
    if (!leaf) return false;

    // Insert data into the leaf node
    leaf->insertDataMap(name, newData);

    // If the leaf overflows, split it
    if (excessDataNode(leaf)) {
        splitDataNode(leaf);
    }
    return true;
}

bool BpTree::excessDataNode(BpTreeNode* pDataNode) {
    // Check whether the data node exceeds the maximum number of entries
    if (!pDataNode || pDataNode->getDataMap() == nullptr) return false;
    map<string, EmployeeData*>* dmap = pDataNode->getDataMap();
    return (int)dmap->size() > (this->order - 1);
}

bool BpTree::excessIndexNode(BpTreeNode* pIndexNode) {
    // Check whether the index node exceeds the maximum number of keys
    if (pIndexNode == nullptr) return false;

    map<string, BpTreeNode*>* indexMap = pIndexNode->getIndexMap();
    if (indexMap == nullptr) return false;

    if ((int)indexMap->size() >= this->order) {
        return true;
    }
    return false;
}

void BpTree::splitDataNode(BpTreeNode* pDataNode) {
    // Split an overflowing leaf (data node)
    if (!pDataNode || pDataNode->getDataMap() == nullptr) return;
    BpTreeDataNode* leftNode = static_cast<BpTreeDataNode*>(pDataNode);
    auto* leftMap = leftNode->getDataMap();
    if (leftMap->empty()) return;

    // Determine the split position
    const int n = static_cast<int>(leftMap->size());
    const int splitPos = (this->order / 2) + 1;
    const int leftKeep = std::min(n, std::max(1, splitPos));

    // Collect keys to move to the right node
    int idx = 0;
    std::vector<std::string> moveKeys;
    for (const auto& pair : *leftMap) {
        if (idx >= leftKeep) moveKeys.push_back(pair.first);
        ++idx;
    }

    // Create the right node and move data
    BpTreeDataNode* rightNode = new BpTreeDataNode;
    for (const std::string& k : moveKeys) {
        rightNode->insertDataMap(k, (*leftMap)[k]);
    }
    for (const std::string& k : moveKeys) {
        leftNode->deleteMap(k);
    }

    // Update leaf node linkage (next/prev pointers)
    BpTreeNode* oldNext = leftNode->getNext();
    rightNode->setPrev(leftNode);
    rightNode->setNext(oldNext);
    leftNode->setNext(rightNode);
    if (oldNext && oldNext->getDataMap() != nullptr) {
        static_cast<BpTreeDataNode*>(oldNext)->setPrev(rightNode);
    }

    // Compute maximum key of each child for parent insertion
    auto leftMaxIt = leftNode->getDataMap()->empty() ? leftNode->getDataMap()->end()
                                                     : std::prev(leftNode->getDataMap()->end());
    auto rightMaxIt = rightNode->getDataMap()->empty() ? rightNode->getDataMap()->end()
                                                       : std::prev(rightNode->getDataMap()->end());
    std::string leftMaxKey =
        (leftMaxIt == leftNode->getDataMap()->end()) ? std::string() : leftMaxIt->first;
    std::string rightMaxKey =
        (rightMaxIt == rightNode->getDataMap()->end()) ? std::string() : rightMaxIt->first;

    // Get the parent node
    BpTreeNode* parent = pDataNode->getParent();

    // Lambda: remove a specific child reference from the parent index map
    auto erase_child_from_parent = [](BpTreeNode* parentNode, BpTreeNode* child) {
        if (!parentNode || parentNode->getIndexMap() == nullptr) return;
        auto* imap = parentNode->getIndexMap();
        std::string targetKey;
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

    // Lambda: refresh the most-left child pointer of an index node
    auto refreshMostLeftChild = [](BpTreeNode* indexNode) {
        if (!indexNode) return;
        auto* imap = indexNode->getIndexMap();
        if (imap && !imap->empty())
            indexNode->setMostLeftChild(imap->begin()->second);
        else
            indexNode->setMostLeftChild(nullptr);
    };

    // Case 1: No parent → create a new root index node
    if (!parent) {
        BpTreeIndexNode* newRoot = new BpTreeIndexNode;
        newRoot->insertIndexMap(leftMaxKey, leftNode);
        newRoot->insertIndexMap(rightMaxKey, rightNode);
        leftNode->setParent(newRoot);
        rightNode->setParent(newRoot);
        refreshMostLeftChild(newRoot);
        this->root = newRoot;
    }
    // Case 2: Parent exists → update and check for overflow
    else {
        erase_child_from_parent(parent, pDataNode);
        if (auto* pIndex = parent->getIndexMap()) {
            pIndex->insert({leftMaxKey, leftNode});
            pIndex->insert({rightMaxKey, rightNode});
        }
        leftNode->setParent(parent);
        rightNode->setParent(parent);
        refreshMostLeftChild(parent);
        if (excessIndexNode(parent)) splitIndexNode(parent);
    }
}

void BpTree::splitIndexNode(BpTreeNode* pIndexNode) {
    // Split an overflowing index node
    if (!pIndexNode || pIndexNode->getIndexMap() == nullptr) return;
    BpTreeIndexNode* leftNode = static_cast<BpTreeIndexNode*>(pIndexNode);
    auto* leftMap = leftNode->getIndexMap();
    if (leftMap->empty()) return;

    // Determine split point
    const int n = static_cast<int>(leftMap->size());
    const int splitPos = (this->order / 2) + 1;
    const int leftKeep = std::min(n, std::max(1, splitPos));

    // Collect keys to move to right node
    int idx = 0;
    std::vector<std::string> moveKeys;
    for (const auto& pair : *leftMap) {
        if (idx >= leftKeep) moveKeys.push_back(pair.first);
        ++idx;
    }

    // Create right index node and move entries
    BpTreeIndexNode* rightNode = new BpTreeIndexNode;
    auto* rightMap = rightNode->getIndexMap();
    for (const std::string& k : moveKeys) {
        BpTreeNode* child = (*leftMap)[k];
        rightMap->insert({k, child});
    }
    for (const std::string& k : moveKeys) {
        leftMap->erase(k);
    }

    // Update parent pointers for all child nodes
    for (const auto& pr : *leftMap)
        if (pr.second) pr.second->setParent(leftNode);
    for (const auto& pr : *rightMap)
        if (pr.second) pr.second->setParent(rightNode);

    // Get max key of each side for parent linkage
    auto leftMaxIt = leftMap->empty() ? leftMap->end() : std::prev(leftMap->end());
    auto rightMaxIt = rightMap->empty() ? rightMap->end() : std::prev(rightMap->end());
    std::string leftMaxKey = (leftMaxIt == leftMap->end()) ? std::string() : leftMaxIt->first;
    std::string rightMaxKey = (rightMaxIt == rightMap->end()) ? std::string() : rightMaxIt->first;

    // Lambda: refresh the most-left child pointer
    auto refreshMostLeftChild = [](BpTreeNode* indexNode) {
        if (!indexNode) return;
        auto* imap = indexNode->getIndexMap();
        if (imap && !imap->empty())
            indexNode->setMostLeftChild(imap->begin()->second);
        else
            indexNode->setMostLeftChild(nullptr);
    };

    // Update child pointers
    refreshMostLeftChild(leftNode);
    refreshMostLeftChild(rightNode);

    BpTreeNode* parent = pIndexNode->getParent();

    // Lambda: erase a specific child from its parent index map
    auto erase_child_from_parent = [](BpTreeNode* parentNode, BpTreeNode* child) {
        if (!parentNode || parentNode->getIndexMap() == nullptr) return;
        auto* imap = parentNode->getIndexMap();
        std::string eraseKey;
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

    // Case 1: No parent → create a new root
    if (!parent) {
        BpTreeIndexNode* newRoot = new BpTreeIndexNode;
        newRoot->insertIndexMap(leftMaxKey, leftNode);
        newRoot->insertIndexMap(rightMaxKey, rightNode);
        leftNode->setParent(newRoot);
        rightNode->setParent(newRoot);
        refreshMostLeftChild(newRoot);
        this->root = newRoot;
    }
    // Case 2: Parent exists → update parent's index map
    else {
        erase_child_from_parent(parent, pIndexNode);
        if (auto* pIndex = parent->getIndexMap()) {
            pIndex->insert({leftMaxKey, leftNode});
            pIndex->insert({rightMaxKey, rightNode});
        }
        leftNode->setParent(parent);
        rightNode->setParent(parent);
        refreshMostLeftChild(parent);
        if (excessIndexNode(parent)) splitIndexNode(parent);
    }
}

BpTreeNode* BpTree::searchDataNode(string name) {
    // Find the leaf node where a given key should be located
    if (!this->root) return nullptr;

    BpTreeNode* cur = this->root;

    // Descend through index nodes until a data node is reached
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
        // If key is greater than all, go to rightmost child
        if (!moved) {
            cur = imap->rbegin()->second;
        }
    }

    return cur;
}

bool BpTree::searchRange(string start, string end) {
    // Swap if the range is reversed
    if (start > end) std::swap(start, end);

    // Find the first leaf that may contain 'start'
    BpTreeNode* p = searchDataNode(start);
    if (!p) return false;

    bool flag = false;

    // Sequentially traverse the leaf chain to collect all keys in range
    while (p && p->getDataMap() != nullptr) {
        auto* dmap = p->getDataMap();
        for (const auto& kv : *dmap) {
            const string& key = kv.first;
            EmployeeData* val = kv.second;
            if (key < start) continue;
            if (key > end) break;

            // Print header once when the first match is found
            if (flag == false) {
                fout << "========SEARCH_BP========\n";
            }
            flag = true;

            // Output matching record
            fout << val->getName() << '/' << val->getDeptNo() << '/' << val->getID() << '/'
                 << val->getIncome() << '\n';
        }
        // Move to next leaf
        p = static_cast<BpTreeDataNode*>(p)->getNext();
    }

    // Print footer if any results were found
    if (flag == true) {
        fout << "=======================\n\n";
        return true;
    }

    return false;
}

void BpTree::clear() {
    // Delete all nodes and free allocated memory
    if (!this->root) return;

    std::queue<BpTreeNode*> q;
    q.push(this->root);

    while (!q.empty()) {
        BpTreeNode* node = q.front();
        q.pop();

        // Case: Data node → delete all EmployeeData objects
        if (node->getDataMap() != nullptr) {
            auto* dmap = node->getDataMap();
            for (auto& pr : *dmap) {
                delete pr.second;
            }
        }
        // Case: Index node → enqueue all child pointers
        else {
            auto* imap = node->getIndexMap();
            if (imap) {
                for (auto& pr : *imap) {
                    if (pr.second) q.push(pr.second);
                }
            }
        }

        // Delete the node itself
        delete node;
    }

    this->root = nullptr;
}
