#include "encstrset.h"
#include <iostream>

int main() {
    char x[] = "wartość";
    char y[] = "klucz";
    std::cout << cypher(y, x) << std::endl;
    return 0;
}
