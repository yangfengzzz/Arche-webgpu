#version 450

#include "base/common.h"

#define PI 3.14159265359
#define RECIPROCAL_PI 0.31830988618
#define EPSILON 1e-6
#define LOG2 1.442695

#define saturate(a) clamp(a, 0.0, 1.0)
#define whiteCompliment(a) (1.0 - saturate(a))

vec4 RGBMToLinear(vec4 value, float maxRange) {
    return vec4(value.rgb * value.a * maxRange, 1.0);
}

vec4 gammaToLinear(vec4 srgbIn){
    return vec4(pow(srgbIn.rgb, vec3(2.2)), srgbIn.a);
}

vec4 linearToGamma(vec4 linearIn){
    return vec4(pow(linearIn.rgb, vec3(1.0 / 2.2)), linearIn.a);
}

layout(location = Position) in vec3 POSITION;

#ifdef HAS_UV
    layout(location = UV_0) in vec2 TEXCOORD_0;
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifdef HAS_SKIN
    layout(location = Joints_0) in vec4 JOINTS_0;
    layout(location = Weights_0) in vec4 WEIGHTS_0;

    #ifdef HAS_JOINT_TEXTURE
        layout(set = 0, binding = 2) uniform sampler2D jointTexture;
        layout(set = 0, binding = 3) uniform jointCount {
            float value;
        } joint_count;

        mat4 getJointMatrix(sampler2D smp, float index) {
            float base = index / joint_count.value;
            float hf = 0.5 / joint_count.value;
            float v = base + hf;

            vec4 m0 = texture2D(smp, vec2(0.125, v));
            vec4 m1 = texture2D(smp, vec2(0.375, v));
            vec4 m2 = texture2D(smp, vec2(0.625, v));
            vec4 m3 = texture2D(smp, vec2(0.875, v));

            return mat4(m0, m1, m2, m3);
        }
    #else
        layout(set = 0, binding = 4) uniform jointMatrix {
            mat4 value[JOINTS_NUM];
        } joint_matrix;
    #endif
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifdef HAS_VERTEXCOLOR
    layout(location = Color_0) in vec4 COLOR_0;
#endif

layout(set = 0, binding = 5) uniform cameraData {
    mat4 view_mat;
    mat4 proj_mat;
    mat4 vp_mat;
    mat4 view_inv_mat;
    mat4 proj_inv_mat;
    vec3 camera_pos;
} camera_data;

layout(set = 0, binding = 6) uniform rendererData {
    mat4 local_mat;
    mat4 model_mat;
    mat4 normal_mat;
} renderer_data;

layout(set = 0, binding = 7) uniform tilingOffset {
    vec4 value;
} tiling_offset;

//----------------------------------------------------------------------------------------------------------------------
#ifndef OMIT_NORMAL
    #ifdef HAS_NORMAL
        layout(location = Normal) in vec3 NORMAL;
    #endif

    #ifdef HAS_TANGENT
        layout(location = Tangent) in vec4 TANGENT;
    #endif
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifdef HAS_BLENDSHAPE
    #ifndef HAS_BLENDSHAPE_TEXTURE
        layout(location = 10) in vec3 POSITION_BS0;
        layout(location = 11) in vec3 POSITION_BS1;
        layout(location = 12) in vec3 POSITION_BS2;
        layout(location = 13) in vec3 POSITION_BS3;

        #ifdef HAS_BLENDSHAPE_NORMAL
            layout(location = 16) in vec3 NORMAL_BS0;
            layout(location = 17) in vec3 NORMAL_BS1;
            layout(location = 18) in vec3 NORMAL_BS2;
            layout(location = 19) in vec3 NORMAL_BS3;
        #endif

        #ifdef HAS_BLENDSHAPE_TANGENT
            layout(location = 20) in vec3 TANGENT_BS0;
            layout(location = 21) in vec3 TANGENT_BS1;
            layout(location = 22) in vec3 TANGENT_BS2;
            layout(location = 23) in vec3 TANGENT_BS3;
        #endif
    #endif
    layout(set = 0, binding = 8) uniform blendShapeWeights {
        float value[4];
    } blend_shape_weights;
#endif

//----------------------------------------------------------------------------------------------------------------------
layout (location = 0) out vec2 v_uv;

#ifdef O3_HAS_VERTEXCOLOR
    layout (location = 1) out vec4 v_color;
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifdef HAS_NORMAL
    #if defined(HAS_TANGENT) && defined(NORMAL_TEXTURE)
        layout (location = 2) out mat3 v_TBN;
    #else
        layout (location = 3) out vec3 v_normal;
    #endif
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifdef NEED_WORLDPOS
    layout (location = 4) out vec3 v_pos;
#endif

#ifdef SHADOW_MAP_COUNT
    layout (location = 5) out vec3 view_pos;
#endif

