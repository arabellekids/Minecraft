#include <vector>
#include <glm/vec2.hpp>

#include "../../vendor/perlin/PerlinNoise.hpp"

class Settings
{
public:
    // The view distance in chunks
    static unsigned int viewDist;

    // Wireframe render mode
    static bool wireframe;

    static siv::BasicPerlinNoise<float> noise;

    static std::vector<glm::vec2> baseTerrainGraph;
};