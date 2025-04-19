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
	vec4 pInstanceColor;
};

void main()
{
	FRAG_COLOR = pInstanceColor * vec4(texture(uTex, pUV).r);
}
