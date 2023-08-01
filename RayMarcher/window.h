#pragma once

#include <iostream>
#include <ostream>
#include <glad/glad.h>

#include "SDL2/SDL.h"

class window
{
public:
    window(const int width, const int height)
    {
        width_ = width;
        height_ = height;
        
        SDL_Init(SDL_INIT_VIDEO);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        window_ = SDL_CreateWindow(
          "SDL2Test",
          SDL_WINDOWPOS_UNDEFINED,
          SDL_WINDOWPOS_UNDEFINED,
          width,
          height,
          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
        );

        if (window_ == nullptr)
        {
            std::cout << "Failed to create SDL2 window" << std::endl;
            SDL_Quit();
        }
    
        gl_context_ = SDL_GL_CreateContext(window_);

        if (gl_context_ == nullptr)
        {
            std::cout << "Failed to create GL context" << std::endl;
            SDL_Quit();
        }
        
        SDL_GL_MakeCurrent(window_, gl_context_);

        if (!gladLoadGLLoader(static_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
        {
            std::cout << "Failed to init GLAD" << std::endl;
            SDL_Quit();
        }
    }
    void update_size()
    {
        SDL_GetWindowSize(window_, &width_, &height_);
    }
    int get_width() const
    {
        return width_;
    }
    int get_height() const
    {
        return height_;
    }
    void swap_window() const
    {
        SDL_GL_SwapWindow(window_);
    }
    void quit() const
    {
        SDL_DestroyWindow(window_);
        SDL_Quit();
    }
private:
    SDL_Window* window_;
    SDL_GLContext gl_context_;
    int width_;
    int height_;
};
