#version 450

layout(location = 0) flat in uint v_color;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(((v_color & 0xff0000) >> 16) / 255.0, ((v_color & 0xff00) >> 8) / 255.0, (v_color & 0xff) / 255.0, 1.0);
}