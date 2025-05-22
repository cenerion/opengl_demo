#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
#include <set>
#include <pair>
#include <glm/glm.hpp>
#include "utils/glm_stream.hpp"

using namespace glm;

/*
jak powinna działać kamera? transformacje mogą być luźne czy muszą być przez macierze?
co pisać własnego a co mogę z libki

*/

void foo()
{
    std::ifstream file;

    std::vector<vec3> positions;
    std::vector<vec3> normals;

    std::set<std::pair<unsigned int, unsigned int>> vertices;
    std::vector<std::tuple<unsigned int, unsigned int, unsigned int>> faces;


    std::string line;
    while(file >> line){
        if(0 == line.compare("v"))
        {
            vec3 v;
            file >> v;
            positions.emplace_back(v);
        }
        else if (0 == line.compare("vn"))
        {
            vec3 v;
            file >> v;
            normals.emplace_back(v);
        }
        else if (0 == line.compare("f"))
        {
            unsigned int v1,v2,v3, n1,n2,n3;
            char tmp;
            std::

        }
        else if (0 == line.compare("g"))
        {
        }
        

    }

}