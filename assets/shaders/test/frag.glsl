#version 130

in vec2 v_texCoord;

uniform sampler2D u_tex;

void main()
{
    gl_FragColor = texture(u_tex, v_texCoord);
    //gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
};