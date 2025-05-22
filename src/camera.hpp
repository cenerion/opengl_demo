#pragma once

#include "transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

class Camera{
public:
    void recalculate()
    {
        view = glm::lookAt(m_position, m_target, up_dir);
        persp = glm::perspective(m_fov, m_aspect, m_near_plane, m_far_plane);
    }


    void set_shader(GLuint shader)
    {
        GLuint view_loc = glGetUniformLocation(shader, "camera.view");
        GLuint projection_loc = glGetUniformLocation(shader, "camera.projection");
        GLuint campos_loc = glGetUniformLocation(shader, "CamPos");


        glUniform3fv(campos_loc, 1, glm::value_ptr(m_position));
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, view.value_ptr());
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, persp.value_ptr());
    }

    void move(glm::vec3 move)
    {
        auto forward = normalize(m_position - m_target);
        auto right = cross(forward, up_dir);

        move = right * move.x + up_dir * move.y + forward * move.z;

        m_target += move;
        m_position += move;
    }

    void move_target(glm::vec3 move )
    {
        m_target += move;
    }



    void set_position(glm::vec3 position)
    {
        m_position = position;
    }

    void set_target(glm::vec3 target)
    {
        m_target = target;
    }
    
    void set_up_direction(glm::vec3 up)
    {
        up_dir = up;
    }

    void set_fov(float deg)
    {
        if(deg <= 0.0f) deg = 0.1f;

        m_fov = glm::radians(deg);
    }
    
    void set_aspect(float aspect)
    {
        if(aspect <= 0.0f) aspect = 0.1f;

        m_aspect = aspect;
    }

    void set_near_plane(float near)
    {
        if(near <= 0.0f) near = 0.1f;
        m_near_plane = near;
    }

    void set_far_plane(float far)
    {
        if(far <= 0.0f) far = 0.1f;
        m_far_plane = far;
    }

private:
    glm::mat4 calc_view();
    glm::mat4 calc_perpective();

private:
    glm::vec3 up_dir;
    glm::vec3 m_position;
    glm::vec3 m_target;


    float m_near_plane = 0.01f;
    float m_far_plane = 1000.0f;
    float m_fov = 1.0f;
    float m_aspect = 1.0f;

    Transform view;
    Transform persp;
};