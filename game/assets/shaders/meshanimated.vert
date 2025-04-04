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

struct dualquat
{
	vec4 real;
	vec4 dual;
};

layout(std140, binding = 3) uniform AnimationUniform
{
	dualquat uBoneMatrices[255];
};

out VP_Shared {
	vec3 pFragPos;
    vec3 pNormal;
    vec2 pUV;
	vec4 pInstanceColor;
};

mat4 CalcAnimationMat4()
{
	ivec4 boneIds = ivec4(
		int(aBoneIds & 0xFF), 
		int((aBoneIds >> 8)) & 0xFF, 
		int((aBoneIds >> 16)) & 0xFF,
		int((aBoneIds >> 24)) & 0xFF);

	vec4 weightedRot = vec4(0.0);
	vec4 weightedTrans = vec4(0.0);
	for (int i = 0; i < 4; i++) {
		if (boneIds[i] == 255) {
			continue;
		}

		dualquat qt = uBoneMatrices[boneIds[i]];

		weightedRot += qt.real * aWeights[i];
		weightedTrans += qt.dual * aWeights[i];
	}

	float rotMag = sqrt(
		weightedRot.x * weightedRot.x + 
		weightedRot.y * weightedRot.y + 
		weightedRot.z * weightedRot.z + 
		weightedRot.w * weightedRot.w);

	weightedRot /= rotMag;
	weightedTrans /= rotMag;

	float xR = weightedRot.x;
	float yR = weightedRot.y;
	float zR = weightedRot.z;
	float wR = weightedRot.w;
	float xT = weightedTrans.x;
	float yT = weightedTrans.y;
	float zT = weightedTrans.z;
	float wT = weightedTrans.w;

	float t0 = 2.0 * (-wT * xR + xT * wR - yT * zR + zT * yR);
	float t1 = 2.0 * (-wT * yR + xT * zR + yT * wR - zT * xR);
	float t2 = 2.0 * (-wT * zR - xT * yR + yT * xR + zT * wR);

	return mat4(
			1.0 - (2.0 * yR * yR) - (2.0 * zR * zR),
			(2.0 * xR * yR) + (2.0 * wR * zR),
			(2.0 * xR * zR) - (2.0 * wR * yR),
			0,
			(2.0 * xR * yR) - (2.0 * wR * zR),
			1.0 - (2.0 * xR * xR) - (2.0 * zR * zR),
			(2.0 * yR * zR) + (2.0 * wR * xR),
			0,
			(2.0 * xR * zR) + (2.0 * wR * yR),
			(2.0 * yR * zR) - (2.0 * wR * xR),
			1.0 - (2.0 * xR * xR) - (2.0 * yR * yR),
			0,
			t0,
			t1,
			t2,
			1
		);
}

void main()
{
	mat4 BoneTransform = CalcAnimationMat4();

	mat4 WorldMatrix = aInstanceModel * BoneTransform;
	pFragPos = vec3(WorldMatrix * vec4(aPos, 1.0));

	gl_Position = uGlobal.Projection * uGlobal.View * vec4(pFragPos, 1.0);

	pNormal = transpose(mat3(inverse(WorldMatrix))) * aNormal;
	pUV = aUV;

	pInstanceColor = aInstanceColor;
}
