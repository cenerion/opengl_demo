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
#include <string>
#include <memory>
#include <format>
#include <unordered_map>
#include <tuple>

#include "Mesh.hpp"
#include "sphere.hpp"
#include "cube.hpp"
#include "camera.hpp"
#include "transform.hpp"

using namespace glm;


static Camera g_camera;

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
    float inner_cos;
    float outer_cos;

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
            inner_cos);

        glUniform1f(
            glGetUniformLocation(shader, (ye+".cos_b").c_str()),
            outer_cos);
        }
};



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



class Model {
private:
    std::vector<std::pair<std::weak_ptr<Mesh>, Transform>> meshs;
    Material material { };
    Transform model_view = Transform(1.0f);

public:
    Model(){

    }

    Model(const Model& other){
        meshs = other.meshs;
        material = other.material;
        model_view = other.model_view;
    }

    Model(Model&&) = delete;


    void addMesh(std::weak_ptr<Mesh> mesh, Transform transform){
        meshs.emplace_back(mesh, transform);
    }

    Transform& getTransform(){
        return model_view;
    }

    void draw(GLuint shader){
        GLuint global_trans_loc = glGetUniformLocation(shader, "global_transform");
        GLuint local_trans_loc = glGetUniformLocation(shader, "local_transform");
        
        material.set_uniform(shader);

        glUniformMatrix4fv(global_trans_loc, 1, GL_FALSE, model_view.value_ptr());

        for(auto& [mesh_ptr, local_transform]: meshs){

            glUniformMatrix4fv(local_trans_loc, 1, GL_FALSE, local_transform.value_ptr() );
            mesh_ptr.lock()->render();
        }
    }

    Material& getMaterial(){
        return material;
    }
};


void resize_handler(GLFWwindow* window, int width, int height);



class InputHandler{
public:
    InputHandler()
    {

    }

    void input_handler(GLFWwindow* window, int key, int scancode, int action, int mods) {
        // Send key event to all KeyInput instances
        // default - orbiting
        // shift - moving axile
        // ctrl - rotating

        //std::println(std::cout, "key: {} a: {}, ctrl: {}, shift: {}", key, action != GLFW_RELEASE, ctrl, shift);
        switch (key)
        {
        case GLFW_KEY_UP:
        case GLFW_KEY_DOWN:
        case GLFW_KEY_LEFT:
        case GLFW_KEY_RIGHT:
        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_PAGE_UP:
        case GLFW_KEY_PAGE_DOWN:
            state[ Key(key) ] = action != GLFW_RELEASE;

        default:
            break;
        }
    }

    
    // Send key event to all KeyInput instances
    // default - orbiting
    // shift - moving axile
    // ctrl - rotating
    void process_input(float delta)
    {
        if(state[l_ctrl])
        {
            //rotating
            vec3 dir = vec3(0.0f);

            if(state[up]) dir.y += 1.0f;
            if(state[down]) dir.y -= 1.0f;

            if(state[right]) dir.x += 1.0f;
            if(state[left]) dir.x -= 1.0f;

            float move_speed = 2.0f;
            g_camera.move_target(dir * delta * move_speed);
        }
        else if(state[l_shift])
        {
            //moving
        }
        else
        {
            //orbiting
            vec3 dir = vec3(0.0f);

            if(state[up]) dir.z -= 1.0f;
            if(state[down]) dir.z += 1.0f;
            if(state[right]) dir.x += 1.0f;
            if(state[left]) dir.x -= 1.0f;
            if(state[pg_up]) dir.y += 1.0f;
            if(state[pg_down]) dir.y -= 1.0f;

            float move_speed = 2.0f;
            g_camera.move(dir * delta * move_speed);
        }
    }

