vec4 GetAmbient(vec3 _world_normal) {
    vec3 normal = normalize(_world_normal);
    vec3 alpha = (normal + 1.) * .5;
    vec2 bt = mix(vec2(.3, .7), vec2(.4, .8), alpha.xz);
    vec3 ambient = mix(vec3(bt.x, .3, bt.x), vec3(bt.y, .8, bt.y), alpha.y);
    return vec4(ambient, 1.);
};

varying vec3 v_world_normal;
varying vec4 v_vertex_color;

void main() {
  vec4 ambient = GetAmbient(v_world_normal);
  gl_FragColor = ambient * v_vertex_color;
};