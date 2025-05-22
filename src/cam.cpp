#include <filesystem>
#include <fstream>
#include <string>
#include <glm/glm.hpp>

#include "utils/glm_stream.hpp"
#include "camera.hpp"

namespace fs = std::filesystem;

using namespace glm;



/*
6.53786 1.75545 7.4606 //pos
-37.7672 -2.87113 -89.1084 //m_target
1920 1080 // res
-73.3434 18.8818 -73.8281 //corner
0.0366474 6.76664e-10 -0.0168137 // dU
0.000731464 -0.0402822 0.0015943 // dV
0.908901 1.85391e-08 -0.417002
-0.0181411 0.99905 -0.0395406
0.0341928
*/

struct cam{
    vec3 pos;       //position
    vec3 target;  //target position
    vec2 res;       //framebuffer resolution
    vec3 corner;    //position of left top corner of plane
    vec3 dU;        //pixel offset in U direction
    vec3 dV;        //pixel offset in V direction

    Camera to_camera()
    {
        Camera camera;
        camera.set_position(pos);
        camera.set_target(target);

        auto aspect_ratio = res.x / res.y;

        auto top_center = corner + (dU * 0.5f * res.x);
        auto down_center = top_center + (dV * res.y);

        auto a = pos - top_center;
        auto b = pos - down_center;

        auto cos_theta = dot(a,b);
        auto angle = acos(cos_theta);

        camera.set_fov(angle);

        return camera;
    }
};

std::istream& operator>>(std::istream& is, cam& v){
    is >> v.pos >> v.target >> 
          v.res >> v.corner >> 
          v.dU >> v.dV;
    return is;
}


cam parse(fs::path filename)
{
    if(!fs::exists(filename)){
        throw std::runtime_error("file not found");
    }
    auto f_size = fs::file_size(filename);

    std::ifstream file(filename);

    if(!file.is_open()){
        throw std::runtime_error("file couldn't be open");
    }


    return cam{};
}