#include "encstrset.h"
#include <unordered_set>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <cstring>

namespace {
#ifdef NDEBUG
    constexpr bool debug = false;
#else
    constexpr bool debug = true;
#endif

// Prints function call for functions that recieve no arguments.
#define FUNC_WITH_NO_ARGS() do {\
    if (debug) {\
      std::cerr << __func__ << "()" << std::endl;\
    }\
  } while (false)

// Prints function call for functions that recieve one argument.
#define FUNC_WITH_ONE_ARG(arg) do {\
    if (debug) {\
      std::cerr << __func__ << "(" << arg << ")" << std::endl;\
    }\
  } while (false)

// Prints function call for functions that recieve two arguments.
#define FUNC_WITH_TWO_ARGS(arg1, arg2) do {\
    if (debug) {\
      std::cerr << __func__ << "(" << arg1 << ", " << arg2 << ")" << std::endl;\
    }\
  } while (false)

// Prints function call for functions that recieve three arguments.
#define FUNC_WITH_THREE_ARGS(arg1, arg2, arg3) do {\
    if (debug) {\
      std::cerr << __func__ << "(" << arg1 << ", " << arg2 << ", " << arg3\
                << ")" << std::endl;\
    }\
  } while (false)

// Prints the state of set_id sate. state describes the state to be printed.
#define STATE_OF_SET(set_id, state) do {\
    if (debug) {\
      std::cerr << __func__ << ": set #" << set_id << " " << state\
                << std::endl;\
    }\
  } while (false)

// Prints the state of cypher in set_id sate.
// state describes the state to be printed.
#define STATE_OF_CYPHER(set_id, cypher, state) do {\
    if (debug) {\
      std::cerr << __func__ << ": set #" << set_id << ", cypher \"" << cypher\
                << "\" "<< state << std::endl;\
    }\
  } while (false)

#define CYPHER_COPIED_PRESENT(func_name, cypher, set_id) do {\
    if (debug) {\
      std::cerr << func_name << ": copied cypher \"" << cypher\
                << "\" was already present in set #" << set_id << std::endl;\
    }\
  } while (false)

#define CYPHER_COPIED(func_name, cypher, set_src, set_dst) do {\
    if (debug) {\
      std::cerr << func_name << ": cypher \"" << cypher\
                << "\" copied from set #" << set_src << " to set #" << set_dst\
                << std::endl;\
    }\
  } while (false)

#define INVALID_VALUE(value) do {\
    if (debug) {\
      std::cerr << __func__ << ": invalid value (" << value << ")"\
                << std::endl;\
    }\
  } while (false)

#define SIZE_OF_SET(set_id, elements) do {\
    if (debug) {\
      std::cerr << __func__ << ": set #" << set_id << " contains " << elements\
                << " element(s)" << std::endl;\
    }\
  } while (false)

    using encstrset = std::unordered_set<std::string>;
    using set_map = std::unordered_map<unsigned long, encstrset>;

    const std::string &NULL_STRING() {
        static const std::string null_string("NULL");
        return null_string;
    }

    // Returns uppercase hex representation of a string
    std::string str_to_hex(const std::string &s) {
        if (s.empty())
            return s;

        std::ostringstream ret;
        for (const char &c : s) {
            // https://stackoverflow.com/a/3381629
            unsigned char c_unsigned = c;
            ret << std::hex << std::setfill('0')
                << std::setw(2) << std::uppercase <<  (unsigned int) c_unsigned << " ";
        }

        std::string str = ret.str();
        return str.erase(str.size() - 1);
    }

    // Global map that stores set's IDs as keys and sets as values.
    set_map &m_set_map() {
        static set_map m_set_map_ptr;
        return m_set_map_ptr;
    }

    /*
        Returns string representation of 'p'.
        If 'p' is a NULL pointer, it returns string 'NULL'.
        Otherwise, it returns string value of 'p' between
        double quotes chars.
     */
    std::string param_str(const char *p) {
        if (p == nullptr)
            return NULL_STRING();
        return "\"" + std::string(p) + "\"";
    }

    // Adds elements from src to dst.
    void add_all(const encstrset &src, encstrset &dst,
                 unsigned long src_id, unsigned long dst_id) {
        static const std::string copy_func_name = "encstrset_copy";
        for (const auto &str : src) {
            std::string hex_val = str_to_hex(str);
            if (dst.insert(str).second)
                CYPHER_COPIED(copy_func_name, hex_val, src_id, dst_id);
            else
                CYPHER_COPIED_PRESENT(copy_func_name, hex_val, dst_id);
        }
    }

    // Increments pointer to C-string's contents cyclically.
    size_t increment_Cstr_ptr(size_t ptr, const char *s) {
        ++ptr;
        if (s[ptr] == '\0')
            return 0;
        return ptr;
    }

    /*
        Parametr value o wartości NULL jest niepoprawny.
        Z kolei wartość NULL parametru
        key lub pusty napis key oznaczają brak szyfrowania.

        Assumes C-strings are null-terminated.
        If the resulting string length would exceed the max_size,
        a length_error exception is thrown.
        A bad_alloc exception is thrown if the function fails
        when attempting to allocate storage.
     */
    std::string cypher(const char *key, const char *value) {
        assert(value != nullptr);
        std::string ans(value);

        // Lazy checking is key here.
        if (key == nullptr || key[0] == '\0')
            return ans;

        size_t ptr = 0;
        for (char &c : ans) {
            c ^= key[ptr];
            ptr = increment_Cstr_ptr(ptr, key);
        }
        return ans;
    }
}

