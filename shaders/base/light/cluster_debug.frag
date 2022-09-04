#version 450

#include "base/light/cluster_common.comp"

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 o_color;

layout(set = 0, binding = 10) uniform clusterUniform {
    vec4 value;
} cluster_uniform;

layout(binding = 12) buffer clusterLights {
    ClusterLightGroup value;
} cluster_lights;

void main() {
    uint clusterIndex = getClusterIndex(cluster_uniform.value, gl_FragCoord);
    uint lightCount = cluster_lights.value.lights[clusterIndex].point_count + cluster_lights.value.lights[clusterIndex].spot_count;
    float lightFactor = float(lightCount) / float(MAX_LIGHTS_PER_CLUSTER);

    o_color =  mix(vec4(0.0, 0.0, 1.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0),
    vec4(lightFactor, lightFactor, lightFactor, lightFactor));
}