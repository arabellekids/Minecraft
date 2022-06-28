#include "settings.h"

unsigned int Settings::viewDist = 5;
bool Settings::wireframe = false;

siv::BasicPerlinNoise<float> Settings::noise = siv::BasicPerlinNoise<float>(0);

std::vector<glm::vec2> Settings::baseTerrainGraph = {
    glm::vec2( 0.0f,  1.0f ),
    glm::vec2( 0.25f, 1.0f ),
    glm::vec2( 0.75f, 0.0f ),
    glm::vec2( 1.0f,  0.0f )
};