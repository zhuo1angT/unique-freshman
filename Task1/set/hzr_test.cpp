#include <ctime>
#include <iostream>
#include <random>
#include <set>
#include <vector>

#include "Set.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;

const int SIZE = 10000000;

const double p1 = 0.3l;
const double p2 = 0.8l;

const int MAX_RAND = 0xFFFFF;
const int x = RAND_MAX;

int Rand() { return (rand() % 16) | ((rand() % (1 << 16)) << 4); }

int main() {
    srand(time(NULL));

    unique::set<long long> stu_set;
    std::set<long long> std_set;
    int cnt = 0;
    for (int i = 0; i < SIZE; i++) {
        int rand_num = Rand();
        int rand_chk = Rand();
        if (rand_chk < MAX_RAND * p1) {
            std_set.insert(rand_num);
            stu_set.insert(rand_num);
        } else if (rand_chk > MAX_RAND * p2) {
            for (int j = 0; j < 10; j++) {
                int rand_num = Rand();
                if (!std_set.count(rand_num)) {
                    continue;
                }
                stu_set.erase(rand_num);
                std_set.erase(rand_num);
            }
        } else {
            if (stu_set.size() != std_set.size()) {
                cout << i << endl;
                cout << cnt << endl;
                cerr << "Error: size number wrong!" << endl;
                cerr << "Test failed" << endl;
                exit(1);
            }
            if (stu_set.count(rand_num) != std_set.count(rand_num)) {
                cout << i << endl;
                cout << cnt << endl;
                cerr << "Error: count function wrong!" << endl;
                cerr << "Test failed" << endl;
                exit(1);
            }
        }
    }
    cout << "Test passed!" << endl;

    return 0;
}