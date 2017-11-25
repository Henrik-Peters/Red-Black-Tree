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

bool randomRemove(int amount) {
    IntTree* tree = new IntTree();
    int numbers[amount];
    
    for (int i = 0; i < amount; i++) {
        numbers[i] = i;
    }
    
    random_shuffle(numbers, numbers+amount);

    for (int i = 0; i < amount; i++) {
        tree->insert(numbers[i]);
    }

    for (int i = 0; i < 10; i++) {
        random_shuffle(numbers, numbers+amount);
    }

    for (int i = 0; i < amount; i++) {
        tree->remove(numbers[i]);
        AssertTrue(tree->invariant());
        AssertFalse(tree->contains(numbers[i]));
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
        }},
        {"Removing the root node", []() {
            IntTree* tree = new IntTree();
            tree->insert(5);
            tree->remove(5);
            AssertTrue(tree->invariant());

            AssertFalse(tree->contains(5));
            AssertEquals("empty tree", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Removing a red leaf [0 childs]", []() {
            IntTree* tree = new IntTree();
            tree->insert(5);
            tree->insert(1);
            tree->insert(7);

            tree->remove(7);
            AssertTrue(tree->invariant());

            AssertFalse(tree->contains(7));
            AssertTrue(tree->contains(5));
            AssertTrue(tree->contains(1));
            AssertEquals("└── 5 (B)\n    └── 1 (R)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Removing a black node with red child [1 child]", []() {
            IntTree* tree = new IntTree();
            tree->insert(5);
            tree->insert(1);
            tree->insert(7);
            tree->insert(3);

            tree->remove(1);
            AssertTrue(tree->invariant());

            AssertFalse(tree->contains(1));
            AssertTrue(tree->contains(5));
            AssertTrue(tree->contains(7));
            AssertTrue(tree->contains(3));
            AssertEquals("└── 5 (B)\n    ├── 3 (B)\n    └── 7 (B)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Removing black leaf [0 childs]", []() {
            IntTree* tree = new IntTree();
            tree->insert(1);
            tree->insert(2);
            tree->insert(3);
            tree->insert(4);

            //remove 3 to get a completly black tree
            tree->remove(3);
            AssertTrue(tree->invariant());

            tree->remove(4);
            AssertTrue(tree->invariant());

            AssertFalse(tree->contains(3));
            AssertFalse(tree->contains(4));
            AssertTrue(tree->contains(1));
            AssertTrue(tree->contains(2));
            AssertEquals("└── 2 (B)\n    └── 1 (R)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Removing node with 1 child [red child]", []() {
            IntTree* tree = new IntTree();
            tree->insert(1);
            tree->insert(2);
            tree->insert(3);
            tree->insert(4);

            tree->remove(3);
            AssertTrue(tree->invariant());

            AssertFalse(tree->contains(3));
            AssertTrue(tree->contains(1));
            AssertTrue(tree->contains(2));
            AssertTrue(tree->contains(4));
            AssertEquals("└── 2 (B)\n    ├── 1 (B)\n    └── 4 (B)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Removing node with 2 childs [root node]", []() {
            IntTree* tree = new IntTree();
            tree->insert(1);
            tree->insert(2);
            tree->insert(3);
            tree->insert(4);

            //remove 4 to get black childs for the root
            tree->remove(4);
            AssertTrue(tree->invariant());

            tree->remove(2);
            AssertTrue(tree->invariant());

            AssertFalse(tree->contains(4));
            AssertFalse(tree->contains(2));
            AssertTrue(tree->contains(1));
            AssertTrue(tree->contains(3));
            AssertEquals("└── 3 (B)\n    └── 1 (R)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Removing adjust case 1 [0 childs, root node]", []() {
            IntTree* tree = new IntTree();
            tree->insert(7);
            tree->remove(7);
            AssertTrue(tree->invariant());

            AssertFalse(tree->contains(7));
            AssertEquals("empty tree", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Removing adjust case 2 [0 childs, left node]", []() {
            IntTree* tree = new IntTree();
            tree->insert(3);
            tree->insert(2);
            tree->insert(5);
            tree->insert(7);
            tree->insert(8);
            tree->insert(9);

            tree->remove(2);
            AssertTrue(tree->invariant());

            AssertFalse(tree->contains(2));
            AssertTrue(tree->contains(3));
            AssertTrue(tree->contains(5));
            AssertTrue(tree->contains(7));
            AssertTrue(tree->contains(8));
            AssertTrue(tree->contains(9));
            AssertEquals("└── 7 (B)\n    ├── 3 (B)\n    │   └── 5 (R)\n    └── 8 (B)\n        └── 9 (R)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Removing adjust case 2 [0 childs, right node]", []() {
            IntTree* tree = new IntTree();
            tree->insert(8);
            tree->insert(6);
            tree->insert(7);
            tree->insert(1);
            tree->insert(4);
            tree->insert(3);

            //same case when deleting the 7 (is root with 2 childs)
            tree->remove(8);
            AssertTrue(tree->invariant());

            AssertFalse(tree->contains(8));
            AssertTrue(tree->contains(6));
            AssertTrue(tree->contains(7));
            AssertTrue(tree->contains(1));
            AssertTrue(tree->contains(4));
            AssertTrue(tree->contains(3));
            AssertEquals("└── 4 (B)\n    ├── 1 (B)\n    │   └── 3 (R)\n    └── 7 (B)\n        └── 6 (R)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Removing adjust case 3 [0 childs, symmetric]", []() {
            IntTree* tree = new IntTree();
            tree->insert(2);
            tree->insert(19);
            tree->insert(3);
            tree->insert(6);
            tree->insert(7);
            tree->insert(10);
            tree->insert(11);
            tree->insert(18);
            tree->insert(17);
            tree->insert(20);

            //same case when deleting the 3 (with 2 childs)
            tree->remove(6);
            AssertTrue(tree->invariant());

            AssertFalse(tree->contains(6));
            AssertTrue(tree->contains(2));
            AssertTrue(tree->contains(19));
            AssertTrue(tree->contains(3));
            AssertTrue(tree->contains(7));
            AssertTrue(tree->contains(10));
            AssertTrue(tree->contains(11));
            AssertTrue(tree->contains(18));
            AssertTrue(tree->contains(17));
            AssertTrue(tree->contains(20));

            delete tree;
            TestPassed;
        }},
        {"Removing adjust case 4 [0 childs, symmetric]", []() {
            IntTree* tree = new IntTree();
            tree->insert(7);
            tree->insert(8);
            tree->insert(3);
            tree->insert(4);
            tree->insert(5);
            tree->insert(2);

            tree->remove(8);
            AssertTrue(tree->invariant());

            AssertFalse(tree->contains(8));
            AssertTrue(tree->contains(7));
            AssertTrue(tree->contains(3));
            AssertTrue(tree->contains(4));
            AssertTrue(tree->contains(5));
            AssertTrue(tree->contains(2));
            AssertEquals("└── 4 (B)\n    ├── 3 (B)\n    │   └── 2 (R)\n    └── 7 (B)\n        └── 5 (R)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Removing adjust case 5 [0 childs, right sibling child red]", []() {
            IntTree* tree = new IntTree();
            tree->insert(5);
            tree->insert(1);
            tree->insert(7);
            tree->insert(2);

            tree->remove(7);
            AssertTrue(tree->invariant());

            AssertFalse(tree->contains(7));
            AssertTrue(tree->contains(5));
            AssertTrue(tree->contains(1));
            AssertTrue(tree->contains(2));
            AssertEquals("└── 2 (B)\n    ├── 1 (B)\n    └── 5 (B)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Removing adjust case 5 [0 childs, left sibling child red]", []() {
            IntTree* tree = new IntTree();
            tree->insert(5);
            tree->insert(1);
            tree->insert(10);
            tree->insert(7);
            tree->insert(12);
            tree->insert(11);

            tree->remove(7);
            AssertTrue(tree->invariant());

            AssertFalse(tree->contains(7));
            AssertTrue(tree->contains(5));
            AssertTrue(tree->contains(1));
            AssertTrue(tree->contains(10));
            AssertTrue(tree->contains(12));
            AssertTrue(tree->contains(11));
            AssertEquals("└── 5 (B)\n    ├── 1 (B)\n    └── 11 (R)\n        ├── 10 (B)\n        └── 12 (B)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Removing adjust case 6 [0 childs, left node]", []() {
            IntTree* tree = new IntTree();
            tree->insert(5);
            tree->insert(1);
            tree->insert(7);
            tree->insert(8);
            tree->insert(9);
            tree->insert(10);

            tree->remove(7);
            AssertTrue(tree->invariant());

            AssertFalse(tree->contains(7));
            AssertTrue(tree->contains(5));
            AssertTrue(tree->contains(1));
            AssertTrue(tree->contains(8));
            AssertTrue(tree->contains(9));
            AssertTrue(tree->contains(10));
            AssertEquals("└── 5 (B)\n    ├── 1 (B)\n    └── 9 (R)\n        ├── 8 (B)\n        └── 10 (B)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Removing adjust case 6 [0 childs, right node]", []() {
            IntTree* tree = new IntTree();
            tree->insert(5);
            tree->insert(3);
            tree->insert(7);
            tree->insert(2);

            tree->remove(7);
            AssertTrue(tree->invariant());

            AssertFalse(tree->contains(7));
            AssertTrue(tree->contains(5));
            AssertTrue(tree->contains(3));
            AssertTrue(tree->contains(2));
            AssertEquals("└── 3 (B)\n    ├── 2 (B)\n    └── 5 (B)\n", tree->toString());

            delete tree;
            TestPassed;
        }},
        {"Removing 20 elements (random)", []() {
            return randomRemove(20);
        }},
        {"Removing 50 elements (random)", []() {
            return randomRemove(50);
        }},
        {"Removing 100 elements (random)", []() {
            return randomRemove(100);
        }},
        {"Removing 1000 elements (random)", []() {
            return randomRemove(1000);
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