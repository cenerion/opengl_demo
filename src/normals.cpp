#include <span>
#include <iostream>
#include <glm/glm.hpp>
#include <format>
#include <regex>

#include "cube.hpp"

using namespace glm;

void lul(){
    std::span buf = std::span(cube::vertices);

    vec3 p = vec3(0.0f);
    for(int i = 0; i < buf.size(); i+=3){
        vec3 v = vec3(buf[i], buf[i+1], buf[i+2]);
        vec3 n = normalize(v);

        std::cout << std::format("\t{:.4f}, {:.4f}, {:.4f}, {:.4f}, {:.4f}, {:.4f}\n",
            v.x, v.y, v.z,
            n.x, n.y, n.z
        );
    }
}

void trans_indices()
{

    auto idx = cube::indices;
    for(int i = 0; i < countof(cube::indices); i += 3){
        std::cout << std::format("{}, {}, {},\n", idx[i]-1, idx[i+1]-1, idx[i+2]-1);
    }
}


int main(){
    std::string text = "f 1//2 3//4 5/6/7";

    
    std::regex pattern("(\\d+)/(\\d+)?/(\\d+) (\\d+)/(\\d+)?/(\\d+) (\\d+)/(\\d+)?/(\\d+)");
    std::smatch match;

    bool found = std::regex_search(text, match, pattern);
    std::cout << "found? " << found << "\n";

    for(auto sub : match){
        
        std::println(std::cout, "submatch: {} {}", sub.str(), sub.matched);
    }

}