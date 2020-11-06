//Uwagi:
//Const w arg-ach i w zwracanych typach

using encstrset = std::unordered_set<std::string>;
using set_map = std::unordered_map<unsigned long, encstrset>;

Static unsigned long largest_id;

std::string cypher(const std::string& key, const std::string& value);
