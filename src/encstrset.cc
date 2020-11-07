#include "encstrset.h"

#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <stdexcept>
#include <cassert>

using encstrset = std::unordered_set<std::string>;
using set_map = std::unordered_map<unsigned long, encstrset>;

static unsigned long largest_id;

set_map m_set_map = set_map();

//TODO wyświetlanie w zależności od debug (poczytać o paczkach c++)
//TODO namespace jnp1 przy kompilacji w c++

//TODO opakować m_set_map w funkcję (zmienna globalna, initialization order fiasco)
//TODO iostream i zmienne globalne cerr i cout (peczar na labach mówił że też mogą być jakieś problemy

// Increments pointer to C-string cyclically.
// Highly unsafe, use at your own risk.
size_t increment_Cstr_ptr(size_t ptr, const char *s) {
    ++ptr;
    if (s[ptr] == '\0')
        return 0;
    return ptr;
}

/*
    Parametr value o wartości NULL jest niepoprawny. Z kolei wartość NULL parametru
    key lub pusty napis key oznaczają brak szyfrowania.
    Assumes C-strings are null-terminated.
If the resulting string length would exceed the max_size, a length_error exception is thrown.
A bad_alloc exception is thrown if the function fails when attempting to allocate storage.
*/
std::string cypher(const char *key, const char *value) {
    // Will remove possibly. If removed without being replaced with
    // an equivalent assertion will cause UB.
    if (value == nullptr)
        throw std::invalid_argument("value is null");

    std::string ans(value);
    size_t ptr = 0;
    for (char& c : ans) {
        c = c ^ key[ptr];
        ptr = increment_Cstr_ptr(ptr, key);
    }
    return ans;
}

bool encstrset_insert(unsigned long id, const char *value, const char *key) {
    if (value == nullptr)
        return false;

    auto it = m_set_map.find(id);
    if (it == m_set_map.end())
        return false;

    encstrset &m_set = it->second;
    std::string cyphered_val = cypher(key, value);

    if (m_set.find(cyphered_val) != m_set.end())
        return false;

    m_set.insert(cyphered_val);
    return true;
}

bool encstrset_remove(unsigned long id, const char *value, const char *key) {
    if (value == nullptr)
        return false;

    auto it = m_set_map.find(id);
    if (it == m_set_map.end())
        return false;

    std::string cyphered_val = cypher(key, value);

    encstrset &m_set = it->second;

    auto set_it = m_set.find(cyphered_val);
    if (set_it == m_set.end())
        return false;

    m_set.erase(set_it);
    return true;
}

bool encstrset_test(unsigned long id, const char *value, const char *key) {
    if (value == nullptr)
        return false;

    auto it = m_set_map.find(id);
    if (it == m_set_map.end())
        return false;

    std::string cyphered_val = cypher(key, value);

    const encstrset &m_set = it->second;
    return m_set.find(cyphered_val) != m_set.end();
}

static void add_all(const encstrset &src, encstrset &dst) {
    //TODO czy takie iterowanie jest dobre, czy powinno zależeć od debug
    for (auto str : src) {
        if (dst.find(str) == dst.end())
            dst.insert(str);
    }
}

void encstrset_copy(unsigned long src_id, unsigned long dst_id) {
    auto src_it = m_set_map.find(src_id);
    if (src_it == m_set_map.end())
        return;
    auto dst_it = m_set_map.find(dst_id);
    if (dst_it == m_set_map.end())
        return;

    add_all(src_it->second, dst_it->second);
}

