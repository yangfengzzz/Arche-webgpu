#version 450

layout(location = 0) in vec4 COLOR0;
layout(location = 0) out vec4 o_color;

void main() {
    o_color = COLOR0;
}