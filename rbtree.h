// ---------------------------------------------------------------------
// MIT License
// Copyright (c) 2017 Henrik Peters
// See LICENSE file in the project root for full license information.
// ---------------------------------------------------------------------
#ifndef RBTREE_H
#define RBTREE_H

#ifdef DEBUG
#include <assert.h>
#include <iostream>
#include <string>
#include <sstream>
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
            DOUBLE_BLACK = 2,
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

        friend class RBTree<T>;
        friend class iterator;

        #ifdef DEBUG
        bool invariant();
        int invariantBlackNodes();
        void dumpNode(ostream& buffer, const string& prefix, bool lastNode);
        #endif

        inline bool isBlack() const { return (this->color == BLACK); }
        inline void adjustInsert(RBTreeNode* insertNode);
        inline void adjustRemove();
        inline void leftRotate();
        inline void rightRotate();

        RBTreeNode* lookup(const T key);
        bool insert(const T key);
        void remove();
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
    string toString();
    #endif

    class iterator {
        private:
            RBTreeNode* node = nullptr;
            
        public:
            typedef T value_type;
            typedef const T& reference;
            typedef const T* pointer;
            typedef std::input_iterator_tag iterator_category;
            friend class RBTree<T>;
            
            explicit iterator(RBTreeNode* _node) : node(_node) {}
            //implicit copy constructor
            
            iterator& operator++ ();
            inline iterator operator++ (int) {
                iterator it = *this;
                ++(*this);
                return it;
            }
            
            inline bool operator== (const iterator& other) { return node == other.node; }
            inline bool operator!= (const iterator& other) { return !(*this == other); }
            
            inline reference operator* () { return node->key; }
            inline pointer operator-> () { return &node->key; }
    };

    iterator begin();
    iterator end();
};

