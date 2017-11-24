// ---------------------------------------------------------------------
// MIT License
// Copyright (c) 2017 Henrik Peters
// See LICENSE file in the project root for full license information.
// ---------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <algorithm>

#ifndef DEBUG
#define DEBUG
#endif

#include "rbtree.h"
using namespace std;

#define TestPassed {return true;}
#define AssertEquals(exp, act) ({\
    if(exp!=act) {\
        cerr << endl << "Asseration failed: Expected <" << exp << "> but was <" << act << "> ";\
        return false;\
    }\
})
#define AssertTrue(x) ({if(!x) {return false;}})
#define AssertFalse(x) ({if(x) {return false;}})

typedef RBTree<int> IntTree;
typedef enum TestResult {
    SUCCESS = 0,
    FAILED = 1,
    UNTESTED = 2
} TestResult;

typedef bool (*TestFunc)();
int TestCounter = 1;

//Test case definition
class Test {
private:
    unsigned int id;
    string description;
    TestResult result;
    TestFunc test;

public:
    Test(string description, TestFunc test);
    void run();

    inline unsigned int getID() { return id; };
    inline unsigned int getResult() { return result; };
    inline string getDescription() { return description; };
};

Test::Test(string description, TestFunc test) {
    this->id = TestCounter++;
    this->description = description;
    this->result = UNTESTED;
    this->test = test;
}

void Test::run() {
    this->result = this->test() ? SUCCESS : FAILED;
}

//Test helper functions
bool sortedInsert(int amount) {
    IntTree* tree = new IntTree();
    
    for (int i = 0; i < amount; i++) {
        tree->insert(i);
        AssertTrue(tree->invariant());
    }
    
    for (int i = 0; i < amount; i++) {
        AssertTrue(tree->contains(i));
    }
    
    delete tree;
    TestPassed;
}

bool randomInsert(int amount, bool checkContains = true, bool invariantAfterInsert = true) {
    IntTree* tree = new IntTree();
    int numbers[amount];
    
    for (int i = 0; i < amount; i++) {
        numbers[i] = i;
    }
    
    random_shuffle(numbers, numbers+amount);

    for (int i = 0; i < amount; i++) {
        tree->insert(numbers[i]);

        if (invariantAfterInsert) {
            AssertTrue(tree->invariant());
        }
    }

    if (checkContains) {
        for (int i = 0; i < amount; i++) {
            AssertTrue(tree->contains(i));
        }
    }
    
    //Check the invariant only for the final tree
    if (!invariantAfterInsert) {
        AssertTrue(tree->invariant());
    }
    
    delete tree;
    TestPassed;
}

