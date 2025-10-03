#include <unrolled_list.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <algorithm>
#include <initializer_list>
#include <vector>
#include <list>

/*
    В данном файле представлен ряд тестов, где используются (вместе, раздельно и по-очереди):
        - push_back
        - push_front
        - insert
    Методы применяются на unrolled_list и на std::list.
    Ожидается, что в итоге порядок элементов в контейнерах будут идентичен
*/

TEST(UnrolledLinkedList, pushBack) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 1000; ++i) {
        std_list.push_back(i);
        unrolled_list.push_back(i);
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(UnrolledLinkedList, pushFront) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 1000; ++i) {
        std_list.push_front(i);
        unrolled_list.push_front(i);
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(UnrolledLinkedList, pushMixed) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 1000; ++i) {
        if (i % 2 == 0) {
            std_list.push_front(i);
            unrolled_list.push_front(i);
        } else {
            std_list.push_back(i);
            unrolled_list.push_back(i);
        }
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(UnrolledLinkedList, insertAndPushMixed) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 1000; ++i) {
        if (i % 3 == 0) {
            std_list.push_front(i);
            unrolled_list.push_front(i);
        } else if (i % 3 == 1) {
            std_list.push_back(i);
            unrolled_list.push_back(i);
        } else {
            auto std_it = std_list.begin();
            auto unrolled_it = unrolled_list.begin();
            std::advance(std_it, std_list.size() / 2);
            std::advance(unrolled_it, std_list.size() / 2);
            std_list.insert(std_it, i);
            unrolled_list.insert(unrolled_it, i);
        }
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(UnrolledLinkedList, popFrontBack) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 1000; ++i) {
        std_list.push_back(i);
        unrolled_list.push_back(i);
    }

    for (int i = 0; i < 500; ++i) {
        if (i % 2 == 0) {
            std_list.pop_back();
            unrolled_list.pop_back();
        } else {
            std_list.pop_front();
            unrolled_list.pop_front();
        }
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));

    for (int i = 0; i < 500; ++i) {
        std_list.pop_back();
        unrolled_list.pop_back();
    }

    ASSERT_TRUE(unrolled_list.empty());
}

TEST(UnrolledLinkedList, AssignByTwoIterators1) {
    std::list<int> l = {1, 2, 3, 4, 5, 6, 7};
    unrolled_list<int> ul(8, 10);
    ul.assign(l.begin(), std::find(l.begin(), l.end(), 5));
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 2, 3, 4));
}

TEST(UnrolledLinkedList, AssignByTwoIterators2) {
    std::list<int> l = {1, 2, 3, 4, 5, 6, 7};
    unrolled_list<int, 2> ul(8, 10);
    ul.assign(l.begin(), std::find(l.begin(), l.end(), 5));
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 2, 3, 4));
}

TEST(UnrolledLinkedList, AssignByInitList1) {
    unrolled_list<int> ul(17, -1);
    ul.assign({1, 2, 3, 4});
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 2, 3, 4)); 
}

TEST(UnrolledLinkedList, AssignByInitList2) {
    unrolled_list<int> ul(5, -1);
    ul.assign({1, 2, 3, 4});
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 2, 3, 4)); 
}

TEST(UnrolledLinkedList, AssignBySizeAndValue1) {
    unrolled_list<int> ul(17, -1);
    ul.assign(5, 1);
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 1, 1, 1, 1));
}

TEST(UnrolledLinkedList, AssignByInitList) {
    unrolled_list<int, 3> ul(17, -1);
    ul.assign({1, 2, 3, 4});
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 2, 3, 4)); 
}

TEST(UnrolledLinkedList, InsertByValue1) {
    unrolled_list<int> ul(7, 1);
    auto it = ul.begin();
    ul.insert(it++, 2);
    ASSERT_THAT(ul, ::testing::ElementsAre(2, 1, 1, 1, 1, 1, 1, 1));
    ++it; 
    ++it;
    ul.insert(it++, 3);
    ASSERT_THAT(ul, ::testing::ElementsAre(2, 1, 1, 3, 1, 1, 1, 1, 1));
}

TEST(UnrolledLinkedList, InsertByValue2) {
    unrolled_list<int, 7> ul(7, 1);
    auto it = ul.begin();
    ul.insert(it++, 2);
    ASSERT_THAT(ul, ::testing::ElementsAre(2, 1, 1, 1, 1, 1, 1, 1));
    ++it; 
    ++it;
    ul.insert(it++, 3);
    ASSERT_THAT(ul, ::testing::ElementsAre(2, 1, 1, 3, 1, 1, 1, 1, 1));
}

