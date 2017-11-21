// ---------------------------------------------------------------------
// MIT License
// Copyright (c) 2017 Henrik Peters
// See LICENSE file in the project root for full license information.
// ---------------------------------------------------------------------
#include<iostream>

#ifndef DEBUG
#define DEBUG
#endif

#include "rbtree.h"
using namespace std;

int main() {
    int a = 1;
    int b = 5;
    int c = 7;
    int d = 9;
    
    RBTree<int> tree;

    cout << "Insert: 7" << endl;
    tree.insert(&c);
    cout << "Inv: " << tree.invariant() << endl;
    tree.dumpTree();

    cout << "Insert: 9" << endl;
    tree.insert(&d);
    cout << "Inv: " << tree.invariant() << endl;
    tree.dumpTree();

    cout << "Insert: 1" << endl;
    tree.insert(&a);
    cout << "Inv: " << tree.invariant() << endl;
    tree.dumpTree();

    cout << "Insert: 5" << endl;
    tree.insert(&b);
    cout << "Inv: " << tree.invariant() << endl;
    tree.dumpTree();
}