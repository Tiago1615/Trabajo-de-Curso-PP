#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include <glad/glad.h>

inline std::string loadTextFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file){
        throw std::runtime_error("No se pudo abrir: " + path);
    }

    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

inline GLuint compileShader(GLenum type, const char* src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    int ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[512];
        glGetShaderInfoLog(s, 512, nullptr, log);
        std::cerr << log << std::endl;
    }
    return s;
}

inline GLuint createProgramFromFiles(const std::string& vertPath, const std::string& fragPath)
{
    GLuint vs = compileShader(GL_VERTEX_SHADER, loadTextFile(vertPath).c_str());
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, loadTextFile(fragPath).c_str());

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
