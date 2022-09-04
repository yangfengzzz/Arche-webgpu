#version 450

layout(set = 0, binding = 10) uniform baseColor {
    vec4 base_color;
};

layout(location = 0) out vec4 o_color;

void main() {
    o_color = base_color;
}