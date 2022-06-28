#include <SDL2/SDL.h>
#include <GLES3/gl32.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <iostream>

#include <time.h>

#include "block/chunk.h"
#include "block/world.h"
#include "object/player.h"

#include "input/input.h"

#include "ibo/ibo.h"
#include "shader/shader.h"
#include "texture/texture.h"
#include "vao/vao.h"
#include "vbo/vbo.h"

#include "settings/settings.h"

#include "skybox/skybox.h"

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define clamp(x, a, b) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))

SDL_Window *g_pWindow;
SDL_GLContext g_pCtx;

void GLErrorCallback(GLenum src, GLenum type, GLuint id,
                     GLenum severity, GLsizei length, const char *msg,
                     const void *usrParams)
{
    std::cout << "OpenGL Error (" << id << "): ";

    std::cout << "\nMsg = [\n"
              << msg << "\n]\n";
}

double GetTime()
{
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 0.000000001;
}

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return -1;
    }

    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    int w = 640, h = 480;
    g_pWindow = SDL_CreateWindow("Minecraft Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
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

    glClearColor(0.4f, 0.6f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDebugMessageCallback(GLErrorCallback, nullptr);

    { // GL program scope

    Skybox sky("assets/textures/Skybox2.png");

    glm::mat4 model, view, proj, rot, mvp;

    model = glm::scale(glm::translate(glm::identity<glm::mat4>(), {0.0f, 0.0f, 0.0f}), {3.0f, 3.0f, 3.0f});
    view = glm::identity<glm::mat4>();
    mvp = glm::identity<glm::mat4>();

    // proj = glm::frustum(-2.0f, 2.0f, -1.5f, 1.5f, 1.0f, 100.0f);
    float aspect = (float)w / (float)h;
    proj = glm::perspectiveLH(90.0f * 0.01745329251994329576923690768489f, aspect, 0.2f, 200.0f);

    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    Player p;

    World world;

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    bool running = true;
    float t = 1.0f;
    float inc = -0.001f;

    while (running)
    {
        if (SDL_QuitRequested() | Input::Instance().GetKey(SDL_SCANCODE_ESCAPE))
        {
            running = false;
        }

        if(Input::Instance().GetKey(SDL_SCANCODE_TAB))
        {
            Settings::wireframe = !Settings::wireframe;
            SDL_Delay(170);
        }

        p.Update(p);
        world.Update(p);

        p.CalcModel();

        view = glm::inverse(p.GetModel());
        glm::mat4 vp = proj * view;
        
        // // break block
        // if(Input::Instance().GetMouseButton(MOUSE_LEFT))
        // {
        //     glm::vec3 forward( view[0][2], view[1][2], view[2][2] );

        //     glm::vec3 pos = p.GetPos();
        //     for(int i = 0; i < 8; ++i)
        //     {
        //         if(world.GetBlock(pos.x, pos.y, pos.z) != BLOCK_AIR)
        //         {
        //             world.SetBlock(p, pos.x, pos.y, pos.z, BLOCK_AIR);
        //             SDL_Delay(30);
        //             break;
        //         }

        //         pos += forward;
        //     }
        // }
        // // build block
        // if(Input::Instance().GetMouseButton(MOUSE_RIGHT))
        // {
        //     glm::vec3 forward( view[0][2], view[1][2], view[2][2] );

        //     glm::vec3 pos = p.GetPos() + forward * 2.0f;
        //     for(int i = 0; i < 7; ++i)
        //     {
        //         if(world.GetBlock(pos.x, pos.y, pos.z) != BLOCK_AIR)
        //         {
        //             if(glm::dot(pos-p.GetPos(), pos-p.GetPos()) > 4.0f)
        //             {
        //                 pos -= forward;
        //                 world.SetBlock(p, pos.x, pos.y, pos.z, BLOCK_DIRT);
        //                 SDL_Delay(20);
        //             }
        //             break;
        //         }

        //         pos += forward;
        //     }
        // }
        

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the skybox
        sky.Draw(vp, p.GetPos(), t);
        
        t += inc;
        if(t > 1.0f || t < 0.0f)
        {
            inc = -inc;
            if(inc > 0) { t = 0.0f; }
            else if(inc < 0) { t = 1.0f; }
        }

        world.RenderSolid(vp);
        
        SDL_GL_SwapWindow(g_pWindow);
    }

    } // End GL program scope

    SDL_GL_DeleteContext(g_pCtx);
    SDL_DestroyWindow(g_pWindow);
    SDL_Quit();

    std::cout << "Hello World\n";
    return 0;
}