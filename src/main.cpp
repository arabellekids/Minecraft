#include <SDL2/SDL.h>
#include <GLES3/gl32.h>
#include <iostream>

#include "vao/vao.h"
#include "vbo/vbo.h"
#include "ibo/ibo.h"
#include "shader/shader.h"

SDL_Window* g_pWindow;
SDL_GLContext g_pCtx;

void GLErrorCallback(GLenum src, GLenum type, GLuint id,
GLenum severity, GLsizei length, const char* msg,
const void* usrParams)
{
    std::cout << "OpenGL Error (" << id << "): ";

    std::cout << "\nMsg = [\n" << msg << "\n]\n";
}

int main()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) { return -1; }
    g_pWindow = SDL_CreateWindow("Minecraft Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640,480, SDL_WINDOW_OPENGL);
    if(g_pWindow == nullptr) { return -1; }
    g_pCtx = SDL_GL_CreateContext(g_pWindow);
    if(g_pCtx == nullptr) { return -1; }

    SDL_GL_SetSwapInterval(1);

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDebugMessageCallback(GLErrorCallback, nullptr);

    std::cout << "Creating vbo\n";
    Vbo vb({
        {-0.5, -0.5},
        { 0.5, -0.5},
        { 0.0,  0.5}
    }, GL_STATIC_DRAW);
    vb.Bind();

    BufferLayout layout;
    layout.Push<float>(2);

    Vao va;
    va.AddBuffer(vb, layout);

    Ibo ib({0,1,2}, GL_STATIC_DRAW);
    ib.Bind();

    Shader shader("./assets/vert.glsl", "./assets/frag.glsl");
    shader.Bind();

    va.Bind();

    while(!SDL_QuitRequested())
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(g_pWindow);
    }

    SDL_GL_DeleteContext(g_pCtx);
    SDL_DestroyWindow(g_pWindow);
    SDL_Quit();

    std::cout << "Hello World\n";
    return 0;
}