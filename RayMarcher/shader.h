#pragma once
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <iostream>

class shader
{
public:
    shader(const char* vertex_path, const char* fragment_path)
    {
        std::stringstream buffer;
        int success;
        char info_log[512];
        std::string vertex_code;
        std::string fragment_code;
        try
        {
            // vertex code
            // const std::ifstream vertex_src("shaders/vertex.vert");
            const std::ifstream vertex_src(vertex_path);
            buffer << vertex_src.rdbuf();
            vertex_code = buffer.str();

            // clear buffer
            buffer.str(std::string());
            buffer.clear();

            // fragment code
            const std::ifstream fragment_src(fragment_path);
            buffer << fragment_src.rdbuf();
            fragment_code = buffer.str();
        }
        catch (std::ifstream::failure&)
        {
            std::cout << "SHADER READ FAIL" << std::endl;
        }
        

        // vertex shader
        const unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        const char* vertex_text = vertex_code.c_str();
        glShaderSource(vertex_shader, 1, &vertex_text, nullptr);
        glCompileShader(vertex_shader);
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
            std::cout << success << "\n" << info_log << std::endl;
        }
        
        // fragment shader
        const unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        const auto fragment_text = fragment_code.c_str();
        glShaderSource(fragment_shader, 1, &fragment_text, nullptr);
        glCompileShader(fragment_shader);
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
            std::cout << success << "\n" << info_log << std::endl;
        }
        
        id_ = glCreateProgram();
        
        glAttachShader(id_, vertex_shader);
        glAttachShader(id_, fragment_shader);
        glLinkProgram(id_);
        glGetProgramiv(id_, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(id_, 512, nullptr, info_log);
            std::cout << success << "\n" << info_log << std::endl;
        }
        // glUseProgram(id_);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }
    unsigned int get_id() const
    {
        return id_;
    }
    void use() const
    {
        glUseProgram(id_);
    }
    void set_bool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(id_, name.c_str()), static_cast<int>(value));
    }
    void set_int(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
    }
    void set_float(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
    }
    void set_v2(const std::string& name, float v1, float v2) const
    {
        glUniform2f(glGetUniformLocation(id_, name.c_str()), v1, v2);
    }
    void set_v3(const std::string& name, float v1, float v2, float v3) const
    {
        glUniform3f(glGetUniformLocation(id_, name.c_str()), v1, v2, v3);
    }
    void set_v4(const std::string& name, float v1, float v2, float v3, float v4) const
    {
        glUniform4f(glGetUniformLocation(id_, name.c_str()), v1, v2, v3, v4);
    }
private:
    unsigned int id_;
};
