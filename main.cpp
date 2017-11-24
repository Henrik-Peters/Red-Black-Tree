// ---------------------------------------------------------------------
// MIT License
// Copyright (c) 2017 Henrik Peters
// See LICENSE file in the project root for full license information.
// ---------------------------------------------------------------------
#include<iostream>
#include <iomanip>
#include <algorithm>

#ifndef DEBUG
#define DEBUG
#endif

#include "rbtree.h"
using namespace std;

#define TestPassed {return true;}
#define AssertEquals(exp, act) ({if(exp!=act) {cerr << endl << "Asseration failed: Expected <" << exp << "> but was <" << act << "> "; return false;}})
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