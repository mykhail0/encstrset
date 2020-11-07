//Uwagi:
//Const w arg-ach i w zwracanych typach

#include <unordered_set>
#include <unordered_map>
#include <iostream>

using encstrset = std::unordered_set<std::string>;
using set_map = std::unordered_map<unsigned long, encstrset>;

static unsigned long largest_id;

set_map m_set_map = set_map();

//TODO wyświetlanie w zależności od debug (poczytać o paczkach c++)
//TODO namespace jnp1 przy kompilacji w c++

//TODO opakować m_set_map w funkcję (zmienna globalna, initialization order fiasco)
//TODO iostream i zmienne globalne cerr i cout (peczar na labach mówił że też mogą być jakieś problemy


std::string cypher(const char *key, const char *value);

/*
      Jeżeli istnieje zbiór o identyfikatorze id i element value po
      zaszyfrowaniu kluczem key nie należy do tego zbioru, to dodaje ten
      zaszyfrowany element do zbioru, a w przeciwnym przypadku nie robi nic.
      Szyfrowanie jest symetryczne, za pomocą operacji bitowej XOR. Gdy klucz
      key jest krótszy od value, to należy go cyklicznie powtórzyć. Wynikiem
      jest true, gdy element został dodany, a false w przeciwnym przypadku.
*/
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

/*
      Jeżeli istnieje zbiór o identyfikatorze id i element value zaszyfrowany
      kluczem key należy do tego zbioru, to usuwa element ze zbioru, a w
      przeciwnym przypadku nie robi nic. Wynikiem jest true, gdy element został
      usunięty, a false w przeciwnym przypadku.
*/
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

/*
      Jeżeli istnieje zbiór o identyfikatorze id i element value zaszyfrowany
      kluczem key należy do tego zbioru, to zwraca true, a w przeciwnym
      przypadku zwraca false.
*/
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
/*
    Jeżeli istnieją zbiory o identyfikatorach src_id oraz dst_id, to kopiuje
    zawartość zbioru o identyfikatorze src_id do zbioru o identyfikatorze
    dst_id, a w przeciwnym przypadku nic nie robi.
*/
void encstrset_copy(unsigned long src_id, unsigned long dst_id) {
    auto src_it = m_set_map.find(src_id);
    if (src_it == m_set_map.end())
        return;
    auto dst_it = m_set_map.find(dst_id);
    if (dst_it == m_set_map.end())
        return;

    add_all(src_it->second, dst_it->second);
}

