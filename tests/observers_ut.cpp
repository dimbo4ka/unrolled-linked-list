#include "unrolled_list.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <list>

TEST(UnrolledLinkedList, BackElement) {
    std::list<int> l;
    unrolled_list<int> ul;
    for (size_t i = 0; i < 100; ++i) {
        l.push_back(i);
        ul.push_back(i);
        ASSERT_EQ(l.back(), ul.back());
    }    
    for (size_t i = 0; i < 100; ++i) {
        ASSERT_EQ(l.back(), ul.back());
        l.pop_front();
        ul.pop_front();
    }    
}

TEST(UnrolledLinkedList, FrontElement) {
    std::list<int> l;
    unrolled_list<int> ul;
    for (size_t i = 0; i < 100; ++i) {
        l.push_back(i);
        ul.push_back(i);
        ASSERT_EQ(l.front(), ul.front());
    }    
    for (size_t i = 0; i < 100; ++i) {
        l.pop_front();
        ul.pop_front();
        ASSERT_EQ(l.front(), ul.front());
    }    
}

TEST(UnrolledLinkedList, Size) {
    std::list<int> l;
    unrolled_list<int> ul;
    for (size_t i = 0; i < 100000; ++i) {
        if (i % 3 == 0) {
            ul.push_back(i);
            l.push_back(i);
        } else {
            ul.push_front(i);
            l.push_front(i);
        }
        ASSERT_EQ(ul.size(), l.size());
    }
}

TEST(UnrolledLinkedList, EmptyChecker) {
    std::list<int> l;
    unrolled_list<int> ul;
    for (size_t i = 0; i < 100000; ++i) {
        if (i % 4 == 0) {
            l.push_back(i);
            ul.push_back(i);
        } else if (i % 4 == 1) {
            l.push_front(i);
            ul.push_front(i);
        } else if (i % 4 == 2) {
            l.pop_back();
            ul.pop_back();
        } else if (i % 4 == 3) {
            l.pop_front();
            ul.pop_front();
        }
        ASSERT_EQ(ul.empty(), l.empty());
    }
}

TEST(UnrolledLinkedList, Clear) {
    unrolled_list<int, 9> ul(10000, 1);
    ul.clear();
    ASSERT_EQ(ul.empty(), true);
}

TEST(UnrolledLinkedList, Equals) {
    unrolled_list<int> ul1(999, 1);
    unrolled_list<int> ul2(999, 1);
    unrolled_list<int> ul3(999, 0);
    unrolled_list<int> ul4(1000, 1);

    ASSERT_EQ(ul1, ul2);
    ASSERT_NE(ul1, ul3);
    ASSERT_NE(ul1, ul3);
    ASSERT_NE(ul2, ul3);
    ASSERT_NE(ul2, ul4);
    ASSERT_NE(ul3, ul4);
}

TEST(UnrolledLinkedList, Swap1) {
    unrolled_list<int> ul1(7, 10);
    unrolled_list<int> ul2(11, 7);

    unrolled_list<int> ul3(ul1);
    unrolled_list<int> ul4(ul2);
    ul3.swap(ul4);

    ASSERT_THAT(ul3, ::testing::ElementsAreArray(ul2));
    ASSERT_THAT(ul4, ::testing::ElementsAreArray(ul1));
}

TEST(UnrolledLinkedList, Swap2) {
    unrolled_list<int> ul1;
    unrolled_list<int> ul2(11, 7);

    unrolled_list<int> ul3(ul1);
    unrolled_list<int> ul4(ul2);
    ul3.swap(ul4);

    ASSERT_THAT(ul3, ::testing::ElementsAreArray(ul2));
    ASSERT_THAT(ul4, ::testing::ElementsAreArray(ul1));
}

TEST(UnrolledLinkedList, Swap3) {
    unrolled_list<int> ul1(7, 10);
    unrolled_list<int> ul2;

    unrolled_list<int> ul3(ul1);
    unrolled_list<int> ul4(ul2);
    ul3.swap(ul4);

    ASSERT_THAT(ul3, ::testing::ElementsAreArray(ul2));
    ASSERT_THAT(ul4, ::testing::ElementsAreArray(ul1));
}
