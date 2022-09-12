#version 450
#define Vert_Shader

#include "common.h"
#include "snippet/common_define.h"
#include "snippet/common_vert_define.h"
#include "snippet/blendShape_define.h"
#include "snippet/uv_define.h"
#include "snippet/color_define.h"
#include "snippet/normal_define.h"
#include "snippet/worldpos_define.h"

void main() {
#include "snippet/begin_position.vert"
#include "snippet/begin_normal.vert"
#include "snippet/blendShape.vert"
#include "snippet/skinning.vert"
#include "snippet/uv.vert"
#include "snippet/color.vert"
#include "snippet/normal.vert"
#include "snippet/worldpos.vert"
#include "snippet/position.vert"
}