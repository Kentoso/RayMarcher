#version 420 core
layout (location = 0) in vec3 aPos;
out vec2 fragCoord;
void main() {
    fragCoord = vec2(aPos.xy);
    gl_Position = vec4(aPos, 1.0);
}