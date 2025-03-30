#version 460

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in ivec4 aBoneIds;
layout(location = 4) in vec4 aBoneWeights;

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

layout(std140, binding = 1) uniform AnimationUniform
{
	mat4 uBoneMatrices[100];
	uint uBoneCount;
};

out VP_Shared {
	vec3 pFragPos;
    vec3 pNormal;
    vec2 pUV;
	vec4 pInstanceColor;
};

void main()
{
	ivec4 boneIds = aBoneIds; //ivec4(aBoneIds.x & 0xFF, aBoneIds.x << 16, aBoneIds.y & 0xFF, aBoneIds.y << 16);

	vec4 totalPosition = vec4(0.0);
	for (uint i = 0; i < 4; i++) {
		if (boneIds[i] == 65535) {
			continue;
		}

		if (boneIds[i] >= 100) {
			totalPosition += vec4(aPos, 1.0);
			break;
		}

		vec4 localPosition = uBoneMatrices[boneIds[i]] * vec4(aPos, 1.0);
		totalPosition += localPosition * aBoneWeights[i];
	}

	pFragPos = vec3(aInstanceModel * totalPosition);

	gl_Position = uGlobal.Projection * uGlobal.View * aInstanceModel * totalPosition;

	pNormal = mat3(transpose(inverse(aInstanceModel))) * aNormal;
	pUV = aUV;

	pInstanceColor = aInstanceColor;
}
