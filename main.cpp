#include "Tritset.h"

int main() {

    //Tritset setA(1000);
    //Tritset setB(2000);
    //Tritset setC = setA & setB;

    //if (setC.capacity() == setB.capacity()) return 0;
    //else return -1;


    Tritset set(1000);
    // length of internal array
    unsigned allocLength = set.capacity();
    // 1000*2 - min bits count
    // 1000*2 / 8 - min bytes count
    // 1000*2 / 8 / sizeof(unsigned) - min unsigned[] size

    //не выделяет никакой памяти
    set[1000000000] = Unknown;

    // false, but no exception or memory allocation
    if (set[2000000000] == True) {}

    //выделение памяти
    set[1000000000] = True;

    //no memory operations
    allocLength = set.capacity();
    set[1000000000] = Unknown;
    set[1000000] = False;

    //освобождение памяти до начального значения или
    //до значения необходимого для хранения последнего установленного трита
    //в данном случае для трита 1000’000
    set.shrink();
    if (allocLength > set.capacity()) return 0;
    else return -1;

	return 0;
}