#include <SDL2/SDL.h>
#include <GLES3/gl32.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <iostream>

#include "vao/vao.h"
#include "vbo/vbo.h"
#include "ibo/ibo.h"
#include "shader/shader.h"

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define clamp(x, a, b) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))

SDL_Window *g_pWindow;
SDL_GLContext g_pCtx;

float rx = 0.0f;
float ry = 0.0f;

glm::vec3 pos = {0.0f, 0.0f, 0.0f};

void GLErrorCallback(GLenum src, GLenum type, GLuint id,
                     GLenum severity, GLsizei length, const char *msg,
                     const void *usrParams)
{
    std::cout << "OpenGL Error (" << id << "): ";

    std::cout << "\nMsg = [\n"
              << msg << "\n]\n";
}

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return -1;
    }
    g_pWindow = SDL_CreateWindow("Minecraft Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    if (g_pWindow == nullptr)
    {
        return -1;
    }
    g_pCtx = SDL_GL_CreateContext(g_pWindow);
    if (g_pCtx == nullptr)
    {
        return -1;
    }

    SDL_GL_SetSwapInterval(1);

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDebugMessageCallback(GLErrorCallback, nullptr);

    Vbo vb({{-5.0, -5.0, 0.0},
            {5.0, -5.0, 0.0},
            {5.0, 5.0, 0.0},
            {-5.0, 5.0, 0.0}},
           GL_STATIC_DRAW);
    vb.Bind();

    BufferLayout layout;
    layout.Push<float>(3);

    Vao va;
    va.AddBuffer(vb, layout);

    Ibo ib({0, 1, 2,
            0, 2, 3},
           GL_STATIC_DRAW);
    ib.Bind();

    Shader shader("./assets/vert.glsl", "./assets/frag.glsl");
    shader.Bind();

    va.Bind();

    glm::mat4 model, view, proj, rot, mvp;

    model = glm::translate(glm::identity<glm::mat4>(), {0.0f, 0.0f, -10.0f});
    view = glm::identity<glm::mat4>();
    mvp = glm::identity<glm::mat4>();

    // proj = glm::frustum(-2.0f, 2.0f, -1.5f, 1.5f, 1.0f, 100.0f);
    proj = glm::perspective(90.0f * 0.01745329251994329576923690768489f, 640.0f / 480.0f, 1.0f, 100.0f);

    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    bool running = true;
    while (running)
    {
        if (SDL_QuitRequested() | SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_ESCAPE])
        {
            running = false;
        }

        if(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_A])
        {
            glm::vec3 right = {
                view[0][0],
                0.0f,
                view[2][0],
            };
            pos -= right * 0.1f;
        }
        if(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_D])
        {
            glm::vec3 right = {
                view[0][0],
                0.0f,
                view[2][0],
            };
            pos += right * 0.1f;
        }
        if(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_W])
        {
            glm::vec3 forward = {
                view[0][2],
                0.0f,
                view[2][2],
            };
            pos -= forward * 0.1f;
        }
        if(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_S])
        {
            glm::vec3 forward = {
                view[0][2],
                0.0f,
                view[2][2],
            };
            pos += forward * 0.1f;
        }

        int x, y;
        SDL_GetRelativeMouseState(&x, &y);

        rx += y * 0.2f;
        rx = clamp(rx, -89,89);
        ry += x * 0.2f;

        rot = glm::rotate(glm::translate(glm::identity<glm::mat4>(), pos), -ry * 0.01745329251994329576923690768489f, {0.0f, 1.0f, 0.0f});
        view = glm::inverse(glm::rotate(rot, -rx * 0.01745329251994329576923690768489f, {1.0f, 0.0f, 0.0f}));
        
        mvp = proj * view * model;
        shader.SetUniformMat4("u_mvp", mvp);

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
        // glDrawArrays(GL_TRIANGLES, 0, 4);

        SDL_GL_SwapWindow(g_pWindow);
    }

    SDL_GL_DeleteContext(g_pCtx);
    SDL_DestroyWindow(g_pWindow);
    SDL_Quit();

    std::cout << "Hello World\n";
    return 0;
}