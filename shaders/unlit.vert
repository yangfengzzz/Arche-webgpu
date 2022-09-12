#version 450
#define Vert_Shader

#include "common.h"
#include "snippet/common_vert_define.h"
#include "snippet/blendShape_define.h"
#include "snippet/uv_define.h"

void main() {
#include "snippet/begin_position.vert"
#include "snippet/blendShape.vert"
#include "snippet/skinning.vert"
#include "snippet/uv.vert"
#include "snippet/position.vert"
}