int main() {
    Test testSuite[] = {
        {"Inserting 1 element into empty tree", []() {
            IntTree* tree = new IntTree();
            tree->insert(5);

            AssertTrue(tree->invariant());
            AssertEquals(true, tree->contains(5));
            AssertEquals("└── 5 (B)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Inserting 2 elements", []() { 
            IntTree* tree = new IntTree();
            tree->insert(5);
            AssertTrue(tree->invariant());

            tree->insert(7);
            AssertTrue(tree->invariant());

            AssertTrue(tree->contains(5));
            AssertTrue(tree->contains(7));
            AssertEquals("└── 5 (B)\n    └── 7 (R)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Inserting 3 elements", []() {
            IntTree* tree = new IntTree();
            tree->insert(5);
            AssertTrue(tree->invariant());

            tree->insert(7);
            AssertTrue(tree->invariant());

            tree->insert(3);
            AssertTrue(tree->invariant());

            AssertTrue(tree->contains(5));
            AssertTrue(tree->contains(7));
            AssertTrue(tree->contains(3));
            AssertEquals("└── 5 (B)\n    ├── 3 (R)\n    └── 7 (R)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Inserting adjust case 1", []() {
            //case: current node is the root
            IntTree* tree = new IntTree();
            tree->insert(1);
            AssertTrue(tree->invariant());

            tree->insert(3);
            AssertTrue(tree->invariant());

            tree->insert(4);
            AssertTrue(tree->invariant());

            tree->insert(2);
            AssertTrue(tree->invariant());

            AssertTrue(tree->contains(1));
            AssertTrue(tree->contains(3));
            AssertTrue(tree->contains(4));
            AssertTrue(tree->contains(2));
            AssertEquals("└── 3 (B)\n    ├── 1 (B)\n    │   └── 2 (R)\n    └── 4 (B)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Inserting adjust case 2", []() {
            //case: the parent is black
            IntTree* tree = new IntTree();
            tree->insert(2);
            AssertTrue(tree->invariant());

            tree->insert(4);
            AssertTrue(tree->invariant());

            tree->insert(1);
            AssertTrue(tree->invariant());

            AssertTrue(tree->contains(2));
            AssertTrue(tree->contains(4));
            AssertTrue(tree->contains(1));
            AssertEquals("└── 2 (B)\n    ├── 1 (R)\n    └── 4 (R)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Inserting adjust case 3", []() {
            //case: parent and uncle are both red
            IntTree* tree = new IntTree();
            tree->insert(5);
            AssertTrue(tree->invariant());

            tree->insert(2);
            AssertTrue(tree->invariant());

            tree->insert(7);
            AssertTrue(tree->invariant());

            tree->insert(1);
            AssertTrue(tree->invariant());

            AssertTrue(tree->contains(5));
            AssertTrue(tree->contains(2));
            AssertTrue(tree->contains(7));
            AssertTrue(tree->contains(1));
            AssertEquals("└── 5 (B)\n    ├── 2 (B)\n    │   └── 1 (R)\n    └── 7 (B)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Inserting adjust case 4 [right child, no uncle]", []() {
            //case: parent red and no uncle (black) with node as right child
            IntTree* tree = new IntTree();
            tree->insert(3);
            AssertTrue(tree->invariant());

            tree->insert(1);
            AssertTrue(tree->invariant());

            tree->insert(7);
            AssertTrue(tree->invariant());

            tree->insert(9);
            AssertTrue(tree->invariant());

            tree->insert(8);
            AssertTrue(tree->invariant());

            AssertTrue(tree->contains(3));
            AssertTrue(tree->contains(1));
            AssertTrue(tree->contains(7));
            AssertTrue(tree->contains(9));
            AssertTrue(tree->contains(8));
            AssertEquals("└── 3 (B)\n    ├── 1 (B)\n    └── 8 (B)\n        ├── 7 (R)\n        └── 9 (R)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Inserting adjust case 4 [right child, with uncle]", []() {
            //case: parent red and uncle (black) with node as right child
            IntTree* tree = new IntTree();
            tree->insert(5);
            AssertTrue(tree->invariant());

            tree->insert(10);
            AssertTrue(tree->invariant());

            tree->insert(6);
            AssertTrue(tree->invariant());

            tree->insert(17);
            AssertTrue(tree->invariant());

            tree->insert(18);
            AssertTrue(tree->invariant());

            tree->insert(7);
            AssertTrue(tree->invariant());

            tree->insert(8);
            AssertTrue(tree->invariant());

            //only here uncle is not null
            tree->insert(14);
            AssertTrue(tree->invariant());

            AssertTrue(tree->contains(5));
            AssertTrue(tree->contains(10));
            AssertTrue(tree->contains(6));
            AssertTrue(tree->contains(17));
            AssertTrue(tree->contains(18));
            AssertTrue(tree->contains(7));
            AssertTrue(tree->contains(8));
            AssertTrue(tree->contains(14));
            AssertEquals("└── 8 (B)\n    ├── 6 (R)\n    │   ├── 5 (B)\n    │   └── 7 (B)\n    └── 17 (R)\n        ├── 10 (B)\n        │   └── 14 (R)\n        └── 18 (B)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Inserting adjust case 5 [left child, no uncle]", []() {
            //case: parent red and no uncle (black) with node as left child
            IntTree* tree = new IntTree();
            tree->insert(5);
            AssertTrue(tree->invariant());

            tree->insert(3);
            AssertTrue(tree->invariant());

            tree->insert(7);
            AssertTrue(tree->invariant());

            tree->insert(1);
            AssertTrue(tree->invariant());

            tree->insert(2);
            AssertTrue(tree->invariant());

            AssertTrue(tree->contains(5));
            AssertTrue(tree->contains(3));
            AssertTrue(tree->contains(7));
            AssertTrue(tree->contains(1));
            AssertTrue(tree->contains(2));
            AssertEquals("└── 5 (B)\n    ├── 2 (B)\n    │   ├── 1 (R)\n    │   └── 3 (R)\n    └── 7 (B)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Inserting adjust case 5 [left child, with uncle]", []() {
            //case: parent red and uncle (black) with node as left child
            IntTree* tree = new IntTree();
            tree->insert(16);
            AssertTrue(tree->invariant());

            tree->insert(18);
            AssertTrue(tree->invariant());

            tree->insert(19);
            AssertTrue(tree->invariant());

            tree->insert(2);
            AssertTrue(tree->invariant());

            tree->insert(3);
            AssertTrue(tree->invariant());

            tree->insert(8);
            AssertTrue(tree->invariant());

            tree->insert(11);
            AssertTrue(tree->invariant());

            //only here uncle is not null
            tree->insert(15);
            AssertTrue(tree->invariant());

            AssertTrue(tree->contains(16));
            AssertTrue(tree->contains(18));
            AssertTrue(tree->contains(19));
            AssertTrue(tree->contains(2));
            AssertTrue(tree->contains(3));
            AssertTrue(tree->contains(8));
            AssertTrue(tree->contains(11));
            AssertTrue(tree->contains(15));
            AssertEquals("└── 11 (B)\n    ├── 3 (R)\n    │   ├── 2 (B)\n    │   └── 8 (B)\n    └── 18 (R)\n        ├── 16 (B)\n        │   └── 15 (R)\n        └── 19 (B)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Inserting 20 elements (sorted)", []() {
            return sortedInsert(20);
        }},
        {"Inserting 20 elements (random)", []() {
            return randomInsert(20);
        }},
        {"Inserting 50 elements (sorted)", []() {
            return sortedInsert(50);
        }},
        {"Inserting 50 elements (random)", []() {
            return randomInsert(50);
        }},
        {"Inserting 100 elements (random)", []() {
            return randomInsert(100);
        }},
        {"Inserting 1000 elements (random)", []() {
            return randomInsert(1000);
        }},
        {"Inserting 1 Mio elements (random)", []() {
            return randomInsert(1000000, false, false);
        }}
    };

    unsigned int passed = 0;
    unsigned int failed = 0;

    for (unsigned int i = 0; i < sizeof(testSuite)/sizeof(testSuite[0]); i++) {
        Test t = testSuite[i];
        cout << "Running Test " << setfill('0') << setw(2) << t.getID() << ": ";
        t.run();

        if (t.getResult() == FAILED) {
            failed++;
            cout << "\033[1;31mFailed\033[0m";
            cout << " (" << t.getDescription() << ")" << endl;
        } else {
            passed++;
            cout << "\033[1;32mPassed\033[0m";
            cout << " (" << t.getDescription() << ")" << endl;
        }
    }

    unsigned int sucessRate = (passed * 100) / (passed + failed);

    cout << "--------------------" << endl;
    cout << "Tests passed: ";
    if (sucessRate == 100) {
        cout << "\033[1;32m" << sucessRate << "%\033[0m" << endl;
    } else {
        cout << "\033[1;31m" << sucessRate << "%\033[0m" << endl;
    }
    cout << "--------------------" << endl;
    return 0;
}