TEST(UnrolledLinkedList, InsertByValue3) {
    unrolled_list<int, 3> ul(7, 1);
    auto it = ul.begin();
    ul.insert(it++, 2);
    ASSERT_THAT(ul, ::testing::ElementsAre(2, 1, 1, 1, 1, 1, 1, 1));
    ++it; 
    ++it;
    ul.insert(it, 3);
    ASSERT_THAT(ul, ::testing::ElementsAre(2, 1, 1, 3, 1, 1, 1, 1, 1));
}

TEST(UnrolledLinkedList, InsertByValueAtEnd1) {
    unrolled_list<int, 3> ul(5, 1);
    ul.insert(ul.end(), 7);
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 1, 1, 1, 1, 7));
}

TEST(UnrolledLinkedList, InsertByValueAtEnd2) {
    unrolled_list<int> ul(5, 1);
    ul.insert(ul.end(), 7);
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 1, 1, 1, 1, 7));
}

TEST(UnrolledLinkedList, InsertBySizeAndValue1) {
    unrolled_list<int> ul(7, 1);
    auto it = ul.begin();
    ++it;
    ++it;
    ++it;
    ul.insert(it, 3, -1);
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 1, 1, -1, -1, -1, 1, 1, 1, 1));
}

TEST(UnrolledLinkedList, InsertBySizeAndValue2) {
    unrolled_list<int, 3> ul(7, 1);
    auto it = ul.begin();
    ++it;
    ++it;
    ++it;
    ul.insert(it, 3, -1);
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 1, 1, -1, -1, -1, 1, 1, 1, 1));
}

TEST(UnrolledLinkedList, InsertBySizeAndValue3) {
    unrolled_list<int, 12> ul(7, 1);
    auto it = ul.begin();
    ++it;
    ++it;
    ++it;
    ul.insert(it, 3, -1);
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 1, 1, -1, -1, -1, 1, 1, 1, 1));
}

TEST(UnrolledLinkedList, InsertByInitializerList1) {
    unrolled_list<int> ul(7, 1);
    auto it = ul.begin();
    ++it;
    ++it;
    ++it;
    ul.insert(it, {2, 3, 4});
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 1, 1, 2, 3, 4, 1, 1, 1, 1));
}

TEST(UnrolledLinkedList, InsertByInitializerList2) {
    unrolled_list<int, 3> ul(7, 1);
    auto it = ul.begin();
    ++it;
    ++it;
    ++it;
    ul.insert(it, {2, 3, 4});
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 1, 1, 2, 3, 4, 1, 1, 1, 1));
}

TEST(UnrolledLinkedList, InsertByInitializerList3) {
    unrolled_list<int, 12> ul(7, 1);
    auto it = ul.begin();
    ++it;
    ++it;
    ++it;
    ul.insert(it, {2, 3, 4});
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 1, 1, 2, 3, 4, 1, 1, 1, 1));
}

TEST(UnrolledLinkedList, InsertByEmptyInitializerList) {
    unrolled_list<int> ul(4, 5);
    auto it = ul.begin();
    ++it;
    ul.insert(it, std::initializer_list<int>{});
    ASSERT_THAT(ul, ::testing::ElementsAre(5, 5, 5, 5));
}

TEST(UnrolledLinkedList, EraseBackElements) {
    unrolled_list<int, 3> ul = {1, 2, 3, 4, 5, 6, 7};
    std::list<int> l = {1, 2, 3, 4, 5, 6, 7};
    for (size_t i = 0; i < 7; ++i) {
        l.erase(--l.end());
        ul.erase(--ul.end());
        ASSERT_THAT(l, ::testing::ElementsAreArray(ul));
    }
}

TEST(UnrolledLinkedList, EraseFrontElements) {
    unrolled_list<int, 3> ul = {1, 2, 3, 4, 5, 6, 7};
    std::list<int> l = {1, 2, 3, 4, 5, 6, 7};
    for (size_t i = 0; i < 7; ++i) {
        l.erase(l.begin());
        ul.erase(ul.begin());
        ASSERT_THAT(l, ::testing::ElementsAreArray(ul));
    }
}

TEST(UnrolledLinkedList, InsertAfterErase1) {
    unrolled_list<int> ul = {1, 2, 3, 4, 5};
    ul.insert(ul.erase(std::find(ul.begin(), ul.end(), 4)), 6);
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 2, 3, 6, 5));
}

TEST(UnrolledLinkedList, InsertAfterErase2) {
    unrolled_list<int, 2> ul = {1, 2, 3, 4, 5};
    ul.insert(ul.erase(std::find(ul.begin(), ul.end(), 3)), 6);
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 2, 6, 4, 5));
}

