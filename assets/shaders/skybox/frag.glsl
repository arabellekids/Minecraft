#version 130

in vec2 v_texCoord;

uniform float u_time;
uniform sampler2D u_tex;

void main()
{
    vec4 color = texture(u_tex, v_texCoord);
    gl_FragColor = vec4(color.r * u_time, color.g * u_time, color.b * u_time, 1.0);
    
};