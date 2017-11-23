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
    RBTree<int>* tree = new RBTree<int>();
    int numbers[] = {1, 6, 8, 11, 13, 15, 17, 22, 25, 27, 32, 33};

    for (unsigned int i = 0; i < sizeof(numbers)/sizeof(numbers[0]); i++) {
        cout << "Insert: " << numbers[i] << endl;
        tree->insert(numbers[i]);
        cout << "Inv: " << tree->invariant() << endl;
        cout << tree->toString();
    }
}