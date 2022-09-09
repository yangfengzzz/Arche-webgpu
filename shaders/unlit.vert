#version 450

#include "common.h"
#include "snippet/common_vert_define.h"
#include "snippet/blendShape_define.h"
#include "snippet/uv_define.h"

void main() {
#include "snippet/begin_position.vert"

    #ifdef HAS_BLENDSHAPE
        #ifdef HAS_BLENDSHAPE_TEXTURE

        #else
            position.xyz += POSITION_BS0 * blend_shape_weights[0];
            position.xyz += POSITION_BS1 * blend_shape_weights[1];
            position.xyz += POSITION_BS2 * blend_shape_weights[2];
            position.xyz += POSITION_BS3 * blend_shape_weights[3];

            #ifndef OMIT_NORMAL
                #if defined(HAS_NORMAL) && defined(HAS_BLENDSHAPE_NORMAL)
                    normal.xyz += NORMAL_BS0 * blend_shape_weights[0];
                    normal.xyz += NORMAL_BS1 * blend_shape_weights[1];
                    normal.xyz += NORMAL_BS2 * blend_shape_weights[2];
                    normal.xyz += NORMAL_BS3 * blend_shape_weights[3];
                #endif

                #if defined(HAS_TANGENT) && defined(NORMAL_TEXTURE) && defined(HAS_BLENDSHAPE_TANGENT)
                    tangent.xyz += TANGENT_BS0 * blend_shape_weights[0];
                    tangent.xyz += TANGENT_BS1 * blend_shape_weights[1];
                    tangent.xyz += TANGENT_BS2 * blend_shape_weights[2];
                    tangent.xyz += TANGENT_BS3 * blend_shape_weights[3];
                #endif
            #endif
        #endif
    #endif

    //------------------------------------------------------------------------------------------------------------------
    #ifdef HAS_SKIN
        #ifdef HAS_JOINT_TEXTURE
            mat4 skinMatrix =
            WEIGHTS_0.x * getJointMatrix(jointSampler, JOINTS_0.x) +
            WEIGHTS_0.y * getJointMatrix(jointSampler, JOINTS_0.y) +
            WEIGHTS_0.z * getJointMatrix(jointSampler, JOINTS_0.z) +
            WEIGHTS_0.w * getJointMatrix(jointSampler, JOINTS_0.w);
        #else
            mat4 skinMatrix =
            WEIGHTS_0.x * joint_matrix[int(JOINTS_0.x)] +
            WEIGHTS_0.y * joint_matrix[int(JOINTS_0.y)] +
            WEIGHTS_0.z * joint_matrix[int(JOINTS_0.z)] +
            WEIGHTS_0.w * joint_matrix[int(JOINTS_0.w)];
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
        v_uv = v_uv * tilingOffset.xy + tilingOffset.zw;
    #endif

    //------------------------------------------------------------------------------------------------------------------
    gl_Position = vp_mat * u_modelMat * position;
}