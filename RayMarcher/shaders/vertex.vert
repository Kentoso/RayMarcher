#version 420 core
layout (location = 0) in vec3 aPos;
out vec4 vertex_color;
void main() {
    gl_Position = vec4(aPos, 1.0);
    vertex_color = vec4(vec2(aPos.xy), 1.0, 1.0);
}