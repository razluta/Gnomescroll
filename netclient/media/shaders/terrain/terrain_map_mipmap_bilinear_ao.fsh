#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

#extension GL_EXT_texture_array : enable

varying vec3 texCoord;

# ifdef GL_EXT_gpu_shader4
    flat varying mat2 lightMatrix;
#else
    varying mat2 lightMatrix;
#endif


#ifdef GL_EXT_gpu_shader4
    flat varying vec3 CE1;
    flat varying vec3 CE2;
    flat varying vec3 CE3;
    flat varying vec3 CE4;
#else
    varying vec3 CE1;
    varying vec3 CE2;
    varying vec3 CE3;
    varying vec3 CE4;
#endif


varying vec3 inColor;
 
uniform sampler2DArray base_texture;

varying float fogFragDepth;

const vec3 fog_color = vec3(0.0, 0.0, 0.0);
//const float fog_start = 96.0;
//const float fog_depth = 32.0;
//const vec3 fog_color = vec3(200.0, 150.0, 200.0);
const float fog_start = 48.0;
const float fog_depth = 128.0 - fog_start;

void main() 
{
    //float gamma = 2.2f;

/*
	Can split into 3 bilinear interpolations
*/
    vec2 vx = vec2(1.0f - texCoord.x, texCoord.x);
    vec2 vy = vec2(1.0f - texCoord.y, texCoord.y);

    float tmp = dot(vx, lightMatrix * vy);

    //vec3 color = inColor.rgb; 
    
    vec3 c1 = mix(CE1, CE4, texCoord.x);
    vec3 c2 = mix(CE2, CE3, texCoord.x);
    vec3 color = mix(c1, c2, texCoord.y);

    //color = CE2;

    //color = c2;
    //color = color*tmp;
    //color = pow(color, vec3(1.0f / 2.2f) );
    //gl_FragColor.rgb = color;

    color = tmp*color;
    color = color*(texture2DArray(base_texture, texCoord.xyz).rgb);      
    //color = color * inColor;




    if(fogFragDepth <= fog_start)
    {
		color = pow(color, vec3(1.0f / 2.2f) );
        gl_FragColor.rgb = color;
    }
    else
    {
        float fogFactor = (fogFragDepth - fog_start) / fog_depth;

        if(fogFactor >= 1.0) discard;
        
        color = mix( color, fog_color, fogFactor);
        color = pow(color, vec3(1.0f / 2.2f) );
        gl_FragColor.rgb = color;
    }

    //gl_FragColor.rgb = fogFactor*vec3(1.0, 0.0, 0.0);
    //gl_FragColor.rgb = mix(color, vec3(0.0, 0.0, 0.0), fogFactor);

    //fogFragDepth
    //fogFactor = clamp(fogFactor, 0.0, 1.0);
    //gl_FragColor = mix(gl_Fog.color, gl_Color, fogFactor);

    //gl_FragColor.rgb = vec3(1.0,0,0);

}



