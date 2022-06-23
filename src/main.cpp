#include <SDL2/SDL.h>
#include <GLES3/gl32.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <iostream>

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

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return -1;
    }

    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

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

    { // GL program scope
    
    // BufferLayout layout;
    // layout.Push<float>(3);
    // layout.Push<float>(2);
    
    // Shader shader("./assets/shaders/test/vert.glsl", "./assets/shaders/test/frag.glsl");
    // shader.Bind();

    // shader.SetUniform1i("u_tex", 0);
    // Texture tex("./assets/textures/Minecraft-atlas.png", GL_NEAREST);
    // tex.Bind();

    glm::mat4 model, view, proj, rot, mvp;

    model = glm::translate(glm::identity<glm::mat4>(), {0.0f, 0.0f, 0.0f});
    view = glm::identity<glm::mat4>();
    mvp = glm::identity<glm::mat4>();

    // proj = glm::frustum(-2.0f, 2.0f, -1.5f, 1.5f, 1.0f, 100.0f);
    int w = 640, h = 480;
    float aspect = (float)w / (float)h;
    proj = glm::perspectiveLH(90.0f * 0.01745329251994329576923690768489f, aspect, 0.2f, 100.0f);

    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    Player p;

    World world;
    // Chunk c;
    // c.GenerateVertices();
    // c.GenerateIndices(p.GetPos());

    // std::cout << "Vb size = " << c.GetVb().GetData().size() << "\n";
    // std::cout << "Ib size = " << c.GetSolidIb().GetData().size() << "\n";

    // Vao va;
    // va.AddBuffer(c.GetVb(), layout);
    // c.GetSolidIb().Bind();
    // va.Bind();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    bool running = true;
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
        
        // mvp = proj * view * model;
        // shader.SetUniformMat4("u_mvp", mvp);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glDrawElements((wireframe) ? GL_LINES : GL_TRIANGLES, c.GetSolidIb().GetData().size(), GL_UNSIGNED_SHORT, nullptr);
        world.RenderSolid(proj * view);
        // glDrawArrays(GL_TRIANGLES, 0, 4);

        SDL_GL_SwapWindow(g_pWindow);
    }

    } // End GL program scope

    SDL_GL_DeleteContext(g_pCtx);
    SDL_DestroyWindow(g_pWindow);
    SDL_Quit();

    std::cout << "Hello World\n";
    return 0;
}