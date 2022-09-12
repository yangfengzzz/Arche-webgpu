#version 450
#define Frag_Shader

#include "common.h"
#include "snippet/common_define.h"
#include "snippet/common_frag_define.h"

#include "snippet/uv_define.h"
#include "snippet/normal_define.h"
#include "snippet/color_define.h"
#include "snippet/worldpos_define.h"

#include "snippet/fog_define.h"
#include "snippet/light_define.h"
#include "snippet/normal_frag_define.h"

#include "snippet/pbr/pbr_frag_define.h"
#include "snippet/pbr/pbr_helper.h"
#include "snippet/pbr/brdf.h"
#include "snippet/pbr/direct_irradiance_frag_define.h"
#include "snippet/pbr/ibl_frag_define.h"

layout(location = 0) out vec4 o_color;

void main() {
#include "snippet/pbr/pbr_frag.h"
#include "snippet/fog_frag.h"
}