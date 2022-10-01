#version 450

vec4 GetAmbient(vec3 _world_normal) {
    vec3 normal = normalize(_world_normal);
    vec3 alpha = (normal + 1.) * .5;
    vec2 bt = mix(vec2(.3, .7), vec2(.4, .8), alpha.xz);
    vec3 ambient = mix(vec3(bt.x, .3, bt.x), vec3(bt.y, .8, bt.y), alpha.y);
    return vec4(ambient, 1.);
}

layout (location = 0) in vec3 v_world_normal;
layout (location = 1) in vec4 v_vertex_color;
layout(location = 0) out vec4 o_color;

void main() {
  vec4 ambient = GetAmbient(v_world_normal);
  o_color = ambient * v_vertex_color;
}