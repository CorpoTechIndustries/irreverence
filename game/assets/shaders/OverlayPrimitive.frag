#version 460

out vec4 FRAG_COLOR;

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

layout(binding = 0) uniform sampler2D uTex;

in VP_Shared {
    vec2 pUV;
    vec4 pColor;
};

void main()
{
	FRAG_COLOR = texture(uTex, pUV) * pColor;
}
