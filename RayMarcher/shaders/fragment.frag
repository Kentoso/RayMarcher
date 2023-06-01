#version 420 core
in vec4 vertex_color;
out vec4 FragColor;
uniform vec4 our_color;

void main() {
//    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
//    FragColor = vertex_color;
    FragColor = our_color;
}