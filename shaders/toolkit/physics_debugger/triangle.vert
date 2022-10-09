#version 450

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNorm;
layout(location = 2) in vec2 vTex;
layout(location = 3) in vec4 vCol;

layout(location = 4) in vec4 iModel1;
layout(location = 5) in vec4 iModel2;
layout(location = 6) in vec4 iModel3;
layout(location = 7) in vec4 iModel4;

layout(location = 8) in vec4 iModelInvTrans1;
layout(location = 9) in vec4 iModelInvTrans2;
layout(location = 10) in vec4 iModelInvTrans3;
layout(location = 11) in vec4 iModelInvTrans4;

layout(location = 12) in vec4 iCol;

layout(set = 0, binding = 10) uniform u_mvpNoscale {
    mat4 mvp_no_scale;
};

layout (location = 0) out vec3 Normal;
layout (location = 1) out vec3 WorldPos;
layout (location = 2) out vec2 Tex;
layout (location = 4) out vec4 Color;

void main() {
	// Get world position
	vec4 pos = vec4(vPos, 1.0f);
	mat4 iModel = mat4(iModel1, iModel2, iModel3, iModel4);
	mat4 iModelInvTrans = mat4(iModelInvTrans1, iModelInvTrans2, iModelInvTrans3, iModelInvTrans4);

	vec4 world_pos = iModel * pos;

    // Transform the position from world space to homogeneous projection space
    gl_Position = mvp_no_scale * world_pos;

	// output normal
	vec4 norm = vec4(vNorm, 0.0f);
	Normal = normalize((iModelInvTrans * norm).xyz);

	// output world position of the vertex
	WorldPos = world_pos.xyz/world_pos.w;

	// output texture coordinates
	Tex = vTex;

	// output color
	Color = vCol * iCol;
}