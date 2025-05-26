#version 400 core

struct Material{
    vec3 diffuse;
    float shininess;
    vec3 specular;
    vec3 luminance;
};

struct Spotlight {
    vec3 pos;
    vec3 dir;
    float inner_cos;
    float outer_cos;

    vec3 color;
    vec3 attenuation;
};

float light_attenuation(vec3 c, float dist){
    return 1.0 / (c.x + (c.y * dist) + (c.z * dist * dist));
}

#define LIGHTS_COUNT 3
vec3 ambient = vec3(0.04);


in vec3 Pos;
in vec3 Norm;

out vec4 FragColor;

uniform Material material;
uniform Spotlight light[LIGHTS_COUNT];
uniform vec3 CamPos;
uniform bool is_flat_shading;



vec3 calc_light(Spotlight l, vec3 pos, vec3 norm, Material material){

    float dist = distance(pos, l.pos);
    vec3 dir = normalize(l.pos - pos);
    
    float at = light_attenuation(l.attenuation, dist);

    vec3 lightColor = clamp(at * l.color, 0.0, 1.0);


    float theta     = dot(dir, normalize(-l.dir));
    float epsilon   = l.inner_cos - l.outer_cos;
    float intensity = clamp((theta - l.outer_cos) / epsilon, 0.0, 1.0); 

    lightColor = lightColor * intensity;


    float diff = max(dot(norm, dir), 0.0);
    vec3 diffuse = material.diffuse * diff * lightColor;


    vec3 viewDir = normalize(CamPos - pos);
    vec3 reflectDir = reflect(-dir, norm); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * lightColor; 
    

    vec3 result = (diffuse) * material.diffuse + specular;

    return result;
}

void main()
{
    vec3 l = ambient;

    //vec3 l = vec3(0.0);
    for(int i = 0; i < LIGHTS_COUNT; ++i){
        l += calc_light(light[i], Pos, Norm, material);
    }
    

    FragColor = vec4(clamp(l, 0.0, 1.0), 1.0f);

    // if(gl_FrontFacing){
    //     FragColor = vec4(1.0f);
    // }
    // else{
    //     FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    // }

} 