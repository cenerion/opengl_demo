#pragma once

#include <istream>
#include <glm/glm.hpp>

std::istream& operator>>(std::istream& is, glm::vec3& v){
    is >> v.x >> v.y >> v.z;
    return is;
}


std::istream& operator>>(std::istream& is, glm::vec2& v){
    is >> v.x >> v.y;
    return is;
}