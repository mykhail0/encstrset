#include "encstrset.h"
#include <unordered_set>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

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

    std::string CYPHER_WAS_PRESENT() {
        static const std::string was_already_present("%: set #%, cypher \"%\" was already present\n");
        return was_already_present;
    }

    std::string CYPHER_WAS_NOT_PRESENT() {
        static const std::string was_already_present("%: set #%, cypher \"%\" was not present\n");
        return was_already_present;
    }

    std::string IS_PRESENT() {
        static const std::string is_present("is present");
        return is_present;
    }

    std::string IS_NOT_PRESENT() {
        static const std::string is_present("is not present");
        return is_present;
    }

    std::string INVALID_VALUE() {
        static const std::string invalid_value("%: invalid value (%)\n");
        return invalid_value;
    }

    std::string CYPHER() {
        static const std::string cypher_str("cypher");
        return cypher_str;
    }

    std::string SET_DOES_NOT_EXIST() {
        static const std::string does_not_exist("%: set #% does not exist \n");
        return does_not_exist;
    }

    std::string CREATED() {
        static const std::string created("created");
    }

    unsigned long largest_id = 0;

    //TODO zastanowić się czy to dobrze
    std::ostream &get_cerr() {
        static std::ios_base::Init init;
        return std::cerr;
    }

    void tprintf(const std::string &format) {
        if (!debug)
            return;
        get_cerr() << format;
    }

    template<typename T, typename... Targs>
    void tprintf(const std::string &format, T value, Targs... Fargs) {
        if (!debug)
            return;
        for (auto it = format.cbegin(); it < format.cend(); it++) {
            if (*it == '%') {
                get_cerr() << value;
                //TODO spytać się o to
                tprintf(&(format[it - format.begin() + 1]), Fargs...); // recursive cal
                return;
            }
            get_cerr() << *it;
        }
    }

    std::string param_str(const char *p) {
        if (p == nullptr)
            return NULL_STRING();
        return "\"" + std::string(p) + "\"";
    }

    std::string str_to_hex(const std::string &s);

    void add_all(const encstrset &src, encstrset &dst, unsigned long src_id, unsigned long dst_id) {
        //TODO czy takie iterowanie jest dobre, czy powinno zależeć od debug
        for (auto str : src) {
            if (dst.find(str) == dst.end()) {
                dst.insert(str);
                tprintf("%: copied cypher \"%\" from set #% to set #%\n",
                        __func__,
                        str_to_hex(str),
                        src_id,
                        dst_id);
            } else {
                tprintf("%: cypher \"%\" was already present in set #%\n",
                        __func__,
                        str_to_hex(str),
                        dst_id);
            }

        }
    }

    set_map &m_set_map() {
        static set_map *m_set_map_ptr = new set_map();
        return *m_set_map_ptr;
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
    if (debug)
        tprintf("%()\n", __func__);
    unsigned long ans = largest_id;
    ++largest_id;
    m_set_map()[ans] = encstrset();

    if (debug)
        tprintf("%: set #%\n", __func__, ans);
    return ans;
}

void jnp1::encstrset_delete(unsigned long id) {
    if (debug)
        tprintf("%(%)\n", __func__, id);
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

    tprintf("%(%, %, %)\n",
            __func__,
            id,
            param_str(value),
            param_str(key));

    if (value == nullptr) {
        tprintf(INVALID_VALUE(), __func__, param_str(value));
        return false;
    }

    auto it = m_set_map().find(id);
    if (it == m_set_map().end()) {
        tprintf(SET_DOES_NOT_EXIST(), __func__, id);
        return false;
    }

    encstrset &m_set = it->second;
    std::string cyphered_val = cypher(key, value);

    if (m_set.find(cyphered_val) != m_set.end()) {
        tprintf(CYPHER_WAS_PRESENT(),
                __func__,
                id,
                str_to_hex(cyphered_val));
        return false;
    }

    m_set.insert(cyphered_val);

    tprintf("%: set #%, cypher \"%\" inserted\n",
            __func__,
            id,
            str_to_hex(cyphered_val));

    return true;
}

bool jnp1::encstrset_remove(unsigned long id, const char *value, const char *key) {
    tprintf("%(%, %, %)\n",
            __func__,
            id,
            param_str(value),
            param_str(key));

    if (value == nullptr) {
        tprintf(INVALID_VALUE(), __func__, param_str(value));
        return false;
    }

    auto it = m_set_map().find(id);
    if (it == m_set_map().end()) {
        tprintf(SET_DOES_NOT_EXIST(), __func__, id);
        return false;
    }

    std::string cyphered_val = cypher(key, value);

    encstrset &m_set = it->second;

    auto set_it = m_set.find(cyphered_val);
    if (set_it == m_set.end()) {
        tprintf(CYPHER_WAS_NOT_PRESENT(), __func__, value, str_to_hex(cyphered_val));
        return false;
    }

    m_set.erase(set_it);

    tprintf("%: set #%, cypher \"%\" removed\n",
            __func__,
            id,
            str_to_hex(cyphered_val));

    return true;
}

bool jnp1::encstrset_test(unsigned long id, const char *value, const char *key) {
    tprintf("%(%, %, %)\n",
            __func__,
            id,
            param_str(value),
            param_str(key));

    if (value == nullptr) {
        tprintf(INVALID_VALUE(), __func__, param_str(value));
        return false;
    }

    auto it = m_set_map().find(id);
    if (it == m_set_map().end()) {
        tprintf(SET_DOES_NOT_EXIST(), __func__, id);
        return false;
    }

    std::string cyphered_val = cypher(key, value);

    const encstrset &m_set = it->second;
    if (m_set.find(cyphered_val) == m_set.end()) {
        tprintf("%: set #%, cypher \"%\" is not present\n",
                __func__,
                id,
                str_to_hex(cyphered_val));
        return false;
    }
    tprintf("%: set #%, cypher \"%\" is present\n",
            __func__,
            id,
            str_to_hex(cyphered_val));
    return true;

}

void jnp1::encstrset_clear(unsigned long id) {
    auto it = m_set_map().find(id);
    if (it == m_set_map().end())
        return;
    it->second.clear();
}

void jnp1::encstrset_copy(unsigned long src_id, unsigned long dst_id) {
    tprintf("%(%, %)\n",
            __func__,
            src_id,
            dst_id);
    auto src_it = m_set_map().find(src_id);
    if (src_it == m_set_map().end()) {
        tprintf(SET_DOES_NOT_EXIST(), __func__, src_id);
        return;
    }
    auto dst_it = m_set_map().find(dst_id);
    if (dst_it == m_set_map().end()) {
        tprintf(SET_DOES_NOT_EXIST(), __func__, dst_id);
        return;
    }

    add_all(src_it->second, dst_it->second, src_id, dst_id);
}
