#version 460

out vec4 FRAG_COLOR;

layout(std140, binding = 0) uniform GlobalUniform
{
    uint Width;
	uint Height;
	float CurTime;
	float FrameTime;
	mat4 ViewMat;
	mat4 ProjectionMat;
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
	FRAG_COLOR = vec4(pNormal, 1.0) * pInstanceColor * texture(uTexture, pUV);
}