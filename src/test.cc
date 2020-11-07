#include "encstrset.h"
#include <iostream>

int main() {
    char x[] = "bar";
    char y[] = "5X";
    std::cout << cypher(y, x) << std::endl;
    return 0;
}
