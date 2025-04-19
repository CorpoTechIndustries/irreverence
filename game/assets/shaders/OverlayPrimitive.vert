#version 460

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec4 aColor;

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
    vec4 pColor;
};

void main()
{
	gl_Position = uCommon.Projection * vec4(aPos, 0.0, 1.0);

	pUV = aUV;
	pColor = aColor;
}
