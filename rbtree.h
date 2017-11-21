// ---------------------------------------------------------------------
// MIT License
// Copyright (c) 2017 Henrik Peters
// See LICENSE file in the project root for full license information.
// ---------------------------------------------------------------------
#ifndef RBTREE_H
#define RBTREE_H

#include <assert.h>

#ifdef DEBUG
#include<iostream>
using namespace std;
#endif

template<typename T>
class RBTree {
private:
    //Tree node sub class
    class RBTreeNode {
    private:
        enum Color {
            RED = 0,
            BLACK = 1,
        };

        const T* key;
        Color color;
        RBTreeNode* left;
        RBTreeNode* right;

    public:
        RBTreeNode(const T* key);
        RBTreeNode(const T* key, Color color);
        virtual ~RBTreeNode();

        #ifdef DEBUG
        bool invariant();
        int invariantBlackNodes();
        bool balanced();
        void dumpNode(const string& prefix, bool lastNode);
        #endif

        inline bool isBlack() const { return (this->color == BLACK); }

        RBTreeNode* lookup(const T* key);
        bool insert(const T* key);
        bool remove(const T* key);
    } *root;

public:
    RBTree();
    RBTree(const RBTree& orig);
    virtual ~RBTree();

    bool contains(const T* key);
    bool insert(const T* key);
    bool remove(const T* key);

    #ifdef DEBUG
    bool invariant();
    bool balanced();
    void dumpTree();
    #endif
};

//Tree nodes
template <typename T>
RBTree<T>::RBTreeNode::RBTreeNode(const T* key) {
    this->left = NULL;
    this->right = NULL;
    this->key = key;
    this->color = BLACK;
}

template <typename T>
RBTree<T>::RBTreeNode::RBTreeNode(const T* key, Color color) {
    this->left = NULL;
    this->right = NULL;
    this->key = key;
    this->color = color;
}

template <typename T>
RBTree<T>::RBTreeNode::~RBTreeNode() {
    if (this->left == NULL) {
        delete this->left;
    }

    if (this->right == NULL) {
        delete this->right;
    }
}

template <typename T>
typename RBTree<T>::RBTreeNode* 
         RBTree<T>::RBTreeNode::lookup(const T* key) {

    RBTreeNode* node = this;

    while (node != NULL && node->key != key) {
        node = node->key < key 
                ? node->right 
                : node->left;
    }

    return node;
}

template <typename T>
bool RBTree<T>::RBTreeNode::insert(const T* key) {
    //Find the insertion position
    RBTreeNode* node = this;
    bool nodeInserted = false;

    while (!nodeInserted) {
        if (node->key == key) return false;

        if (node->key < key) {
            if (node->right == NULL) {
                node->right = new RBTreeNode(key, RED);
                nodeInserted = true;
            } else {
                node = node->right;
            }

        } else {
            if (node->left == NULL) {
                node->left = new RBTreeNode(key, RED);
                nodeInserted = true;
            } else {
                node = node->left;
            }
        }
    }

    //TODO repair tree
    return true;
}

template <typename T>
bool RBTree<T>::RBTreeNode::remove(const T* key) {
    return true;
}

#ifdef DEBUG
template <typename T>
bool RBTree<T>::RBTreeNode::invariant() {

    //If a node is red then both children are black
    bool invColor = (color == BLACK) || (
        (left == NULL || left->color == BLACK) && 
        (right == NULL || right->color == BLACK)
    );
    
    //Left nodes have a lower ordner and right nodes a higher order
    bool invOrder = (left == NULL  || left->key  < this->key) && 
                    (right == NULL || right->key > this->key);

    //Every path to a leaf node contains the same number of black nodes
    bool blackNodeCount = invariantBlackNodes();

    return invColor && invOrder && blackNodeCount && 
           (left == NULL || left->invariant()) && 
           (right == NULL || right->invariant());
}

template <typename T>
int RBTree<T>::RBTreeNode::invariantBlackNodes() {
    //Empty Nodes will be treated as black nodes
    int leftCount = (this->left == NULL)
                    ? 1
                    : this->left->invariantBlackNodes();

    int rightCount = (this->right == NULL)
                    ? 1 
                    : this->right->invariantBlackNodes();

    //when the black node count differs -1 will be returned
    return (leftCount == rightCount && leftCount != -1)
           ? leftCount + this->color
           : -1;
}

template <typename T>
bool RBTree<T>::RBTreeNode::balanced() {
    return true;
}

template <typename T>
void RBTree<T>::RBTreeNode::dumpNode(const string& prefix, bool lastNode) {
    //print the current element and the children
    cout << prefix << (lastNode ? "└── " : "├── ") << *key << (color == RED ? " (R)" : " (B)") << endl;

    if (left != NULL) {
        left->dumpNode(prefix + (lastNode ? "    " : "│   "), right == NULL);
    }

    if (right != NULL) {
        right->dumpNode(prefix + (lastNode ? "    " : "│   "), true);
    }
}
#endif


//tree
template <typename T>
RBTree<T>::RBTree() {   
    this->root = NULL;
}

template <typename T>
RBTree<T>::RBTree(const RBTree<T>& orig) {

}

template <typename T>
RBTree<T>::~RBTree() {

}

template <typename T>
bool RBTree<T>::contains(const T* key) {
    return true;
}

template <typename T>
bool RBTree<T>::insert(const T* key) {
    if (root == NULL) {
        root = new RBTreeNode(key);
        return true;
    }

    return root->insert(key);
}

template <typename T>
bool RBTree<T>::remove(const T* key) {
    return true;
}


#ifdef DEBUG
template <typename T>
bool RBTree<T>::invariant() {
    //The root is empty or black
    return root == NULL || (
        root->isBlack() &&
        root->invariant()
    );
}

template <typename T>
bool RBTree<T>::balanced() {
    return true;
}

template <typename T>
void RBTree<T>::dumpTree() {
    //std::cout << std::endl;

    if (root == NULL) {
        cout << "empty tree";
    } else {
        root->dumpNode("", true);
    }

    cout << endl;
}
#endif

#endif /* RBTREE_H */