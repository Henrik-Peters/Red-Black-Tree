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

        T key;
        Color color;

        RBTreeNode* parent;
        RBTreeNode* left;
        RBTreeNode* right;
        RBTree<T>* tree;

    public:
        RBTreeNode(const T key, RBTree<T>* tree);
        RBTreeNode(const T key, RBTreeNode* parent, RBTree<T>* tree, Color color);
        virtual ~RBTreeNode();

        #ifdef DEBUG
        bool invariant();
        int invariantBlackNodes();
        void dumpNode(const string& prefix, bool lastNode);
        #endif

        inline bool isBlack() const { return (this->color == BLACK); }
        inline void adjustInsert(RBTreeNode* insertNode);
        inline void leftRotate();
        inline void rightRotate();

        RBTreeNode* lookup(const T key);
        bool insert(const T key);
        bool remove(const T key);
    } *root;

public:
    RBTree();
    virtual ~RBTree();

    bool contains(const T key);
    bool insert(const T key);
    bool remove(const T key);

    #ifdef DEBUG
    bool invariant();
    void dumpTree();
    #endif
};

//Tree Nodes
template <typename T>
RBTree<T>::RBTreeNode::RBTreeNode(const T key, RBTree<T>* tree) {
    this->left = NULL;
    this->right = NULL;
    this->parent = NULL;
    this->key = key;
    this->tree = tree;
    this->color = BLACK;
}

template <typename T>
RBTree<T>::RBTreeNode::RBTreeNode(const T key, RBTreeNode* parent, RBTree<T>* tree, Color color) {
    this->left = NULL;
    this->right = NULL;
    this->parent = parent;
    this->key = key;
    this->tree = tree;
    this->color = color;
}

template <typename T>
RBTree<T>::RBTreeNode::~RBTreeNode() {
    if (this->left != NULL) {
        delete this->left;
    }

    if (this->right != NULL) {
        delete this->right;
    }
}

template <typename T>
typename RBTree<T>::RBTreeNode* 
         RBTree<T>::RBTreeNode::lookup(const T key) {

    RBTreeNode* node = this;

    while (node != NULL && node->key != key) {
        node = node->key < key 
                ? node->right
                : node->left;
    }

    return node;
}

template <typename T>
bool RBTree<T>::RBTreeNode::insert(const T key) {
    //Find the insertion position
    RBTreeNode* node = this;
    bool nodeInserted = false;

    while (!nodeInserted) {
        if (node->key == key) return false;

        if (node->key < key) {
            if (node->right == NULL) {
                node->right = new RBTreeNode(key, node, tree, RED);
                adjustInsert(node->right);
                nodeInserted = true;

            } else {
                node = node->right;
            }

        } else {
            if (node->left == NULL) {
                node->left = new RBTreeNode(key, node, tree, RED);
                adjustInsert(node->left);
                nodeInserted = true;

            } else {
                node = node->left;
            }
        }
    }

    return true;
}

template <typename T>
void RBTree<T>::RBTreeNode::adjustInsert(RBTreeNode* insertNode) {
    //Adjust the tree after an inseration
    RBTreeNode* node = insertNode;

    while (true) {
        if (node->parent == NULL) {
            //node is the root node
            node->color = BLACK;
            return;
    
        } else if (node->parent->color == BLACK) {
            //the black depth is the same on all paths
            return;

        } else {
            #ifdef DEBUG
            //red nodes always have a parent
            assert (node->parent->parent != NULL);

            //the parent of red nodes is always black
            assert (node->parent->parent->color == BLACK);
            #endif

            RBTreeNode* parent = node->parent;
            RBTreeNode* grand = node->parent->parent;
            RBTreeNode* uncle = (grand->left == parent)
                                ? grand->right 
                                : grand->left;

            //when parent and uncle are red swap their color
            //and color the grand parent of this node red
            if (uncle != NULL && uncle->color == RED) {
                parent->color = BLACK;
                uncle->color = BLACK;
                grand->color = RED;

                //adjust the tree for the grand parent
                node = grand;
                continue;

            } else {
                //the parent is red and the uncle is black or empty
                //rotate the parent into the grandparent position

                if (grand->left != NULL && node == grand->left->right) {
                    parent->leftRotate();
                    node = node->left;

                } else if (node == grand->right->left) {
                    parent->rightRotate();
                    node = node->right;
                }
                
                //Update pointers after the rotation
                parent = node->parent;
                grand = node->parent->parent;

                //The node will not be a subtree of the grandparent
                if (node == parent->left) {
                    grand->rightRotate();
                } else {
                    grand->leftRotate();
                }

                parent->color = BLACK;
                grand->color = RED;
            }
        }
    }
}

template <typename T>
void RBTree<T>::RBTreeNode::leftRotate() {
    #ifdef DEBUG
    //the right node will be the new parent
    assert (this->right != NULL);
    #endif

    //rotate the right node to the left
    RBTreeNode* root = this->right;

    this->right = root->left;
    root->left = this;
    root->parent = this->parent;
    
    //update the child link
    if (this->parent != NULL) {
        if (this->parent->left == this) {
            this->parent->left = root;
        } else {
            this->parent->right = root;
        }
    }
    
    this->parent = root;

    //set the new root of the tree
    if (root->parent == NULL) {
        tree->root = root;
    }
}

template <typename T>
void RBTree<T>::RBTreeNode::rightRotate() {
    #ifdef DEBUG
    //the left node will be the new parent
    assert (this->left != NULL);
    #endif

    //rotate the left node to the right
    RBTreeNode* root = this->left;

    this->left = root->right;
    root->right = this;
    root->parent = this->parent;
    
    //update the child link
    if (this->parent != NULL) {
        if (this->parent->left == this) {
            this->parent->left = root;
        } else {
            this->parent->right = root;
        }
    }
    
    this->parent = root;

    //set the new root of the tree
    if (root->parent == NULL) {
        tree->root = root;
    }
}

template <typename T>
bool RBTree<T>::RBTreeNode::remove(const T key) {
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
    bool blackNodeCount = invariantBlackNodes() > -1;

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
void RBTree<T>::RBTreeNode::dumpNode(const string& prefix, bool lastNode) {
    //print the current element and the children
    cout << prefix << (lastNode ? "└── " : "├── ") << key << (color == RED ? " (R)" : " (B)") << endl;

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
RBTree<T>::~RBTree() {
    if (root != NULL) {
        delete root;
    }
}

template <typename T>
bool RBTree<T>::contains(const T key) {
    if (root == NULL) {
        return false;
    } else {
        return root->lookup(key) != NULL;
    }
}

template <typename T>
bool RBTree<T>::insert(const T key) {
    if (root == NULL) {
        root = new RBTreeNode(key, this);
        return true;
    }

    return root->insert(key);
}

template <typename T>
bool RBTree<T>::remove(const T key) {
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