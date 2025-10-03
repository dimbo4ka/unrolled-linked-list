#include "unrolled_list.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(DefaultIterators, BeginAndEnd1) {
    unrolled_list<int> ul1 = {1, 2, 3, 4, 5};
    unrolled_list<int> ul2(ul1.begin(), ul1.end());

    ASSERT_THAT(ul2, ::testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(DefaultIterators, BeginAndEnd2) {
    unrolled_list<int> ul1 = {1, 2, 3, 4, 5};
    unrolled_list<int, 2> ul2(ul1.begin(), ul1.end());

    ASSERT_THAT(ul2, ::testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(ReverseIterators, BeginAndEnd1) {
    unrolled_list<int> ul1 = {1, 2, 3, 4, 5};
    unrolled_list<int> ul2(ul1.rbegin(), ul1.rend());

    ASSERT_THAT(ul2, ::testing::ElementsAre(5, 4, 3, 2, 1));
}

TEST(ReverseIterators, BeginAndEnd2) {
    unrolled_list<int> ul1 = {1, 2, 3, 4, 5};
    unrolled_list<int, 2> ul2(ul1.rbegin(), ul1.rend());

    ASSERT_THAT(ul2, ::testing::ElementsAre(5, 4, 3, 2, 1));
}
