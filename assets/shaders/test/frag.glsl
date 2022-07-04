#version 130

in vec2 v_texCoord;
in float v_light;

uniform sampler2D u_tex;

void main()
{
    vec4 color = texture(u_tex, v_texCoord);
    
    gl_FragColor = vec4(color.r * v_light, color.g * v_light, color.b * v_light, color.a);
};