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

enum player_variable_type : char{
    crouch = 0,
    jump = 1,
    attack = 2,
    grab = 3,
    item = 4,
    drop = 5,
    blood_damage = 6,
    blood_health = 7,
    block_health = 8,
    temp_health = 9,
    permanent_health = 10,
    blood_amount = 11,
    recovery_time = 12,
    roll_recovery_time = 13,
    knocked_out = 14,
    ragdoll_type = 15,
    blood_delay = 16,
    state = 17,
    cut_throat = 18,
    position_x = 19,
    position_y = 20,
    position_z = 21,
    direction_x = 22,
    direction_z = 23,
    remove_blood = 24
};

#endif // HTTP_SHARED_HPP
