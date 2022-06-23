#version 130

attribute vec4 a_pos;
attribute vec2 a_uv;
attribute float a_light;

uniform mat4 u_mvp;

out vec2 v_texCoord;
out float v_light;

void main()
{
    gl_Position =  u_mvp * a_pos;
    v_texCoord = a_uv;
    v_light = a_light;
};