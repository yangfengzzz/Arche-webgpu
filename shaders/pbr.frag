#version 450
#define Frag_Shader
#define IS_METALLIC_WORKFLOW

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

#include "pbr/pbr_frag_define.h"
#include "pbr/pbr_helper.h"
#include "pbr/brdf.h"
#include "pbr/direct_irradiance_frag_define.h"
#include "pbr/ibl_frag_define.h"
#include "shadow/shadow_frag_define.h"

layout(location = 0) out vec4 o_color;

void main() {
#include "pbr/pbr.frag"
#include "snippet/fog.frag"
}