TEST(UnrolledLinkedList, InsertAtBegin) {
    unrolled_list<int> ul = {1, 2, 3, 4, 5, 6, 7};
    ul.insert(ul.begin(), 10);
    ASSERT_THAT(ul, ::testing::ElementsAre(10, 1, 2, 3, 4, 5, 6, 7));
}

TEST(UnrolledLinkedList, InsertAtEmptyList) {
    unrolled_list<int> ul;
    ul.insert(ul.begin(), 11);
    ASSERT_THAT(ul, ::testing::ElementsAre(11));
}

TEST(UnrolledLinkedList, InsertByTwoIterators1) {
    unrolled_list<int> ul = {1, 2, 3, 4, 5, 6};
    std::list<int> l = {7, 2, 3};
    ul.insert(++ul.begin(), l.begin(), l.end());
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 7, 2, 3, 2, 3, 4, 5, 6));
}

TEST(UnrolledLinkedList, InsertByTwoIterators2) {
    unrolled_list<int, 2> ul = {1, 2, 3, 4, 5, 6};
    std::list<int> l = {7, 2, 3};
    ul.insert(++ul.begin(), l.begin(), l.end());
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 7, 2, 3, 2, 3, 4, 5, 6));
}

TEST(UnrolledLinkedList, InsertByTwoIterators3) {
    unrolled_list<int, 1> ul = {1, 2, 3, 4, 5, 6};
    std::list<int> l = {7, 2, 3};
    ul.insert(++ul.begin(), l.begin(), l.end());
    ASSERT_THAT(ul, ::testing::ElementsAre(1, 7, 2, 3, 2, 3, 4, 5, 6));
}


TEST(PushOperations, PushFront) {
    unrolled_list<int> ul;
    for (size_t i = 0; i < 5; ++i) {
        ul.push_front(i);
    }
    ASSERT_THAT(ul, ::testing::ElementsAre(4, 3, 2, 1, 0));
}

TEST(PushOperations, PushBack) {
    unrolled_list<int> ul;
    for (size_t i = 0; i < 5; ++i) {
        ul.push_back(i);
    }
    ASSERT_THAT(ul, ::testing::ElementsAre(0, 1, 2, 3, 4));
}

TEST(PopOperations, PopBack1) {
    unrolled_list<int> ul = {1, 2, 3, 4, 5, 6, 7, 8};
    std::list<int> l = {1, 2, 3, 4, 5, 6, 7, 8};
    for (int i = 0; i < 8; ++i) {
        ul.pop_back();
        l.pop_back();
        ASSERT_THAT(l, ::testing::ElementsAreArray(ul));
    }
}

TEST(PopOperations, PopBack2) {
    unrolled_list<int, 3> ul = {1, 2, 3, 4, 5, 6, 7, 8};
    std::list<int> l = {1, 2, 3, 4, 5, 6, 7, 8};
    for (int i = 0; i < 8; ++i) {
        ul.pop_back();
        l.pop_back();
        ASSERT_THAT(l, ::testing::ElementsAreArray(ul));
    }
}

TEST(PopOperations, PopFront1) {
    unrolled_list<int> ul = {1, 2, 3, 4, 5, 6, 7, 8};
    std::list<int> l = {1, 2, 3, 4, 5, 6, 7, 8};
    for (int i = 0; i < 8; ++i) {
        ul.pop_front();
        l.pop_front();
        ASSERT_THAT(l, ::testing::ElementsAreArray(ul));
    }
}

TEST(PopOperations, PopFront2) {
    unrolled_list<int, 3> ul = {1, 2, 3, 4, 5, 6, 7, 8};
    std::list<int> l = {1, 2, 3, 4, 5, 6, 7, 8};
    for (int i = 0; i < 8; ++i) {
        ul.pop_front();
        l.pop_front();
        ASSERT_THAT(l, ::testing::ElementsAreArray(ul));
    }
}

TEST(PopAndPushOperations, PopAndPush) {
    unrolled_list<int> ul;
    std::list<int> l;
    for (int i = 0; i < 25; ++i) {
        ul.push_back(i);
        l.push_back(i);
        ASSERT_THAT(l, ::testing::ElementsAreArray(ul));
    }
    for (int i = 0; i < 25; ++i) {
        ul.push_front(i);
        l.push_front(i);
        ASSERT_THAT(l, ::testing::ElementsAreArray(ul));
    }
    for (int i = 0; i < 25; ++i) {
        ul.pop_back();
        l.pop_back();
        ASSERT_THAT(l, ::testing::ElementsAreArray(ul));
    }
    for (int i = 0; i < 25; ++i) {
        ul.pop_front();
        l.pop_front();
        ASSERT_THAT(l, ::testing::ElementsAreArray(ul));
    }
}

