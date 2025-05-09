#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <array>
#include <fstream>
#include <filesystem>
#include <vector>
#include <span>
#include <format>
#include <unordered_map>

#include "sphere.hpp"
#include "cube.hpp"

using namespace glm;

struct Camera{
    vec3 position;
    vec3 look_at;
    vec3 up_dir;

    float near_plane;
    float far_plane;
    float fov;
    float aspect;

    mat4 view;
    mat4 perspective;

    mat4 calc_view(){
        view = lookAt(position, look_at, up_dir);
        return view;
    }

    mat4 calc_perpective(){
        perspective = glm::perspective(
                    radians(fov), 
                    aspect, 
                    near_plane, 
                    far_plane
                );

        return perspective;
    }
};

static Camera g_camera = {
    vec3(), //position
    vec3(), //look_at
    vec3(), //up_dir

    0.001f,
    100.0f,
    100.0f,
    1.0f,

    mat4(1.0f), //view
    mat4(1.0f), //perspective
};

struct Material{
    vec3 diffuse;
    //float ambient;
    float shininess;
    vec3 specular;
    vec3 luminance;

    void set_uniform(GLuint shader){
        GLuint diff_loc = glGetUniformLocation(shader, "material.diffuse");
        //GLuint ambi_loc = glGetUniformLocation(shader, "material.ambient");
        GLuint shine_loc = glGetUniformLocation(shader, "material.shininess");
        GLuint specular_loc = glGetUniformLocation(shader, "material.specular");
        GLuint lumi_loc = glGetUniformLocation(shader, "material.luminance");

        glUniform1f(shine_loc, shininess);
        //glUniform1f(ambi_loc, ambient);

        glUniform3fv(diff_loc, 1, value_ptr(diffuse));
        glUniform3fv(specular_loc, 1, value_ptr(specular));
        glUniform3fv(lumi_loc, 1, value_ptr(luminance));
        
    }
};


struct Spotlight {
    vec3 pos;
    vec3 dir;
    float cos_a;
    float cos_b;

    vec3 color;
    vec3 attenuation;


    void set_uniform(GLuint shader, int idx){
        auto ye = std::format("light[{}]", idx);

        glUniform3fv(
            glGetUniformLocation(shader, (ye+".pos").c_str()),
            1, value_ptr(pos));

        glUniform3fv(
            glGetUniformLocation(shader, (ye+".dir").c_str()),
            1, value_ptr(dir));

        glUniform3fv(
            glGetUniformLocation(shader, (ye+".color").c_str()),
            1, value_ptr(color));

        glUniform3fv(
            glGetUniformLocation(shader, (ye+".attenuation").c_str()),
            1, value_ptr(attenuation));

        glUniform1f(
            glGetUniformLocation(shader, (ye+".cos_a").c_str()),
            cos_a);

        glUniform1f(
            glGetUniformLocation(shader, (ye+".cos_b").c_str()),
            cos_b);
        }
};


void resize_handler(GLFWwindow* window, int width, int height){
    glViewport(0,0, width, height);
    g_camera.aspect = (float)width / (float)height;
    g_camera.calc_perpective();
}

void input_handler(){

}


void println(const char* str){
    std::cout << str << "\n";
}


GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            // case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            // case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

#define INIT_WIDTH 1800
#define INIT_HEIGHT 1200


class Mesh{
private:
    GLuint VAO = 0;
    unsigned int idx_count = 0;

public:
    void render(){
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, idx_count, GL_UNSIGNED_SHORT, 0);
    }

    Mesh(std::span<const float> vertices, std::span<const unsigned short> indices){
        idx_count = indices.size();

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);  
        glEnableVertexAttribArray(1); 

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float))); 
        
        GLuint EBO;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)* indices.size(), indices.data(), GL_STATIC_DRAW);
    }
};


class Transform : public mat4{
public:
    Transform& translate(const vec3& v){
        (*this) = glm::translate(*this, v);
        return *this;
    }

    Transform& scale(const vec3& v){
        (*this) = glm::scale(*this, v);
        return *this;
    }

    value_type const* value_ptr() const{
        return glm::value_ptr(*((mat4*)this));
    }

    Transform& operator=(const mat4& m){
        mat4::operator=(m);
        return *this;
    }

    mat4 get_mat(){
        return *this;
    }

    using mat4::mat;
};



