#version 120

uniform mat4 InMatrix;

attribute vec3 InNormal;

const vec3 light_dir = normalize(vec3(1.00, 1.00, 2.00));   //fixed light source?

varying float diffuse_light;

void main(void) 
{
    vec4 pos = gl_ModelViewProjectionMatrix * InMatrix * gl_Vertex;

    // set light diffusion
 
    //lighting -1
    vec3 normal = gl_NormalMatrix*(mat3(InMatrix)*InNormal);
    diffuse_light = dot(light_dir, normal);

/*
    //lighting 0
    vec3 normal = mat3(InMatrix)*InNormal;
    diffuse_light = dot(light_dir, normal);
*/

/*
    //lighting 1
    vec3 look = normalize(pos.xyz - InCameraPos);
    vec3 normal = gl_NormalMatrix*(mat3(InMatrix)*InNormal);
    diffuse_light = abs(dot(look, normal));
*/

/*  
    //lighting 2
    vec3 look = normalize(pos.xyz - InCameraPos);
    diffuse_light = abs(dot(look, (InMatrix * vec4(InNormal, 0.0f)).xyz));           //use camera position
*/

    // set position
    gl_Position = pos;

    gl_FrontColor = gl_Color;
}
