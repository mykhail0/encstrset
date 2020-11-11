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
    Creates a new set and returns its id.
*/
    unsigned long encstrset_new();

/*
    If there is a set with id equal to `id`, it is removed,
    otherwise nothing happens.
*/
    void encstrset_delete(unsigned long id);

/*
    If there is a set with id equal to `id`, its size is returned.
    Otherwise returns 0.
*/
    size_t encstrset_size(unsigned long id);

/*
      Jeżeli istnieje zbiór o identyfikatorze id i element value po
      zaszyfrowaniu kluczem key nie należy do tego zbioru, to dodaje ten
      zaszyfrowany element do zbioru, a w przeciwnym przypadku nie robi nic.
      Szyfrowanie jest symetryczne, za pomocą operacji bitowej XOR. Gdy klucz
      key jest krótszy od value, to należy go cyklicznie powtórzyć. Wynikiem
      jest true, gdy element został dodany, a false w przeciwnym przypadku.
*/
    bool encstrset_insert(unsigned long id, const char *value, const char *key);

/*
      Jeżeli istnieje zbiór o identyfikatorze id i element value zaszyfrowany
      kluczem key należy do tego zbioru, to usuwa element ze zbioru, a w
      przeciwnym przypadku nie robi nic. Wynikiem jest true, gdy element został
      usunięty, a false w przeciwnym przypadku.
*/
    bool encstrset_remove(unsigned long id, const char *value, const char *key);

/*
      Jeżeli istnieje zbiór o identyfikatorze id i element value zaszyfrowany
      kluczem key należy do tego zbioru, to zwraca true, a w przeciwnym
      przypadku zwraca false.
*/
    bool encstrset_test(unsigned long id, const char *value, const char *key);

/*
    If there is a set with id equal to `id`, all of its elements are deleted.
    Otherwise nothing happens.
*/
    void encstrset_clear(unsigned long id);

/*
    Jeżeli istnieją zbiory o identyfikatorach src_id oraz dst_id, to kopiuje
    zawartość zbioru o identyfikatorze src_id do zbioru o identyfikatorze
    dst_id, a w przeciwnym przypadku nic nie robi.
*/
    void encstrset_copy(unsigned long src_id, unsigned long dst_id);

#ifdef __cplusplus
    }
}
#endif

#endif /* ENCSTRSET_H */
