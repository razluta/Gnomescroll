#version 120

varying float diffuse_light;
varying vec3 base_color;

#define diffuse_p .20f //diffuse lighting weight
#define ambient_p .65f //ambient lighting weight

void main() 
{
    vec3 color = (diffuse_light*diffuse_p+ambient_p) * base_color;
    color = pow(color, vec3(1.0f / 2.2f));  // gamma correction  TOO BRIGHT
    gl_FragColor.rgb = color;

    //uncomment to see the diffuse
    //gl_FragColor.rgb = vec3(diffuse_light, diffuse_light, diffuse_light);

    //uncomment to see total lighting
    //gl_FragColor.rgb = vec3(diffuse_light*diffuse_p+ambient_p, diffuse_light*diffuse_p+ambient_p, diffuse_light*diffuse_p+ambient_p);
}
