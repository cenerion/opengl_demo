#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
#include <set>
#include <utility>
#include <regex>
#include <glm/glm.hpp>
#include "utils/glm_stream.hpp"
#include "Mesh.hpp"

using namespace glm;

void foo()
{
    std::ifstream file;

    std::vector<Mesh> meshes;
    
    std::vector<float> vertices;
    std::vector<unsigned long> indicies;
    std::vector<vec3> normals;

    constexpr auto INF = std::numeric_limits<float>::quiet_NaN();

    bool is_group = false;

    std::string line;
    while(file >> line){
        if(0 == line.compare("v"))
        {
            vec3 v;
            file >> v;
            vertices.insert(vertices.end(), {v.x, v.y, v.z, INF, INF, INF});
        }
        else if (0 == line.compare("vn"))
        {
            vec3 v;
            file >> v;
            normals.emplace_back(v);
        }
        else if (0 == line.compare("f"))
        {
            std::getline(file, line);
            
            std::regex pattern("(\\d+)/(\\d+)?/(\\d+) (\\d+)/(\\d+)?/(\\d+) (\\d+)/(\\d+)?/(\\d+)");
            std::smatch match;

            bool found = std::regex_search(line, match, pattern);
            unsigned int v1,v2,v3, n1,n2,n3;
            v1 = std::stoul(match[1].str()) -1;
            n1 = std::stoul(match[3].str()) -1;

            v2 = std::stoul(match[4].str()) -1;
            n2 = std::stoul(match[6].str()) -1;

            v3 = std::stoul(match[7].str()) -1;
            n3 = std::stoul(match[9].str()) -1;

            auto check = [&vertices, &normals, INF](unsigned int v, unsigned int n) -> unsigned int
            {
                auto vert = &vertices.data()[v*6]; // ?? .data() or not to data ?
                auto norm = normals[n];
                if(vert[3] == INF){
                    vert[3] = norm.x;
                    vert[4] = norm.y;
                    vert[5] = norm.z;
                }
                else if(!(vert[3] == norm.x && vert[4] == norm.y && vert[5] == norm.z))
                {
                    vertices.insert(vertices.end(), {vert[0], vert[1], vert[2], norm.x, norm.y, norm.z});
                    v = (vertices.size() / 6) -1;
                }

                return v;
            };

            indicies.emplace_back(check(v1,n1));
            indicies.emplace_back(check(v2,n2));
            indicies.emplace_back(check(v3,n3));
        }
        else if (0 == line.compare("g"))
        {//g mesh_0 //before verts
            if(is_group)
            {
                meshes.emplace_back(vertices, indicies);
                vertices.clear();
                indicies.clear();
                normals.clear();
            }
            is_group = true;
        }
        else if(0 == line.compare("usemtl"))
        {//usemtl mat_X3GPL_COL_MAT_fa5d096
            file >> line;
        }
        else
        {
            std::getline(file, line);
            continue;
        }
    }

}