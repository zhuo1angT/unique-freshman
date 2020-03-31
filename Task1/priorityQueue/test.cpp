#include<queue>
#include<random>
#include<cstdio>
#include<vector>
#include<ctime>
#include<gtest/gtest.h>
#include"PriorityQueue.h"

const int SIZE = 6000000;

TEST(AllTests, HandlerTrueReturn) {
    std::priority_queue<int, std::vector<int>, std::greater<int> > stdQueue;
    PriorityQueue<int> stuQueue;
    ASSERT_TRUE(stdQueue.empty()==stuQueue.empty()) << "You didn't pass the empty test";
    for(int i = 0; i < SIZE; i++) {
        int k = rand() * (rand()>>1);
        stdQueue.push(k);
        stuQueue.push(k);
    }
    ASSERT_TRUE(stdQueue.size()==stuQueue.size()) << "You didn't pass the push test";
    for(int i = 0; i < SIZE; i++) {
        int k = stdQueue.top();
        ASSERT_TRUE(stuQueue.top()==k) << "You didn't pass the top test";
        stdQueue.pop();
        stuQueue.pop();
        ASSERT_TRUE(stdQueue.size()==stuQueue.size()) << "You didn't pass the pop test";
    }
    ASSERT_TRUE(stdQueue.empty()==stuQueue.empty()) << "You didn't pass the empty test";
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    srand(time(NULL));
    return RUN_ALL_TESTS();
}
