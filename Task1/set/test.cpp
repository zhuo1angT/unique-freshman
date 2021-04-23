#include <cstdio>
#include <iostream>
#include <set>

#include "Set.h"
#include "chen.h"
#include "gtest/gtest.h"

Set<int> stdSet;
unique::set<int> stuSet;

const int SIZE = 600000;

TEST(IsEmptyTest, HandlerTrueReturn) {
  ASSERT_TRUE(stdSet.empty() == stuSet.empty())
      << "You didn't pass the empty test";
}

TEST(InsertTest, HandlerTrueReturn) {
  for (int i = 0; i < SIZE; i++) {
    int k = rand();
    stdSet.insert(k);
    stuSet.insert(k);
  }
  ASSERT_TRUE(stdSet.size() == stuSet.size())
      << "You didn't pass the insert test";
}

TEST(CountAndEraseTest, HandlerTrueReturn) {
  int size = stuSet.size();
  for (auto s : stdSet) {
    ASSERT_TRUE(stuSet.count(s) == 1) << "You didn't pass the count test";
    stuSet.erase(s);
    size--;
    ASSERT_TRUE(stuSet.size() == (unsigned long int)size)
        << "You didn't pass the erase test";
  }
}

TEST(ClearTest, HandlerTrueReturn) {
  stuSet.clear();
  stdSet.clear();
  ASSERT_TRUE(stuSet.size() == stdSet.size())
      << "You didn't pass the clear test";
}
/*
TEST(AdvancedTest, HandlerTrueReturn) {
    for (int lp = 0; lp < 2; lp++) {
        int minn = 0x7fffffff, maxx = 0;
        for (int i = 1; i < SIZE; i++) {
            int k = rand();
            minn = k > minn ? minn : k;
            maxx = k > maxx ? k : maxx;
            stdSet.insert(k);
            stuSet.insert(k);
        }
        // Find test
        std::set<int>::iterator stdit;
        unique::set<int>::iterator it;
        for (auto s : stdSet) {
            it = stuSet.find(s);
            ASSERT_TRUE(s == *it) << "You didn't pass the find test";
        }

        // upper lower test
        for (int i = 0; i < SIZE; i++) {
            int k = rand();
            k = k % (maxx - minn + 1) + minn;
            assert(k >= minn && k <= maxx);
            stdit = stdSet.upper_bound(k);
            it = stuSet.upper_bound(k);
            ASSERT_TRUE(*stdit == *it)
                << "You didn't pass the upper_bound test";
            stdit = stdSet.lower_bound(k);
            it = stuSet.lower_bound(k);
            ASSERT_TRUE(*stdit == *it)
                << "You didn't pass the lower_bound test";
        }

        // operator test
        for (int i = 0; i < 100; i++) {
            int k = rand();
            k = k % (maxx - minn + 1) + minn;
            stdit = stdSet.lower_bound(k);
            it = stuSet.lower_bound(k);
            int t = 0;
            while (*it != maxx && t < 100) {
                t++;
                stdit++;
                it++;
                ASSERT_TRUE(*stdit == *it) << "You didn't pass the ++ test";
            }
            while (t--) {
                stdit--;
                it--;
                ASSERT_TRUE(*stdit == *it) << "You didn't pass the -- test";
            }
            unique::set<int>::iterator it2 = it;
            ASSERT_TRUE(*it2 == *it) << "You didn't pass the = test";
        }
        stdSet.clear();
        stuSet.clear();
    }
}
*/
int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  srand(time(NULL));
  return RUN_ALL_TESTS();
}