//Tree nodes
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
    //Adjust the tree after an insertion
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

                } else if (grand->right != NULL && node == grand->right->left) {
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

    //update the parent link
    if (this->right != NULL) {
        this->right->parent = this;
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

    //update the parent link
    if (this->left != NULL) {
        this->left->parent = this;
    }
    
    this->parent = root;

    //set the new root of the tree
    if (root->parent == NULL) {
        tree->root = root;
    }
}

template <typename T>
void RBTree<T>::RBTreeNode::remove() {
    RBTreeNode* node = this;

    if (this->left != NULL && this->right != NULL) {
        //For the 2 child case we will convert the problem into 1 or 0 childs
        //Therefore find the minimum element in the right subtree

        node = this->right;

        while (node->left != NULL) {
            node = node->left;
        }

        //Swap the node values and remove the minimum node
        this->key = node->key;
    }

    //Now we have 1 or 0 childs
    RBTreeNode* child = (node->left == NULL) 
                        ? node->right 
                        : node->left;

    //Detach the node from the tree
    if (node->parent == NULL) {
        node->tree->root = child;

    } else if (node->parent->left == node) {
        node->parent->left = child;

    } else {
        node->parent->right = child;
    }

    //Update the childs parent link
    if (child != NULL) {
        child->parent = node->parent;
    }

    //Red nodes can be deleted without any tree repairing
    if (node->color == BLACK) {
        
        //When the child is red change the color to black
        if (child != NULL && child->color == RED) {
            child->color = BLACK;
            
        } else {
            //Node and the child are both black (that means the child is null)
            //Create a pseudo double black leaf
            child = new RBTreeNode((T)0, node->parent, node->tree, DOUBLE_BLACK);

            //Attach the double black leaf node
            if (node->parent == NULL) {
                node->tree->root = child;

            } else if (node->parent->left == NULL) {
                node->parent->left = child;

            } else {
                node->parent->right = child;
            }

            child->adjustRemove();

            //Detach the pseudo node from the tree
            if (child->parent == NULL) {
                child->tree->root = NULL;

            } else if (child->parent->left == child) {
                child->parent->left = NULL;

            } else {
                child->parent->right = NULL;
            }

            delete child;
        }
    }

    //Prevent the destructor from deleting nodes
    node->left = NULL;
    node->right = NULL;
    delete node;
}

template <typename T>
void RBTree<T>::RBTreeNode::adjustRemove() {
    //Adjust the tree when a node was colored double black
    #ifdef DEBUG
    assert (this->color == DOUBLE_BLACK);
    #endif

    RBTreeNode* node = this;
    
    while (true) {
        if (node->parent == NULL) {
            //node is the root node
            node->color = BLACK;
            return;
        }

        RBTreeNode* parent = node->parent;
        RBTreeNode* sibling = (node == node->parent->left)
                                ? node->parent->right 
                                : node->parent->left;

        //Black parent and red sibling
        if (sibling->color == RED) {
            sibling->color = BLACK;
            parent->color = RED;

            if (node == parent->left) {
                parent->leftRotate();
                sibling = parent->right;

            } else {
                parent->rightRotate();
                sibling = parent->left;
            }
        }

        //Black sibling with black childs
        if (parent->color == BLACK && 
            (sibling->left == NULL || sibling->left->color == BLACK) && 
            (sibling->right == NULL || sibling->right->color == BLACK)) {

                sibling->color = RED;
                node = parent;
                continue;
        }

        //Everything black only the parent is red
        if (parent->color == RED && 
            (sibling->left == NULL || sibling->left->color == BLACK) && 
            (sibling->right == NULL || sibling->right->color == BLACK)) {

                sibling->color = RED;
                parent->color = BLACK;
                return;
        }

        //Black sibling with the siblings left child red
        if (node == parent->left && 
            (sibling->right == NULL || sibling->right->color == BLACK)) {

                sibling->color = RED;
                sibling->left->color = BLACK;
                sibling->rightRotate();
                sibling = sibling->parent;

        //Black sibling with the siblings right child red
        } else if (node == parent->right &&
                    (sibling->left == NULL || sibling->left->color == BLACK)) {

                sibling->color = RED;
                sibling->right->color = BLACK;
                sibling->leftRotate();
                sibling = sibling->parent;
        }

        sibling->color = parent->color;
        parent->color = BLACK;

        if (node == parent->left) {
            parent->leftRotate();
            sibling->right->color = BLACK;

        } else {
            parent->rightRotate();
            sibling->left->color = BLACK;
        }
        return;
    }
}

#ifdef DEBUG
template <typename T>
bool RBTree<T>::RBTreeNode::invariant() {

    //If a node is red then both children are black
    bool invColor = (color == BLACK) || (
        (left == NULL || left->color == BLACK) && 
        (right == NULL || right->color == BLACK)
    );

    //Left nodes have a lower order and right nodes a higher order
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
void RBTree<T>::RBTreeNode::dumpNode(ostream& buffer, const string& prefix, bool lastNode) {
    //print the current element and the children
    buffer << prefix << (lastNode ? "└── " : "├── ") << key << (color == RED ? " (R)" : " (B)") << endl;

    if (left != NULL) {
        left->dumpNode(buffer, prefix + (lastNode ? "    " : "│   "), right == NULL);
    }

    if (right != NULL) {
        right->dumpNode(buffer, prefix + (lastNode ? "    " : "│   "), true);
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
    if (root == NULL) {
        return false;
    } else {
        RBTreeNode* node = root->lookup(key);

        if (node == NULL) {
            return false;
        } else {
            node->remove();
            return true;
        }
    }
}

//iterator
template <typename T>
typename RBTree<T>::iterator& RBTree<T>::iterator::operator++ () {
    //Perform a post-order tree traversal
    RBTreeNode* node = this->node;
    
    //The root node is the last element
    if (node->parent == NULL) {
        this->node = NULL;
        return *this;
    }
    
    //Switch to the right sibling or bubble up in the tree
    if (node == node->parent->left && node->parent->right != NULL) {
        node = node->parent->right;
        
    } else {
        this->node = node->parent;
        return *this;
    }
    
    //Descend to the next leaf node
    while (true) {
        if (node->left != NULL) {
            node = node->left;
            
        } else if (node->right != NULL) {
            node = node->right;
            
        } else {
            this->node = node;
            return *this;
        }
    }
}

template <typename T>
typename RBTree<T>::iterator RBTree<T>::begin() {
    //The first node will be the minimum node
    RBTreeNode* node = root;
    
    while (node->left != NULL) {
        node = node->left;
    }
    
    return iterator(node);
}

template <typename T>
typename RBTree<T>::iterator RBTree<T>::end()   {
    return iterator(NULL);
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
    if (root == NULL) {
        cout << "empty tree";
    } else {
        root->dumpNode(cout, "", true);
    }

    cout << endl;
}

template <typename T>
string RBTree<T>::toString() {
    stringstream buffer;

    if (root == NULL) {
        buffer << "empty tree";
    } else {
        root->dumpNode(buffer, "", true);
    }

    return buffer.str();
}
#endif

#endif /* RBTREE_H */