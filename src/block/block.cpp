#include "block.h"

static float u1 = 0.0001f;
static float u2 = 0.0624f;

static float v1 = 0.0001f;
static float v2 = 0.0624f;

static std::vector< std::vector<Vertex> > g_blockVerts = {
    // North = +Z
    {
        { 1.0, 1.0, 1.0, u1, v1, 0.8f },
        { 1.0, 0.0, 1.0, u1, v2, 0.8f },
        { 0.0, 0.0, 1.0, u2, v2, 0.8f },
        { 0.0, 1.0, 1.0, u2, v1, 0.8f }
    },

    // South = -Z
    {
        { 0.0, 1.0, 0.0, u1, v1, 0.8f },
        { 0.0, 0.0, 0.0, u1, v2, 0.8f },
        { 1.0, 0.0, 0.0, u2, v2, 0.8f },
        { 1.0, 1.0, 0.0, u2, v1, 0.8f }
    },

    // West = -X
    {
        { 0.0, 1.0, 1.0, u1, v1, 0.9f },
        { 0.0, 0.0, 1.0, u1, v2, 0.9f },
        { 0.0, 0.0, 0.0, u2, v2, 0.9f },
        { 0.0, 1.0, 0.0, u2, v1, 0.9f }
    },

    // East = +X
    {
        { 1.0, 1.0, 0.0, u1, v1, 0.9f },
        { 1.0, 0.0, 0.0, u1, v2, 0.9f },
        { 1.0, 0.0, 1.0, u2, v2, 0.9f },
        { 1.0, 1.0, 1.0, u2, v1, 0.9f }
    },

    // Up = +Y
    {
        { 0.0, 1.0, 1.0, u1, v1, 1.0f },
        { 0.0, 1.0, 0.0, u1, v2, 1.0f },
        { 1.0, 1.0, 0.0, u2, v2, 1.0f },
        { 1.0, 1.0, 1.0, u2, v1, 1.0f }
    },

    // Down = -Y
    {
        { 0.0, 0.0, 0.0, u1, v1, 0.6f },
        { 0.0, 0.0, 1.0, u1, v2, 0.6f },
        { 1.0, 0.0, 1.0, u2, v2, 0.6f },
        { 1.0, 0.0, 0.0, u2, v1, 0.6f }
    }
};

static std::vector<glm::vec3> g_blockFaceCenters = {
    {0.5f, 0.5f, 1.0f},
    {0.5f, 0.5f, 0.0f},
    {0.0f, 0.5f, 0.5f},
    {1.0f, 0.5f, 0.5f},
    {0.5f, 1.0f, 0.5f},
    {0.5f, 0.0f, 0.5f}
};

const std::vector< std::vector<Vertex> >& GetBlockVertices()
{
    return g_blockVerts;
}

const std::vector<glm::vec3>& GetBlockFaceCenters()
{
    return g_blockFaceCenters;
}