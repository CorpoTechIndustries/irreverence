#version 460

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;

layout(location = 2) in vec4 aInstanceColor;
layout(location = 3) in vec2 aInstancePos;
layout(location = 4) in vec2 aInstanceSize;
layout(location = 5) in vec4 aInstanceUVs;

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
    vec2 pUV;
	vec4 pInstanceColor;
};

void main()
{
	gl_Position = uCommon.Projection * vec4(aPos * aInstanceSize + aInstancePos, 0.0, 1.0);

	pUV = vec2(mix(aInstanceUVs.x, aInstanceUVs.z, aUV.x), mix(aInstanceUVs.y, aInstanceUVs.w, aUV.y));
	pInstanceColor = aInstanceColor;
}
