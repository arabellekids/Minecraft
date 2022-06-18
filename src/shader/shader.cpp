#include <GLES3/gl32.h>
#include <iostream>
#include <SDL2/SDL.h>

#include "shader.h"

Shader::Shader(const std::string& vert, const std::string& frag) : m_id(0)
{
    // Create the program
    m_id = glCreateProgram();

    // Load the Vertex and Fragment shaders
    unsigned int vertexShader = LoadShader(GL_VERTEX_SHADER, vert);
    unsigned int fragShader = LoadShader(GL_FRAGMENT_SHADER, frag);

    // Compile the shader
    CompileShader();

    // Delete the temp vert and frag shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);
}

Shader::~Shader()
{
    glDeleteProgram(m_id);
}

void Shader::Bind() const
{
    glUseProgram(m_id);
}

void Shader::UnBind() const
{
    glUseProgram(0);
}

int Shader::GetUniformLocation(const std::string& name) const
{
    if(m_uniformLocations.find(name) != m_uniformLocations.end())
    {
        return m_uniformLocations[name];
    }

    int loc = glGetUniformLocation(m_id, name.c_str());
    m_uniformLocations[name] = loc;

    if(loc == -1)
    {
        std::cout << "[WARNING] Uniform: " << name << " not found!\n";
    }

    return loc;
}

void Shader::SetUniform1i(const std::string& name, int value) const { glUniform1i(GetUniformLocation(name), value); }
void Shader::SetUniformMat4(const std::string& name, const glm::mat4& value) const { glUniformMatrix4fv(GetUniformLocation(name), 1, false, &value[0][0]); }

unsigned int Shader::LoadShader(unsigned int type, const std::string& path)
{
    // Load the file
    void* file = SDL_LoadFile(path.c_str(), nullptr);

    if(file == nullptr)
    {
        if(type == GL_VERTEX_SHADER)
        {
            std::cerr << "Failed to load Vertex Shader source code at: " << path << "\n";
        }
        else if(type == GL_FRAGMENT_SHADER)
        {
            std::cerr << "Failed to load Fragment Shader source code at: " << path << "\n";
        }

        return 0;
    }

    // Create the shader
    unsigned int shader = glCreateShader(type);
    
    // Attach the shader
    glAttachShader(m_id, shader);
    
    const char* src[1] = { (const char*)file };

    // Set the source code
    glShaderSource(shader, 1, src, nullptr);

    // Compile the shader
    glCompileShader(shader);

    // Free the file
    SDL_free(file);

    // Make sure the shader compiled
    int status = 1;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if(status == GL_FALSE)
    {
        // Shader compile failed, print the debug info
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        char* info = (char*)alloca(length);
        glGetShaderInfoLog(shader, length, nullptr, info);

        if(type == GL_VERTEX_SHADER)
        {
            std::cerr << "Failed to compile Vertex Shader at: " << path \
            << "\nDebug Info=[\n" << info << "\n]\n";
        }
        else if(type == GL_FRAGMENT_SHADER)
        {
            std::cerr << "Failed to compile Fragment Shader at: " << path \
            << "\nDebug Info=[\n" << info << "\n]\n";
        }
        
        return 0;
    }

    // Shader loading complete!
    return shader;
}

bool Shader::CompileShader()
{
    // Compile the program
    glLinkProgram(m_id);
    glValidateProgram(m_id);

    // Make sure the program compiled
    int status = 1;
    glGetProgramiv(m_id, GL_LINK_STATUS, &status);

    if(status == GL_FALSE)
    {
        // Failed to link program, print debug info
        int length;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);

        char* info = (char*)alloca(length);
        glGetProgramInfoLog(m_id, length, nullptr, info);

        std::cerr << "Failed to link Program: " << m_id
        << "\nDebug Info=[\n" << info << "\n]\n";

        return false;
    }

    // Program successfully compiled!
    return true;
}