unsigned long jnp1::encstrset_new() {
    static unsigned long largest_free_id = 0;
    FUNC_WITH_NO_ARGS();
    unsigned long ans = largest_free_id;
    ++largest_free_id;
    m_set_map()[ans] = encstrset();

    STATE_OF_SET(ans, "created");
    return ans;
}

void jnp1::encstrset_delete(unsigned long id) {
    FUNC_WITH_ONE_ARG(id);

    if (m_set_map().erase(id)) // Map entry erased.
        STATE_OF_SET(id, "deleted");
    else
        STATE_OF_SET(id, "does not exist");
}

size_t jnp1::encstrset_size(unsigned long id) {
    FUNC_WITH_ONE_ARG(id);
    auto it = m_set_map().find(id);
    if (it == m_set_map().end()) {
        STATE_OF_SET(id, "does not exist");
        return 0;
    }

    auto ans = it->second.size();
    SIZE_OF_SET(id, ans);
    return ans;
}

bool jnp1::encstrset_insert(unsigned long id,
                            const char *value, const char *key) {
    FUNC_WITH_THREE_ARGS(id, param_str(value), param_str(key));

    if (value == nullptr) {
        INVALID_VALUE(param_str(value));
        return false;
    }

    auto it = m_set_map().find(id);
    if (it == m_set_map().end()) {
        STATE_OF_SET(id, "does not exist");
        return false;
    }

    encstrset &m_set = it->second;
    std::string cyphered_val = cypher(key, value);

    std::string hex_val = str_to_hex(cyphered_val);
    if (!m_set.insert(cyphered_val).second) {
        STATE_OF_CYPHER(id, hex_val, "was already present");
        return false;
    }

    STATE_OF_CYPHER(id, hex_val, "inserted");
    return true;
}

bool jnp1::encstrset_remove(unsigned long id,
                            const char *value, const char *key) {
    FUNC_WITH_THREE_ARGS(id, param_str(value), param_str(key));

    if (value == nullptr) {
        INVALID_VALUE(param_str(value));
        return false;
    }

    auto it = m_set_map().find(id);
    if (it == m_set_map().end()) {
        STATE_OF_SET(id, "does not exist");
        return false;
    }

    std::string cyphered_val = cypher(key, value);
    encstrset &m_set = it->second;

    std::string hex_val = str_to_hex(cyphered_val);
    if (!m_set.erase(cyphered_val)) { // There was no cyphered_val in the set.
        STATE_OF_CYPHER(id, hex_val, "was not present");
        return false;
    }

    STATE_OF_CYPHER(id, hex_val, "removed");
    return true;
}

bool jnp1::encstrset_test(unsigned long id,
                          const char *value, const char *key) {
    FUNC_WITH_THREE_ARGS(id, param_str(value), param_str(key));

    if (value == nullptr) {
        INVALID_VALUE(param_str(value));
        return false;
    }

    auto it = m_set_map().find(id);
    if (it == m_set_map().end()) {
        STATE_OF_SET(id, "does not exist");
        return false;
    }

    std::string cyphered_val = cypher(key, value);

    const encstrset &m_set = it->second;
    std::string hex_val = str_to_hex(cyphered_val);
    if (m_set.find(cyphered_val) == m_set.end()) {
        STATE_OF_CYPHER(id, hex_val, "is not present");
        return false;
    }

    STATE_OF_CYPHER(id, hex_val, "is present");
    return true;
}

void jnp1::encstrset_clear(unsigned long id) {
    FUNC_WITH_ONE_ARG(id);
    auto it = m_set_map().find(id);
    if (it == m_set_map().end()) {
        STATE_OF_SET(id, "does not exist");
        return;
    }
    it->second.clear();
    STATE_OF_SET(id, "cleared");
}

void jnp1::encstrset_copy(unsigned long src_id, unsigned long dst_id) {
    FUNC_WITH_TWO_ARGS(src_id, dst_id);

    auto src_it = m_set_map().find(src_id);
    if (src_it == m_set_map().end()) {
        STATE_OF_SET(src_id, "does not exist");
        return;
    }
    auto dst_it = m_set_map().find(dst_id);
    if (dst_it == m_set_map().end()) {
        STATE_OF_SET(dst_id, "does not exist");
        return;
    }

    add_all(src_it->second, dst_it->second, src_id, dst_id);
}
