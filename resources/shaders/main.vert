#version 400 core


struct Camera{
    mat4 view;
    mat4 projection;
};


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Pos;  
out vec3 Norm;

uniform Camera camera;
uniform mat4 global_transform;
uniform mat4 local_transform;

void main()
{
    vec4 temp = global_transform * local_transform * vec4(aPos, 1.0);

    gl_Position = camera.projection * camera.view * temp;

    Pos = vec3(temp);
    Norm = aNormal;

}