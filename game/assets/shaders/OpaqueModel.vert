#version 460

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

layout(location = 3) in vec4 aInstanceColor;
layout(location = 4) in mat4 aInstanceModel;

layout(std140, binding = 0) uniform CommonUniform
{
	uint Width;
	uint Height;
	float CurTime;
	float FrameTime;
	mat4 View;
	mat4 Projection;
	float Near;
	float Far;
} uCommon;

out VP_Shared {
	vec3 pFragPos;
	float pDepth;
    vec3 pNormal;
    vec2 pUV;
	vec4 pInstanceColor;
};

void main()
{
	pFragPos = vec3(aInstanceModel * vec4(aPos, 1.0));

	vec4 viewPos = uCommon.View * vec4(pFragPos, 1.0);
	gl_Position = uCommon.Projection * viewPos;

    pDepth = 1.0 - (-viewPos.z - uCommon.Near) / (uCommon.Far - uCommon.Near);

	pNormal = transpose(mat3(inverse(aInstanceModel))) * aNormal;
	pUV = aUV;

	pInstanceColor = aInstanceColor;
}
