#version 130

attribute vec4 a_pos;
attribute vec2 a_uv;

uniform mat4 u_mvp;

out vec2 v_texCoord;

void main()
{
    gl_Position =  u_mvp * a_pos;
    v_texCoord = a_uv;
};