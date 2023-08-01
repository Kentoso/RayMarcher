#include <iostream>
#include <fstream>
#include <sstream>

#include "shader.h"
#include "vec3.h"
#include "glad/glad.h"
#include "SDL2/SDL.h"
#include "window.h"

void process_events(window w, const SDL_Event& event, bool& done, shader& s, vec3& look_direction, vec3& look_at, vec3 look_from)
{
    float angular_speed = 0.01f;
    float angle = 0.;
    switch (event.type)
    {
    // case SDL_MOUSEMOTION:
    //     SDL_Event next_e;
    //     SDL_PumpEvents();
    //     if (SDL_PeepEvents(&next_e, 1, SDL_PEEKEVENT, SDL_MOUSEMOTION, SDL_MOUSEMOTION) <= 0)
    //     {
    //         break;
    //     }
    //     angle = angular_speed * event.motion.yrel;
    //     look_direction = vec3(look_direction.x() * cos(angle) + look_direction.z() * sin(angle),
    //                         look_direction.y(),
    //                         -look_direction.x() * sin(angle) + look_direction.z() * cos(angle));
    //     look_at = look_from + look_direction;
    //     break;
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
            w.update_size();
            s.set_v2("iResolution", w.get_width(), w.get_height());
        }
        break;
    default:
        break;
    }
}

