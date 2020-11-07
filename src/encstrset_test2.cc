#include "encstrset.h"

#ifdef NDEBUG
    #undef NDEBUG
#endif

#include <cassert>

namespace {
    unsigned long test() {
        unsigned long id = encstrset_new();
        encstrset_insert(id, "testowy", "1538221");
        return id;
    }

    unsigned long id = test();
}

int main() {
    encstrset_insert(id, nullptr, nullptr);
    encstrset_insert(id, nullptr, "ABC");
    assert(encstrset_test(id, "testowy", "1538221"));
    assert(!encstrset_test(id, "Testowy", "1538221"));
    assert(!encstrset_test(id, "testowy", ""));
    encstrset_delete(id);
}
