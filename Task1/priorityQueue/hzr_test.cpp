#include <bits/stdc++.h>
#include <sys/time.h>

#include "PriorityQueue.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;

unique::priority_queue<int> stu;
std::priority_queue<int, std::vector<int>, std::greater<int> > sd;

const int N = 10000000;
const double p1 = 0.3l;
const double p2 = 0.8l;

const int MAX_RAND = 0xFFFFF;

int Rand() { return (rand() % 16) | ((rand() % (1 << 16)) << 4); }

int main() {
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        int rand_num = Rand();
        int rand_chk = Rand();
        if (rand_chk < MAX_RAND * p2) {
            stu.push(rand_num);
            sd.push(rand_num);
        } else if (sd.size()) {
            stu.pop();
            sd.pop();
        }
        if (sd.size() == 0) continue;
        if (stu.size() != sd.size() || stu.top() != sd.top()) {
            cerr << "Test failed." << endl;
        }
    }
    cout << "Test passed!" << endl;
}