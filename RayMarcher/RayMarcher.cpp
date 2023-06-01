#include <iostream>
#include <fstream>
#include <sstream>

#include "shader.h"
#include "glad/glad.h"
#include "SDL2/SDL.h"
#include "window.h"

void process_events(const SDL_Event& event, bool& done)
{
    switch (event.type)
    {
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
            done = true;
        break;
    case SDL_QUIT:
        done = true;
        break;
    case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            glViewport(0, 0, event.window.data1, event.window.data2);
            std::cout << event.window.data1 << " " << event.window.data2 << std::endl;
        }
        break;
    default:
        break;
    }
}

int main(int argc, char *argv[])
{
    const window main(800, 600);

    float vertices[] = {
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    
    glViewport(0, 0, main.get_width(), main.get_height());

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
    glEnableVertexAttribArray(0);
    
    shader main_shader("shaders/vertex.vert", "shaders/fragment.frag");
    
    bool done = false;
    while (!done)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event)) process_events(event, done);

        // Render
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        main_shader.use();
        double time_value = SDL_GetTicks() / 1000.0f;
        float green_value = (sin(time_value) / 2.0f) + 0.5f;
        main_shader.set_v4("our_color", 0.f, green_value, 0.f, 1.f);
        
        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        main.swap_window();
    }
    
    main.quit();

    return 0;
}


