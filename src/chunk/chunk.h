#pragma once

#include <array>

// Block scale
const float BS = 1.0f;

template<typename T, std::size_t x, std::size_t y, std::size_t z> struct grid3D : std::array<std::array<std::array<T, x>, y>, z> {};

class Chunk
{
private:
    grid3D<unsigned char, 16,16,32> m_blocks;
public:
    void GenerateBuffers();
};