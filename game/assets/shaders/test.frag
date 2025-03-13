#version 460

out vec4 FRAG_COLOR;

layout(std140, binding = 0) uniform GlobalUniform
{
	uint Width;
	uint Height;
	float CurTime;
	float FrameTime;
	mat4 View;
	mat4 Projection;
} uGlobal;

in VP_Shared {
	vec3 pFragPos;
    vec3 pNormal;
    vec2 pUV;
	vec4 pInstanceColor;
};

layout(binding = 0) uniform sampler2D uTexture;

void main()
{
	FRAG_COLOR = pInstanceColor * texture(uTexture, pUV);
}