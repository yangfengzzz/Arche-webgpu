#version 450

layout (location = 0) in vec2 localPos;
layout (location = 1) in vec3 color;
layout(location = 0) out vec4 o_color;

void main() {
    float distToCenter = length(localPos);
    float fade = (1.0 - distToCenter) * (1.0 / (distToCenter * distToCenter));
    o_color = vec4(color * fade, fade);
}