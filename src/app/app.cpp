#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <SDL2/SDL.h>

#include "app.h"
#include "../settings/settings.h"

App* App::s_pInstance = nullptr;

App::App(int w, int h, bool fullscreen) : m_bRunning(false), m_pWindow(nullptr), m_pGlCtx(nullptr)
{
    s_pInstance = this;
    
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) { return; }

    m_pWindow = (fullscreen) ? SDL_CreateWindow("Minecraft Demo", 0,0, w,h, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN)
    : SDL_CreateWindow("Minecraft Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w,h, SDL_WINDOW_OPENGL);
    
    if(m_pWindow == nullptr) { return; }

    m_pGlCtx = SDL_GL_CreateContext(m_pWindow);

    if(m_pGlCtx == nullptr) { return; }

    SDL_GL_SetSwapInterval(1);

    // This should never be seen if the skybox is working correctly
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);

    m_projection = glm::perspectiveLH(90.0f * 0.01745329251994329576923690768489f, (float)w / (float)h, 0.2f, 200.0f);

    // SDL_ShowCursor(SDL_DISABLE);
    // SDL_SetRelativeMouseMode(SDL_TRUE);

    m_skybox = std::make_unique<Skybox>();
    m_world = std::make_unique<World>();
    m_player = std::make_unique<Player>();

    // Init successful
    m_bRunning = true;
}

void App::HandleEvents()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                m_bRunning = false;
                break;
            
            case SDL_KEYDOWN:
                // Quit
                if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                {
                    m_bRunning = false;
                }

                // Update player
                else
                {
                    m_player->OnKeyDown(event.key.keysym.scancode);
                }

                break;
            
            case SDL_KEYUP:
                // Toggle wireframe
                if(event.key.keysym.scancode == SDL_SCANCODE_TAB)
                {
                    Settings::wireframe = !Settings::wireframe;
                }

                break;
            case SDL_MOUSEBUTTONUP:
                m_player->OnMouseButtonUp(event.button.button);
                break;
            
        }
    }
}

App::~App()
{
    SDL_GL_DeleteContext(m_pGlCtx);
    SDL_DestroyWindow(m_pWindow);
    SDL_Quit();
}

void App::Run()
{
    glm::mat4 view(1.0f);

    while(m_bRunning)
    {   
        HandleEvents();
        m_skybox->Update();
        m_player->Update();
        m_world->Update(*m_player);

        m_player->CalcModel();

        view = glm::inverse(m_player->GetModel());
        glm::mat4 vp = m_projection * view;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        m_skybox->Draw(vp, m_player->GetPos());
        m_world->RenderSolid(vp, m_player->GetPos());
        m_player->Draw(vp);
        
        SDL_GL_SwapWindow(m_pWindow);
    }
}

void App::GLErrorCallback(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei length, const char *msg, const void *usrParams)
{
    std::cout << "OpenGL Error (" << id << "): ";
    std::cout << "\nMsg = [ " << msg << " ]\n";
}