int main(){
    println("START");
    if(!glfwInit()){
        println("glfw init error");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);

    GLFWwindow* window = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, "opengl testing", NULL, NULL);
    if(!window){
        glfwTerminate();
        println("window creation error");
        return -1;
    }
    glfwMakeContextCurrent(window);


    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        println("OGL loader error");
        return -1;
    }

    glfwSetWindowSizeCallback(window, resize_handler);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    glViewport(0,0, INIT_WIDTH, INIT_HEIGHT);



    //setting up shaders
    GLuint shader = glCreateProgram();
    {
        std::filesystem::path vs_path("./resources/shaders/main.vert");
        std::filesystem::path fs_path("./resources/shaders/main.frag");

        std::ifstream vs_file(vs_path);
        std::string vs_source;
        vs_source.resize(std::filesystem::file_size(vs_path));
        vs_file.read(vs_source.data(), vs_source.size());


        std::ifstream fs_file(fs_path);
        std::string fs_source;
        fs_source.resize(std::filesystem::file_size(fs_path));
        fs_file.read(fs_source.data(), fs_source.size());


        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        auto vs_sources = std::to_array<const char*>({vs_source.data()});
        auto fs_sources = std::to_array<const char*>({fs_source.data()});

        glShaderSource(vs, 1, vs_sources.data() , NULL);
        glShaderSource(fs, 1, fs_sources.data(), NULL);

        glCompileShader(vs);
        glCompileShader(fs);

        glAttachShader(shader, vs);
        glAttachShader(shader, fs);
        glLinkProgram(shader);

        glDeleteShader(vs);
        glDeleteShader(fs);
    }



    //setting up the object

    // Mesh sphere_mesh(sphere::vertices, sphere::indices);
    // Mesh cube_mesh(cube::vertices, cube::indices);


    std::vector<std::pair<Mesh, std::vector<std::pair<Material, std::vector<Transform>>>>> render_objects;
    render_objects.reserve(2);

    auto& [sphere_mesh, sphere_mats] = render_objects.emplace_back(std::make_pair(
        Mesh(sphere::vertices, sphere::indices), 
        std::vector<std::pair<Material, std::vector<Transform>>>()
    ));

    auto& [cube_mesh, cube_mats] = render_objects.emplace_back(std::make_pair(
        Mesh(cube::vertices, cube::indices), 
        std::vector<std::pair<Material, std::vector<Transform>>>()
    ));

    sphere_mats.reserve(2);
    cube_mats.reserve(2);

    auto& [black_sphere_mat, black_sphere_instances] = sphere_mats.emplace_back(std::make_pair(
        Material(),
        std::vector<Transform>()
    ));
    auto& [white_sphere_mat, white_sphere_instances] = sphere_mats.emplace_back(std::make_pair(
        Material(),
        std::vector<Transform>()
    ));


    auto& [black_cube_mat, black_cube_instances] = cube_mats.emplace_back(std::make_pair(
        Material(),
        std::vector<Transform>()
    ));
    auto& [white_cube_mat, white_cube_instances] = cube_mats.emplace_back(std::make_pair(
        Material(),
        std::vector<Transform>()
    ));


    black_sphere_mat = {
        vec3(0.1f), //diffuse
        18.0f,//shininess
        vec3(0.9f), //specular
        vec3(0.0f), //luminance
    };
    black_cube_mat = black_sphere_mat;


    white_sphere_mat = {
        vec3(1.0f, 1.0f, 1.0f), //diffuse
        34.0f,//shininess
        vec3(0.7f), //specular
        vec3(0.0f), //luminance
    };
    white_cube_mat = white_sphere_mat;


    std::vector<mat4> transforms;
    transforms.reserve(8*4*3);


    auto mat_iden = Transform(1.0f);
    for(float x = -3.5f; x < 4.0f; x += 1.0f){
        for(float y = -3.5f; y < -2.0f; y += 1.0f){
            auto e = mat_iden;
            e.translate(vec3(x,y,0.0f));
            auto top = e;
            auto mid = e;
            auto dwn = e;
    
            top.translate(vec3(0.0f, 0.0f, 1.2f)).scale(vec3(0.25f, 0.25f, 0.25f));
            mid.translate(vec3(0.0f, 0.0f, 0.6f)).scale(vec3(0.2f, 0.2f, 0.6f));
            dwn.scale(vec3(.4f, .4f, 0.08f));

            black_sphere_instances.push_back(top);
            black_sphere_instances.push_back(mid);
            black_sphere_instances.push_back(dwn);
        }

        for(float y = 2.5f; y < 4.0f; y += 1.0f){
            auto e = mat_iden;
            e.translate(vec3(x,y,0.0f));
            auto top = e;
            auto mid = e;
            auto dwn = e;
    
            top.translate(vec3(0.0f, 0.0f, 1.2f)).scale(vec3(0.25f, 0.25f, 0.25f));
            mid.translate(vec3(0.0f, 0.0f, 0.6f)).scale(vec3(0.2f, 0.2f, 0.6f));
            dwn.scale(vec3(.4f, .4f, 0.08f));

            white_sphere_instances.push_back(top);
            white_sphere_instances.push_back(mid);
            white_sphere_instances.push_back(dwn);
        }
    }



    std::vector<mat4> cube_trans;
    cube_trans.reserve(8*8);
    int counter = 0;
    for(float x = -3.5f; x < 4.0f; x += 1.0f){
        counter++;
        for(float y = -3.5f; y < 4.0f; y += 1.0f){
            auto tmp = mat_iden;
            tmp = translate(tmp, vec3(x, y, -0.2f));
            tmp = scale(tmp, vec3(.5f, .5f, .1f));
            //cube_trans.push_back(tmp);
            if(counter % 2 == 0){
                black_cube_instances.push_back(tmp);
            }
            else{
                white_cube_instances.push_back(tmp);
            }
            counter++;
        }
    }
    



    Spotlight light[3];

    light[0].attenuation = vec3(1.0f, 0.07f, 0.017f);
    light[0].color = vec3(1.0f, 1.0f, 1.0f);
    light[0].pos = vec3(0.0f, .0f, 2.0f);
    light[0].dir = normalize(vec3(0.0f, 0.0f, -1.0f));
    light[0].cos_a = cos(radians(30.0f));
    light[0].cos_b = cos(radians(45.0f));

    light[1].attenuation = vec3(1.0f, 0.07f, 0.017f);
    light[1].color = vec3(1.0f, 1.0f, 1.0f);
    light[1].pos = vec3(0.0f, .0f, 2.0f);
    light[1].dir = normalize(vec3(0.0f, 0.0f, -1.0f));
    light[1].cos_a = cos(radians(30.0f));
    light[1].cos_b = cos(radians(45.0f));

    light[2].attenuation = vec3(1.0f, 0.07f, 0.017f);
    light[2].color = vec3(1.0f, 1.0f, 1.0f);
    light[2].pos = vec3(0.0f, .0f, 2.0f);
    light[2].dir = normalize(vec3(0.0f, 0.0f, -1.0f));
    light[2].cos_a = cos(radians(30.0f));
    light[2].cos_b = cos(radians(45.0f));



    g_camera.position = vec3(0.0f, 5.0f, 2.0f);
    g_camera.up_dir = vec3(0.0f, 0.0f, 1.0f);
    g_camera.look_at = vec3(0.0f, 0.0f, 0.0f);

    g_camera.fov = 60.0f;
    g_camera.near_plane = 0.001f;
    g_camera.far_plane = 100.0f;
    g_camera.aspect = (float)INIT_WIDTH / (float)INIT_HEIGHT;
    
    g_camera.calc_view();
    g_camera.calc_perpective();


    GLuint view_loc = glGetUniformLocation(shader, "camera.view");
    GLuint projection_loc = glGetUniformLocation(shader, "camera.projection");
    GLuint campos_loc = glGetUniformLocation(shader, "CamPos");
    GLuint transform_loc = glGetUniformLocation(shader, "transform");
    GLuint is_flat_loc = glGetUniformLocation(shader, "is_flat_shading");



    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); 
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);


    glUseProgram(shader);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(g_camera.view));
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(g_camera.perspective));

    glUniform1i(is_flat_loc, 0);
    light[0].set_uniform(shader, 0);



    while(!glfwWindowShouldClose(window)){
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        double t = glfwGetTime()*8;
        double r = 8.0;
        g_camera.position.x = sin(radians(t)) * r;
        g_camera.position.y = cos(radians(t)) * r;
        g_camera.position.z = 2.0f;
        //g_camera.position = g_camera.position + g_camera.look_at;
        g_camera.calc_view();

        glUniform3fv(campos_loc, 1, value_ptr(g_camera.position));
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(g_camera.view));
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(g_camera.perspective));


        glUniform1i(is_flat_loc, 0);

        for(auto& [mesh, mats]: render_objects){
            for(auto& [mat, transforms]: mats){
                mat.set_uniform(shader);
                for(auto& t: transforms){
                    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, t.value_ptr() );
                    mesh.render();
                }
            }

        }

        
        // glUniform1i(is_flat_loc, 1);
        // for(auto& t : cube_trans){
        //     glUniformMatrix4fv(transform_loc, 1, GL_FALSE, value_ptr(t));
        //     cube_mesh.render();
        // }



        glfwSwapBuffers(window);
    }


    glfwTerminate();
    return 0;
}