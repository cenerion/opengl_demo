#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>



class Transform : public glm::mat4{
public:

    inline
    Transform& translate(const glm::vec3& v)
    {
        (*this) = glm::translate(*this, v);
        return *this;
    }

    inline
    Transform& translate(float x, float y, float z)
    {
        (*this) = glm::translate(*this, glm::vec3(x,y,z));
        return *this;
    }

    inline
    Transform& scale(const glm::vec3& v)
    {
        (*this) = glm::scale(*this, v);
        return *this;
    }

    inline
    Transform& scale(float x, float y, float z)
    {
        (*this) = glm::scale(*this, glm::vec3(x,y,z));
        return *this;
    }

    inline
    Transform& rotate(const glm::vec3& v)
    {
        (*this) = glm::rotate(*this, v.x, glm::vec3(1.0f, 0.0f, 0.0f));
        (*this) = glm::rotate(*this, v.y, glm::vec3(0.0f, 1.0f, 0.0f));
        (*this) = glm::rotate(*this, v.z, glm::vec3(0.0f, 0.0f, 1.0f));
        return *this;
    }

    inline
    Transform& rotate(float x, float y, float z)
    {
        (*this) = glm::rotate(*this, x, glm::vec3(1.0f, 0.0f, 0.0f));
        (*this) = glm::rotate(*this, y, glm::vec3(0.0f, 1.0f, 0.0f));
        (*this) = glm::rotate(*this, z, glm::vec3(0.0f, 0.0f, 1.0f));
        return *this;
    }

    inline
    value_type const* value_ptr() const
    {
        return glm::value_ptr(*((glm::mat4*)this));
    }

    inline
    Transform& operator=(const glm::mat4& m)
    {
        glm::mat4::operator=(m);
        return *this;
    }

    inline
    glm::mat4 get_mat()
    {
        return *this;
    }

    using glm::mat4::mat;
};
