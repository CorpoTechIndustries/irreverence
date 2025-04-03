#version 460

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in uint aBoneIds;
layout(location = 4) in vec4 aWeights;

layout(location = 5) in vec4 aInstanceColor;
layout(location = 6) in mat4 aInstanceModel;

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
	vec4 totalPos = vec4(0.0);

	mat4 BoneTransform = uBoneMatrices[int(aBoneIds) 		 & 0xFF] * aWeights[0];
    BoneTransform 	  += uBoneMatrices[int((aBoneIds >> 8))  & 0xFF] * aWeights[1];
    BoneTransform 	  += uBoneMatrices[int((aBoneIds >> 16)) & 0xFF] * aWeights[2];
    BoneTransform 	  += uBoneMatrices[int((aBoneIds >> 24)) & 0xFF] * aWeights[3];

	mat4 WorldMatrix = aInstanceModel * BoneTransform;
	pFragPos = vec3(WorldMatrix * vec4(aPos, 1.0));

	gl_Position = uGlobal.Projection * uGlobal.View * vec4(pFragPos, 1.0);

	pNormal = transpose(mat3(inverse(WorldMatrix))) * aNormal;
	pUV = aUV;

	pInstanceColor = aInstanceColor;
}
