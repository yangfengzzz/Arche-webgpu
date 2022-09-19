#version 450
#define Frag_Shader

#include "common.h"
#include "snippet/common_define.h"
#include "snippet/common_frag_define.h"

#include "snippet/uv_define.h"
#include "snippet/normal_define.h"
#include "snippet/color_define.h"
#include "snippet/worldpos_define.h"

#include "snippet/light_define.h"
#include "snippet/mobile_material_define.h"
#include "snippet/normal_frag_define.h"
#include "shadow/shadow_frag_define.h"

layout(location = 0) out vec4 o_color;

void main() {
#include "snippet/begin_mobile.frag"
#include "snippet/begin_viewdir.frag"
#include "snippet/mobile_blinnphong.frag"

    //------------------------------------------------------------------------------------------------------------------
    o_color = emission + ambient + diffuse + specular;
    o_color.a = diffuse.a;

    #ifndef COLORSPACE_GAMMA
        o_color = linearToGamma(o_color);
    #endif
}