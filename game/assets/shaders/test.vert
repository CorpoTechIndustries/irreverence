#version 460

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

layout(location = 3) in vec4 aInstanceColor;
layout(location = 4) in mat4 aInstanceModel;

layout(std140, binding = 0) uniform GlobalUniform
{
	uint Width;
	uint Height;
	float CurTime;
	float FrameTime;
	mat4 View;
	mat4 Projection;
} uGlobal;

layout(std140, binding = 3) uniform AnimationUniform
{
	mat4 uBoneMatrices[255];
};

out VP_Shared {
	vec3 pFragPos;
    vec3 pNormal;
    vec2 pUV;
	vec4 pInstanceColor;
};

void main()
{
	pFragPos = vec3(aInstanceModel * vec4(aPos, 1.0));

	gl_Position = uGlobal.Projection * uGlobal.View * vec4(pFragPos, 1.0);

	pNormal = mat3(transpose(inverse(aInstanceModel))) * aNormal;
	pUV = aUV;

	pInstanceColor = aInstanceColor;
}
