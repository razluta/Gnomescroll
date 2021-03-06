#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

#ifdef GL_EXT_gpu_shader4
    flat varying vec2 Light;
#else
    varying vec2 Light;
#endif

varying float fogFragDepth;

uniform sampler2D base_texture;
uniform sampler3D clut_texture;
uniform sampler2D clut_light_texture;


varying vec2 texCoord;

const float clut_start = 16.0;
const float _clut_depth = 1.0/16.0;

void main() 
{
	vec4 tex = texture2D(base_texture, texCoord.xy);
	tex.rgb = tex.rgb*texture2D(clut_light_texture, Light).rgb;

	vec3 color = tex.rgb;

    vec3 color_clut = texture3D(clut_texture, color).rgb; //clut correction

    if(fogFragDepth > clut_start)
    {
        float f = _clut_depth*(fogFragDepth - clut_start);
        f = clamp(f, 0.0f, 1.0f);
        tex.rgb = mix(color, color_clut, f);
    }

	gl_FragColor = tex;
}



