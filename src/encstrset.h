#ifndef ENCSTRSET_H
#define ENCSTRSET_H

#ifdef __cplusplus
#ifndef NDEBUG

#include <iostream>

#endif

#include <cstddef>

namespace jnp1 {
    extern "C" {
#else
#include <stdbool.h>
#include <stddef.h>
#endif

/*
    Creates a new set and returns its 'id'.
*/
    unsigned long encstrset_new();

/*
    If there is a set with id equal to 'id', it is removed,
    otherwise nothing happens.
*/
    void encstrset_delete(unsigned long id);

/*
    If there is a set with id equal to 'id', its size is returned.
    Otherwise returns 0.
*/
    size_t encstrset_size(unsigned long id);

/*
    If there exists a set with id equal to 'id' and it doesn't contain
    'value' element encrypted using key 'key', function inserts
    encrypted element to the set. Encryption is symmetrical, and is done using
    XOR bitwise operation. If length of 'key' is smaller than length of 'value'
    it is cyclically repeated. Function returs 'true' if element was inserted,
    'false' otherwise.
*/
    bool encstrset_insert(unsigned long id, const char *value, const char *key);

/*
    If there exists a set with id equal to 'id' containing element 'value'
    encrypted with 'key', function removes the element from the set.
    Returns 'true' if the element has been removed, 'false' otherwise.
*/
    bool encstrset_remove(unsigned long id, const char *value, const char *key);

/*
    If there exists a set with id equal to 'id', containing element 'value'
    encrypted with 'key', it returns 'true', otherwise returns 'false'.
*/
    bool encstrset_test(unsigned long id, const char *value, const char *key);

/*
    If there is a set with id equal to 'id', all of its elements are deleted.
    Otherwise nothing happens.
*/
    void encstrset_clear(unsigned long id);

/*
    If there exists a set with id equal to 'src_id' and set with id equal to
    'dst_id', it copies the content of set with id equal to 'src_id' to the one
    with id equal to 'dst_id', otherwise function does nothing.
*/
    void encstrset_copy(unsigned long src_id, unsigned long dst_id);

#ifdef __cplusplus
    }
}
#endif

#endif /* ENCSTRSET_H */
