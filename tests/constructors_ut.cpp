#include <unrolled_list.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <algorithm>
#include <initializer_list>
#include <list>

TEST(IsCorrectConstructor, SizeAndValue1) {
    unrolled_list<int, 3> ul(7, 3);
    std::list<int> l(7, 3);
    ASSERT_THAT(l, ::testing::ElementsAreArray(ul));
}

TEST(IsCorrectConstructor, SizeAndValue2) {
    unrolled_list<int, 10> ul(7, 3);
    std::list<int> l(7, 3);
    ASSERT_THAT(l, ::testing::ElementsAreArray(ul));
}

TEST(IsCorrectConstructor, SizeAndValue3) {
    unrolled_list<char, 17> ul(100000, 'c');
    std::list<char> l(100000, 'c');
    ASSERT_THAT(l, ::testing::ElementsAreArray(ul));
}

TEST(IsCorrectConstructor, DefaultConstructor) {
    unrolled_list<int> ul;
    ASSERT_EQ(ul.size(), 0);
    ASSERT_EQ(ul.empty(), true);
}

TEST(IsCorrectConstructor, TwoInputIterators1) {
    std::list<int> l = {1, 2, 3, 4, 5, 6, 7};
    unrolled_list<int, 2> ul(l.begin(), std::find(l.begin(), l.end(), 7));
    ASSERT_EQ(ul.size(), 6);
    l.pop_back();
    ASSERT_THAT(l, ::testing::ElementsAreArray(ul));
}

TEST(IsCorrectConstructor, TwoInputIterators2) {
    std::list<int> l = {1, 2, 3, 4, 5, 6, 7};
    unrolled_list<int, 10> ul(l.begin(), std::find(l.begin(), l.end(), 7));
    ASSERT_EQ(ul.size(), 6);
    l.pop_back();
    ASSERT_THAT(l, ::testing::ElementsAreArray(ul));
}

TEST(IsCorrectConstructor, InitializerList) {
    std::initializer_list<int> init_list = {0, -1, 1, -2, 2, -3, 3, -4, 4, -5, 5};
    std::list<int> l = init_list;
    unrolled_list<int, 3> ul1 = init_list;
    unrolled_list<int, 2> ul2 = init_list;
    unrolled_list<int, 12> ul3 = init_list;
    unrolled_list<int, 7> ul4 = init_list;

    ASSERT_THAT(l, ::testing::ElementsAreArray(ul1));
    ASSERT_THAT(ul1, ::testing::ElementsAreArray(ul2));
    ASSERT_THAT(ul2, ::testing::ElementsAreArray(ul3));
    ASSERT_THAT(ul3, ::testing::ElementsAreArray(ul4));
}

TEST(IsCorrectConstructor, SizeAndAllocator1) {
    unrolled_list<int> ul(5, std::allocator<int>());
    ASSERT_THAT(ul, ::testing::ElementsAre(0, 0, 0, 0, 0));
}

TEST(IsCorrectConstructor, SizeAndAllocator2) {
    unrolled_list<int, 3> ul(7, std::allocator<int>());
    ASSERT_THAT(ul, ::testing::ElementsAre(0, 0, 0, 0, 0, 0, 0));
}

TEST(IsCorrectConstructor, TwoIteratorsAndAllocator) {
    std::list<int> l = {1, 2, 3, 4, 5, 6, 7};
    unrolled_list<int, 1> ul1(l.begin(), l.end(), std::allocator<int>());
    ASSERT_THAT(l, ::testing::ElementsAreArray(ul1));

    unrolled_list<int, 2> ul2(l.begin(), l.end(), std::allocator<int>());
    ASSERT_THAT(l, ::testing::ElementsAreArray(ul2));
    unrolled_list<int, 10> ul3(l.begin(), l.end(), std::allocator<int>());
    ASSERT_THAT(l, ::testing::ElementsAreArray(ul3));
}

TEST(IsCorrectConstructor, CopyConstructor1) {
    unrolled_list<int> ul1 = {1, 2, 3, 4, 5, 6}; 
    unrolled_list<int> ul2(ul1);
    ASSERT_THAT(ul2, ::testing::ElementsAre(1, 2, 3, 4, 5, 6));
}

TEST(IsCorrectConstructor, CopyConstructor2) {
    unrolled_list<int, 2> ul1 = {1, 2, 3, 4, 5, 6}; 
    unrolled_list<int, 2> ul2(ul1);
    ASSERT_THAT(ul2, ::testing::ElementsAre(1, 2, 3, 4, 5, 6));
}

TEST(IsCorrectAssignmentOperator, AssignmentOperator1) {
    unrolled_list<int> ul1 = {1, 2, 3, 4, 5, 6};
    unrolled_list<int> ul2 = ul1;
    ASSERT_THAT(ul2, ::testing::ElementsAreArray(ul1));
}

TEST(IsCorrectAssignmentOperator, AssignmentOperator2) {
    unrolled_list<int, 2> ul1 = {1, 2, 3, 4, 5, 6, 7};
    unrolled_list<int, 2> ul2 = ul1;
    ASSERT_THAT(ul2, ::testing::ElementsAreArray(ul1));
}

