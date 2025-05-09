#version 400 core


struct Camera{
    mat4 view;
    mat4 projection;
};


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Pos;  
out vec3 Norm;
out vec3 camPos;

uniform Camera camera;
uniform mat4 transform;

void main()
{
    vec4 temp = transform * vec4(aPos, 1.0);

    gl_Position = camera.projection * camera.view * transform * vec4(aPos, 1.0);

    Pos = vec3(temp);
    Norm = aNormal;
    fNorm = aNormal;

}