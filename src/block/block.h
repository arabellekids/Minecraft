#pragma once

#include <glm/vec3.hpp>

#include "../vbo/vbo.h"

enum BlockSide
{
    BLOCK_SIDE_FRONT  = 0,
    BLOCK_SIDE_BACK   = 1,
    BLOCK_SIDE_LEFT   = 2,
    BLOCK_SIDE_RIGHT  = 3,
    BLOCK_SIDE_TOP    = 4,
    BLOCK_SIDE_BOTTOM = 5
};

const std::vector< std::vector<Vertex> >& GetBlockVertices();
const std::vector<glm::vec3>& GetBlockFaceCenters();

const std::vector<Vertex>& GetCubeVertices();
const std::vector<unsigned short>& GetCubeIndices();
