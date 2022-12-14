#pragma once

#include <memory>

#include <GLES3/gl32.h>
#include <glm/mat4x4.hpp>

#include "../block/world.h"
#include "../object/player.h"
#include "../skybox/skybox.h"

typedef void* SDL_GLContext;
struct SDL_Window;

class App
{
private:
    static App* s_pInstance;

    bool m_bRunning;

    SDL_Window* m_pWindow;
    SDL_GLContext m_pGlCtx;

    glm::mat4 m_projection;

    std::unique_ptr<World> m_world;
    std::unique_ptr<Player> m_player;
    std::unique_ptr<Skybox> m_skybox;

    void GLErrorCallback(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei length, const char *msg, const void *usrParams);
    void HandleEvents();
public:
    App(int w, int h, bool fullscreen = false);
    ~App();

    static App& Instance() { return *s_pInstance; }

    void Run();

    bool IsRunning() const { return m_bRunning; }

    const World& GetWorld() const { return *m_world; }
    World& GetWorld() { return *m_world; }
    
    const Player& GetPlayer() const { return *m_player; }
    Player& GetPlayer() { return *m_player; }
};