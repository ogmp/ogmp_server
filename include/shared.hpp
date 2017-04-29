#ifndef HTTP_SHARED_HPP
#define HTTP_SHARED_HPP

#include <string>
#include <vector>
#include <map>

typedef std::vector<std::string> string_vector;
typedef std::map<std::string, std::string> string_map;
typedef std::vector<string_map> string_map_vector;

#define _awake 0
#define _unconscious 1
#define _dead 2

#endif // HTTP_SHARED_HPP
