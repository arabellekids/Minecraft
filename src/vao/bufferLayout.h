#pragma once

#include <GLES3/gl32.h>
#include <vector>

struct VertexLayoutElement
{
    unsigned int type;
    unsigned int count;
    std::size_t size;
    bool normalized;
};

namespace detail {

    template<typename T>
    struct VertexLayoutInfo
    {
        constexpr static bool IsValid = false;
        constexpr static unsigned int Type = 0;
    };

    template<>
    struct VertexLayoutInfo<float>
    {
        constexpr static bool IsValid = true;
        constexpr static unsigned int Type = GL_FLOAT;
    };

    template<>
    struct VertexLayoutInfo<unsigned int>
    {
        constexpr static bool IsValid = true;
        constexpr static unsigned int Type = GL_UNSIGNED_INT;
    };

    template<>
    struct VertexLayoutInfo<unsigned char>
    {
        constexpr static bool IsValid = true;
        constexpr static unsigned int Type = GL_UNSIGNED_BYTE;
    };
}

class BufferLayout
{
private:
    std::vector<VertexLayoutElement> m_elements;
    unsigned int m_stride = 0;
public:
    BufferLayout() {}
    ~BufferLayout() {}

    inline const std::vector<VertexLayoutElement>& GetElements() const { return m_elements; }
    inline unsigned int GetStride() const { return m_stride; }

    template<typename T>
    void Push(unsigned int count, bool normalized = false)
    {
        static_assert(detail::VertexLayoutInfo<T>::IsValid);

        auto type = detail::VertexLayoutInfo<T>::Type;
        auto size = sizeof(T) * count;
        
        m_elements.push_back({ type, count, size, normalized });
        m_stride += size;
    }
};