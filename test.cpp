#include "pch.h"
#include "../triplePlus/Tritset.cpp"

TEST(TestCaseName, TestName) {
    //резерв памяти для хранения 1000 тритов
    Tritset set(1000);
    // length of internal array
    unsigned allocLength = set.capacity();
    EXPECT_TRUE(allocLength >= 1000 * 2 / 8 / sizeof(unsigned));
    // 1000*2 - min bits count
    // 1000*2 / 8 - min bytes count
    // 1000*2 / 8 / sizeof(unsigned) - min unsigned[] size

    //не выделяет никакой памяти
    set[1000000000] = Unknown;
    EXPECT_TRUE(allocLength == set.capacity());

    // false, but no exception or memory allocation
    if (set[2000000000] == True) {}
    EXPECT_TRUE(allocLength == set.capacity());

    //выделение памяти
    set[1000000000] = True;
    EXPECT_TRUE(allocLength < set.capacity());

    //no memory operations
    allocLength = set.capacity();
    set[1000000000] = Unknown;
    set[1000000] = False;
    EXPECT_TRUE(allocLength == set.capacity());

    //освобождение памяти до начального значения или
    //до значения необходимого для хранения последнего установленного трита
    //в данном случае для трита 1000’000
    set.shrink();
    EXPECT_TRUE(allocLength > set.capacity());
}

/*
    //освобождение памяти до начального значения или
    //до значения необходимого для хранения последнего установленного трита
    //в данном случае для трита 1000’000
    set.shrink();
    EXPECT_TRUE(allocLength > set.capacity());
*/
