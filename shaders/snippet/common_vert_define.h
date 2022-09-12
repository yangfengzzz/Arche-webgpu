layout(location = Position) in vec3 POSITION;

#ifdef HAS_UV
    layout(location = UV_0) in vec2 TEXCOORD_0;
#endif

#ifdef HAS_UV1
    layout(location = UV_1) in vec2 TEXCOORD_1;
#endif

#ifdef HAS_SKIN
    layout(location = Joints_0) in vec4 JOINTS_0;
    layout(location = Weights_0) in vec4 WEIGHTS_0;

    #ifdef USE_JOINT_TEXTURE
        layout(set = 0, binding = Joint_Texture_Location) uniform texture2D u_jointTexture;
        layout(set = 0, binding = Joint_Sampler_Location) uniform sampler u_jointSampler;
        layout(set = 0, binding = Joint_Count_Location) uniform float u_jointCount;

        mat4 getJointMatrix(sampler2D smp, float index) {
            float base = index / u_jointCount;
            float hf = 0.5 / u_jointCount;
            float v = base + hf;

            vec4 m0 = texture(smp, vec2(0.125, v ));
            vec4 m1 = texture(smp, vec2(0.375, v ));
            vec4 m2 = texture(smp, vec2(0.625, v ));
            vec4 m3 = texture(smp, vec2(0.875, v ));

            return mat4(m0, m1, m2, m3);
        }

    #else
        layout(set = 0, binding = Joint_Matrix_Location) uniform mat4 u_jointMatrix[ JOINTS_NUM ];
    #endif
#endif

#ifdef HAS_VERTEXCOLOR
    layout(location = Color_0) vec4 COLOR_0;
#endif

layout(set = 0, binding = Camera_Location) uniform u_cameraData {
    mat4 u_viewMat;
    mat4 u_projMat;
    mat4 vp_mat;
    mat4 view_inv_mat;
    mat4 proj_inv_mat;
    vec3 u_cameraPos;
};

layout(set = 0, binding = Renderer_Location) uniform u_rendererData {
    mat4 u_localMat;
    mat4 u_modelMat;
    mat4 u_normalMat;
};

layout(set = 0, binding = Tilling_Offset_Location) uniform u_tilingOffset {
    vec4 tilingOffset;
};

#ifndef OMIT_NORMAL
    #ifdef HAS_NORMAL
        layout(location = Normal) in vec3 NORMAL;
    #endif

    #ifdef HAS_TANGENT
        layout(location = Tangent) in vec4 TANGENT;
    #endif
#endif