int main(int argc, char *argv[])
{
    const window main(800, 600);
    vec3 look_from(0., 0., 2.);
    vec3 look_direction(0., 0., -1.);
    
    vec3 look_at = look_from + look_direction;
    vec3 look_up(0., 1., 0.);
    // vec3 axis = unit_vector(cross(look_direction, look_up));
    // look_direction = look_direction.rotate_by(axis, 3.14159 / 2.);
    float eps = 0.01f;
    float angular_speed = 0.01f;

    float vertices[] = {
        1.f, 1.f, 0.0f,
        1.f, -1.f, 0.0f,
        -1.f, -1.f, 0.0f,
        -1.f, 1.f, 0.0f
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
    
    shader main_shader("shaders/vertex.vert", "shaders/fractal.frag");
    main_shader.use();
    main_shader.set_v2("iResolution", main.get_width(), main.get_height());
    // main_shader.set_v3("ball_position", )
    // SDL_SetRelativeMouseMode(SDL_TRUE);
    bool done = false;
    while (!done)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event)) process_events(main, event, done, main_shader, look_direction, look_at, look_from);
        auto k_state = SDL_GetKeyboardState(nullptr);
        if (k_state[SDL_SCANCODE_W])
        {
            look_from += look_direction * eps;
            look_at = look_from + look_direction;
        }
        if (k_state[SDL_SCANCODE_S])
        {
            look_from += -look_direction * eps;
            look_at = look_from + look_direction;
        }
        if (k_state[SDL_SCANCODE_A])
        {
            vec3 side = unit_vector(cross(look_direction, look_up));
            look_from += -side * eps;
            look_at = look_from + look_direction;

            // vec3 look_direction_norm = look_direction;
            // look_direction = vec3(look_direction_norm.x() * cos(angular_speed) + look_direction_norm.z() * sin(angular_speed),
            //                 look_direction_norm.y(),
            //                 -look_direction_norm.x() * sin(angular_speed) + look_direction_norm.z() * cos(angular_speed));
            // look_at = look_from + look_direction;
        }
        if (k_state[SDL_SCANCODE_D])
        {
            vec3 side = unit_vector(cross(look_direction, look_up));
            look_from += side * eps;
            look_at = look_from + look_direction;
            // vec3 look_direction_norm = unit_vector(look_direction);
            // look_direction = vec3(look_direction_norm.x() * cos(-angular_speed) + look_direction_norm.z() * sin(-angular_speed),
            //                 look_direction_norm.y(),
            //                 -look_direction_norm.x() * sin(-angular_speed) + look_direction_norm.z() * cos(-angular_speed));
            // look_at = look_from + look_direction;
        }
        if (k_state[SDL_SCANCODE_LEFT])
        {
            vec3 look_direction_proj = vec3(look_direction.x(), 0., look_direction.z()); 
            look_direction_proj = vec3(look_direction_proj.x() * cos(angular_speed) + look_direction_proj.z() * sin(angular_speed),
                            look_direction_proj.y(),
                            -look_direction_proj.x() * sin(angular_speed) + look_direction_proj.z() * cos(angular_speed));
            look_direction = vec3(look_direction_proj.x(), look_direction.y(), look_direction_proj.z());
            look_at = look_from + look_direction;
        }
        if (k_state[SDL_SCANCODE_RIGHT])
        {
            vec3 look_direction_proj = vec3(look_direction.x(), 0., look_direction.z()); 
            look_direction_proj = vec3(look_direction_proj.x() * cos(angular_speed) + look_direction_proj.z() * sin(-angular_speed),
                            look_direction_proj.y(),
                            -look_direction_proj.x() * sin(-angular_speed) + look_direction_proj.z() * cos(angular_speed));
            look_direction = vec3(look_direction_proj.x(), look_direction.y(), look_direction_proj.z());
            look_at = look_from + look_direction;
        }
        if (k_state[SDL_SCANCODE_DOWN])
        {
            vec3 axis = unit_vector(cross(look_up, look_direction));
            look_direction = look_direction.rotate_by(axis, angular_speed);
            look_at = look_from + look_direction;
            // vec3 look_direction_proj = vec3(0., look_direction.y(), look_direction.z()); 
            // look_direction_proj = vec3(look_direction_proj.x(),
            //                 look_direction_proj.y() * cos(angular_speed) - look_direction_proj.z() * sin(angular_speed),
            //                 look_direction_proj.y() * sin(angular_speed) + cos(angular_speed) * look_direction_proj.z());
            // look_direction = vec3(look_direction.x(), look_direction_proj.y(), look_direction_proj.z());
            // look_at = look_from + look_direction;
        }
        if (k_state[SDL_SCANCODE_UP])
        {
            vec3 axis = unit_vector(cross(look_up, look_direction));
            look_direction = look_direction.rotate_by(axis, -angular_speed);
            look_at = look_from + look_direction;
            // vec3 look_direction_proj = unit_vector(vec3(0., look_direction.y(), look_direction.z())); 
            // look_direction_proj = vec3(look_direction_proj.x(),
            //                 look_direction_proj.y() * cos(angular_speed) - look_direction_proj.z() * sin(-angular_speed),
            //                 look_direction_proj.y() * sin(-angular_speed) + cos(angular_speed) * look_direction_proj.z());
            // look_direction = unit_vector(vec3(look_direction.x(), look_direction_proj.y(), look_direction_proj.z()));
            // look_at = look_from + look_direction;
        }
        // std::cout << "LOOK_DIRECTION: " << look_direction.x() << " " << look_direction.y() << " " << look_direction.z() << std::endl;
        // std::cout << "LOOK_FROM: " << look_from.x() << " " << look_from.y() << " " << look_from.z() << std::endl;
        // if (event.key.keysym.sym == SDLK_w)
        // {
        //     look_from += look_at * eps;
        //     // look_at += look_at;
        // }
        // else if (event.key.keysym.sym == SDLK_s)
        // {
        //     look_from += -look_at * eps;
        //     // look_at += -look_at;
        // }
        // Render
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        main_shader.use();
        
        double time_value = SDL_GetTicks() / 1000.0f;
        main_shader.set_float("iTime", time_value);
        main_shader.set_v3("cam_from", look_from.x(), look_from.y(), look_from.z());
        main_shader.set_v3("cam_at", look_at.x(), look_at.y(), look_at.z());
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        main.swap_window();
    }
    
    main.quit();

    return 0;
}