    enum Key : int{
        up = GLFW_KEY_UP,
        down = GLFW_KEY_DOWN,
        left = GLFW_KEY_LEFT,
        right = GLFW_KEY_RIGHT,
        pg_up = GLFW_KEY_PAGE_UP,
        pg_down = GLFW_KEY_PAGE_DOWN,
        l_shift = GLFW_KEY_LEFT_SHIFT,
        l_ctrl = GLFW_KEY_LEFT_CONTROL
    };

private:
    std::unordered_map<Key, bool> state;
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
    InputHandler input_handler;
    glfwSetWindowUserPointer(window, &input_handler);

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
        InputHandler* input_handler = (InputHandler*)glfwGetWindowUserPointer(window);
        input_handler->input_handler(window, key, scancode, action, mods);
    });

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
    auto sphere_mesh = std::make_shared<Mesh>(sphere::vertices, sphere::indices);
    auto cube_mesh = std::make_shared<Mesh>(cube::vertices, cube::indices);

    // Mesh sphere_mesh(sphere::vertices, sphere::indices);
    // Mesh cube_mesh(cube::vertices, cube::indices);

    std::vector<Model> models;
    models.reserve(8*8*5);

    auto black_sphere_mat = Material();
    auto white_sphere_mat = Material();
    auto black_cube_mat = Material();
    auto white_cube_mat = Material();

    auto black_sphere_instances = std::vector<Transform>();
    auto white_sphere_instances = std::vector<Transform>();
    auto black_cube_instances = std::vector<Transform>();
    auto white_cube_instances = std::vector<Transform>();

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


    Model plansza;
    Model pionek;

    {
        auto top = mat_iden;
        auto mid = mat_iden;
        auto dwn = mat_iden;

        top.translate(0.0f, 1.2f, 0.0f).scale(0.25f, 0.25f, 0.25f);
        mid.translate(0.0f, 0.6f, 0.0f).scale(0.2f, 0.6f, 0.2f);
        dwn.scale(.25f, 0.08f, .25f);

        pionek.addMesh(sphere_mesh, top);
        pionek.addMesh(sphere_mesh, mid);
        pionek.addMesh(cube_mesh, dwn);
    }


    for(float x = -3.5f; x < 4.0f; x += 1.0f){
        for(float y = -3.5f; y < -2.0f; y += 1.0f){

            auto new_pionek = models.emplace_back(pionek);
            new_pionek.getTransform().translate(x,0.0f,y);
            new_pionek.getMaterial() = black_sphere_mat;
        }

        for(float y = 2.5f; y < 4.0f; y += 1.0f){
            auto new_pionek = models.emplace_back(pionek);
            new_pionek.getTransform().translate(x,0.0f,y);
            new_pionek.getMaterial() = white_sphere_mat;
        }
    }


    std::vector<mat4> cube_trans;
    cube_trans.reserve(8*8);
    int counter = 0;
    for(float x = -3.5f; x < 4.0f; x += 1.0f){
        for(float y = -3.5f; y < 4.0f; y += 1.0f){
            auto tmp = mat_iden;
            tmp.translate(x, -0.2f, y).scale(.5f, .1f, .5f);
            
            if(counter % 2 == 0){
                black_cube_instances.push_back(tmp);
            }
            else{
                white_cube_instances.push_back(tmp);
            }
            counter++;
        }
        counter++;
    }
    



    Spotlight light[3];

    light[0].attenuation = vec3(1.0f, 0.07f, 0.017f);
    light[0].color = vec3(0.6f);
    light[0].pos = vec3(0.0f, .0f, 2.0f);
    light[0].dir = normalize(vec3(0.0f, 0.0f, -1.0f));
    light[0].inner_cos = cos(radians(30.0f));
    light[0].outer_cos = cos(radians(45.0f));

    light[1].attenuation = vec3(1.0f, 0.07f, 0.017f);
    light[1].color = vec3(0.0f, 1.0f, 0.0f);
    light[1].pos = vec3(0.0f, .0f, 2.0f);
    light[1].dir = normalize(vec3(4.0f, 4.0f, -1.0f) - light[1].pos);
    light[1].inner_cos = cos(radians(30.0f));
    light[1].outer_cos = cos(radians(45.0f));

    light[2].attenuation = vec3(1.0f, 0.07f, 0.017f);
    light[2].color = vec3(1.0f, 0.0f, 0.0f);
    light[2].pos = vec3(0.0f, .0f, 2.0f);
    light[2].dir = normalize(vec3(-4.0f, -4.0f, 0.0f) - light[2].pos);
    light[2].inner_cos = cos(radians(30.0f));
    light[2].outer_cos = cos(radians(45.0f));
/**///


    g_camera.set_position(vec3(0.0f, 5.0f, 2.0f));
    g_camera.set_target(vec3(0.0f, 0.0f, 0.0f));
    g_camera.set_up_direction(vec3(0,1,0));
    g_camera.set_fov(100.0f);
    g_camera.set_near_plane(0.001f);
    g_camera.set_far_plane(100.0f);
    g_camera.set_aspect((float)INIT_WIDTH / (float)INIT_HEIGHT);
    g_camera.recalculate();



    GLuint global_transform_loc = glGetUniformLocation(shader, "global_transform");
    GLuint local_transform_loc = glGetUniformLocation(shader, "local_transform");
    GLuint is_flat_loc = glGetUniformLocation(shader, "is_flat_shading");


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); 
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );//
    glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);


    glUseProgram(shader);

    glUniform1i(is_flat_loc, 0);
    light[0].set_uniform(shader, 0);
    light[1].set_uniform(shader, 1);
    light[2].set_uniform(shader, 2);


    auto time_s = glfwGetTime();
    while(!glfwWindowShouldClose(window)){
        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        
        auto delta = glfwGetTime() - time_s;
        time_s = glfwGetTime();

        input_handler.process_input(delta);

        g_camera.recalculate();
        g_camera.set_shader(shader);


        for(auto& model : models){
            model.draw(shader);
        }


        glUniformMatrix4fv(local_transform_loc, 1, GL_FALSE, mat_iden.value_ptr());
        black_cube_mat.set_uniform(shader);
        for(auto& t: black_cube_instances){
            glUniformMatrix4fv(global_transform_loc, 1, GL_FALSE, t.value_ptr() );
            cube_mesh->render();
        }
        
        white_cube_mat.set_uniform(shader);
        for(auto& t: white_cube_instances){
            glUniformMatrix4fv(global_transform_loc, 1, GL_FALSE, t.value_ptr() );
            cube_mesh->render();
        }
        
        black_sphere_mat.set_uniform(shader);
        for(auto& t: black_sphere_instances){
            glUniformMatrix4fv(global_transform_loc, 1, GL_FALSE, t.value_ptr() );
            sphere_mesh->render();
        }
        
        white_sphere_mat.set_uniform(shader);
        for(auto& t: white_sphere_instances){
            glUniformMatrix4fv(global_transform_loc, 1, GL_FALSE, t.value_ptr() );
            sphere_mesh->render();
        }

        glfwSwapBuffers(window);
    }


    glfwTerminate();
    return 0;
}



void resize_handler(GLFWwindow* window, int width, int height){
    glViewport(0,0, width, height);
    g_camera.set_aspect((float)width / (float)height);
    g_camera.recalculate();
}