//----------------------------------------------------------------------------------------------------------------------
//**********************************************************************************************************************
//----------------------------------------------------------------------------------------------------------------------
void main() {
    vec4 position = vec4(POSITION, 1.0);

    //------------------------------------------------------------------------------------------------------------------
    #ifdef HAS_NORMAL
        vec3 normal = vec3(NORMAL);
        #if defined(HAS_TANGENT) && defined(NORMAL_TEXTURE)
            vec4 tangent = vec4(TANGENT);
        #endif
    #endif

    //------------------------------------------------------------------------------------------------------------------
    #ifdef HAS_BLENDSHAPE
        #ifdef HAS_BLENDSHAPE_TEXTURE
            //todo
        #else
            position.xyz += POSITION_BS0 * blend_shape_weights.value[0];
            position.xyz += POSITION_BS1 * blend_shape_weights.value[1];
            position.xyz += POSITION_BS2 * blend_shape_weights.value[2];
            position.xyz += POSITION_BS3 * blend_shape_weights.value[3];

            #ifndef OMIT_NORMAL
                #if defined(HAS_NORMAL) && defined(HAS_BLENDSHAPE_NORMAL)
                    normal.xyz += NORMAL_BS0 * blend_shape_weights.value[0];
                    normal.xyz += NORMAL_BS1 * blend_shape_weights.value[1];
                    normal.xyz += NORMAL_BS2 * blend_shape_weights.value[2];
                    normal.xyz += NORMAL_BS3 * blend_shape_weights.value[3];
                #endif

                #if defined(HAS_TANGENT) && defined(NORMAL_TEXTURE) && defined(HAS_BLENDSHAPE_TANGENT)
                    tangent.xyz += TANGENT_BS0 * blend_shape_weights.value[0];
                    tangent.xyz += TANGENT_BS1 * blend_shape_weights.value[1];
                    tangent.xyz += TANGENT_BS2 * blend_shape_weights.value[2];
                    tangent.xyz += TANGENT_BS3 * blend_shape_weights.value[3];
                #endif
            #endif
        #endif
    #endif

    //------------------------------------------------------------------------------------------------------------------
    #ifdef HAS_SKIN
        #ifdef HAS_JOINT_TEXTURE
            mat4 skinMatrix =
            WEIGHTS_0.x * getJointMatrix(jointTexture, JOINTS_0.x) +
            WEIGHTS_0.y * getJointMatrix(jointTexture, JOINTS_0.y) +
            WEIGHTS_0.z * getJointMatrix(jointTexture, JOINTS_0.z) +
            WEIGHTS_0.w * getJointMatrix(jointTexture, JOINTS_0.w);
        #else
            mat4 skinMatrix =
            WEIGHTS_0.x * joint_matrix.value[int(JOINTS_0.x)] +
            WEIGHTS_0.y * joint_matrix.value[int(JOINTS_0.y)] +
            WEIGHTS_0.z * joint_matrix.value[int(JOINTS_0.z)] +
            WEIGHTS_0.w * joint_matrix.value[int(JOINTS_0.w)];
        #endif

        position = skinMatrix * position;

        #if defined(HAS_NORMAL) && !defined(OMIT_NORMAL)
            normal = vec4(skinMatrix * vec4(normal, 0.0)).xyz;
            #if defined(HAS_TANGENT) && defined(NORMAL_TEXTURE)
                tangent.xyz = vec4(skinMatrix * vec4(tangent.xyz, 0.0)).xyz;
            #endif
        #endif
    #endif

    //------------------------------------------------------------------------------------------------------------------
    #ifdef HAS_UV
        v_uv = TEXCOORD_0;
    #else
    // may need this calculate normal
        v_uv = vec2(0., 0.);
    #endif

    #ifdef NEED_TILINGOFFSET
        v_uv = v_uv * tiling_offset.value.xy + tiling_offset.value.zw;
    #endif

    //------------------------------------------------------------------------------------------------------------------
    #ifdef HAS_VERTEXCOLOR
        v_color = COLOR_0;
    #endif

    //------------------------------------------------------------------------------------------------------------------
    #ifdef HAS_NORMAL
        #if defined(HAS_TANGENT) && defined(NORMAL_TEXTURE)
            vec3 normalW = normalize(mat3(renderer_data.normal_mat) * normal.xyz);
            vec3 tangentW = normalize(mat3(renderer_data.normal_mat) * tangent.xyz);
            vec3 bitangentW = cross(normalW, tangentW) * tangent.w;
            v_TBN = mat3(tangentW, bitangentW, normalW);
        #else
            v_normal = normalize(mat3(renderer_data.normal_mat) * normal);
        #endif
    #endif

    //------------------------------------------------------------------------------------------------------------------
    #ifdef NEED_WORLDPOS
        vec4 temp_pos = renderer_data.model_mat * position;
        v_pos = temp_pos.xyz / temp_pos.w;
    #endif

    #ifdef SHADOW_MAP_COUNT
        view_pos = (camera_data.view_mat * renderer_data.model_mat * position).xyz;
    #endif

    //------------------------------------------------------------------------------------------------------------------
    gl_Position = camera_data.vp_mat * renderer_data.model_mat * position;
}