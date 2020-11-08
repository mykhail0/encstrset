#include "encstrset.h"
#include <unordered_set>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <iomanip>

namespace {
    #ifdef NDEBUG
        constexpr bool debug = false;
    #else
        constexpr bool debug = true;
    #endif

    using encstrset = std::unordered_set<std::string>;
    using set_map = std::unordered_map<unsigned long, encstrset>;

    std::string NULL_STRING() {
        static const std::string null_string("NULL");
        return null_string;
    }

    std::string INSERTED() {
        static const std::string inserted("inserted");
        return inserted;
    }

    std::string WAS_ALREADY_PRESENT() {
        static const std::string was_already_present("was alredy present");
        return was_already_present;
    }

    std::string IS_PRESENT() {
        static const std::string is_present("is present");
        return is_present;
    }

    std::string INVALID_VALUE() {
        static const std::string invalid_value("invalid value");
        return invalid_value;
    }

    std::string CYPHER() {
        static const std::string cypher_str("cypher");
        return cypher_str;
    }

    std::string DOES_NOT_EXIST() {
        static const std::string does_not_exist("does not exist");
        return does_not_exist;
    }

    unsigned long largest_id = 0;

    std::ostream &get_cerr() {
        static std::ios_base::Init init;
        return std::cerr;
    }

    template<typename T>
    void tprintf(T format) {
        get_cerr() << format;
    }

    template<typename T, typename... Targs>
    void tprintf(T value, Targs... Fargs) { // recursive variadic function
        get_cerr() << value << ", ";
        tprintf(Fargs...); // recursive call
    }

    template<typename... Targs>
    void print_brackets(Targs... Fargs) {
        get_cerr() << "(";
        tprintf(Fargs...);
        get_cerr() << ")";
    }

    template<typename ... Targs>
    void print_func_info(const std::string &func_name, Targs... Fargs) {
        get_cerr() << func_name << ": ";
        tprintf(Fargs...);
        get_cerr() << std::endl;
    }

    template<typename... Targs>
    void print_func(const std::string &func_name, Targs... Fargs) {
        get_cerr() << func_name;
        print_brackets(Fargs...);
        get_cerr() << std::endl;
    }

    std::string set_str(unsigned long id) {
        return "set #" + std::to_string(id);
    }

    std::string param_str(const char *p) {
        if (p == nullptr)
            return NULL_STRING();
        return "\"" + std::string(p) + "\"";
    }

    void add_all(const encstrset &src, encstrset &dst) {
        //TODO czy takie iterowanie jest dobre, czy powinno zależeć od debug
        for (auto str : src) {
            if (dst.find(str) == dst.end())
                dst.insert(str);
        }
    }

    set_map &m_set_map() {
        static set_map *m_set_map_ptr = new set_map();
        return *m_set_map_ptr;
    }

    std::string str_to_hex(const std::string& s) {
        std::ostringstream s;
        for (char& c : foo)
            // https://stackoverflow.com/a/3381629
            s << std::hex << std::setfill('0') << std::setw(2) << std::uppercase << (int) c;
        return s.str();
    }

    // Increments pointer to C-string's contents cyclically.
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
        for (char &c : ans) {
            c = c ^ key[ptr];
            ptr = increment_Cstr_ptr(ptr, key);
        }
        return ans;
    }
}

unsigned long jnp1::encstrset_new() {
    unsigned long ans = largest_id;
    ++largest_id;
    m_set_map()[ans] = encstrset();
    return ans;
}

void jnp1::encstrset_delete(unsigned long id) {
    m_set_map().erase(id);
    if (id != 0 && id == largest_id - 1)
        --largest_id;
}

size_t jnp1::encstrset_size(unsigned long id) {
    auto it = m_set_map().find(id);
    if (it == m_set_map().end())
        return 0;

    return it->second.size();
}

bool jnp1::encstrset_insert(unsigned long id, const char *value, const char *key) {
    if (value == nullptr)
        return false;

    auto it = m_set_map().find(id);
    if (it == m_set_map().end())
        return false;

    encstrset &m_set = it->second;
    std::string cyphered_val = cypher(key, value);

    if (m_set.find(cyphered_val) != m_set.end())
        return false;

    m_set.insert(cyphered_val);
    return true;
}

bool jnp1::encstrset_remove(unsigned long id, const char *value, const char *key) {
    if (value == nullptr)
        return false;

    auto it = m_set_map().find(id);
    if (it == m_set_map().end())
        return false;

    std::string cyphered_val = cypher(key, value);

    encstrset &m_set = it->second;

    auto set_it = m_set.find(cyphered_val);
    if (set_it == m_set.end())
        return false;

    m_set.erase(set_it);
    return true;
}

bool jnp1::encstrset_test(unsigned long id, const char *value, const char *key) {
    if (value == nullptr)
        return false;

    auto it = m_set_map().find(id);
    if (it == m_set_map().end())
        return false;

    std::string cyphered_val = cypher(key, value);

    const encstrset &m_set = it->second;
    return m_set.find(cyphered_val) != m_set.end();
}

void jnp1::encstrset_clear(unsigned long id) {
    auto it = m_set_map().find(id);
    if (it == m_set_map().end())
        return;
    it->second.clear();
}

void jnp1::encstrset_copy(unsigned long src_id, unsigned long dst_id) {
    auto src_it = m_set_map().find(src_id);
    if (src_it == m_set_map().end())
        return;
    auto dst_it = m_set_map().find(dst_id);
    if (dst_it == m_set_map().end())
        return;

    add_all(src_it->second, dst_it->second);
}
