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

layout(binding = 0) uniform sampler2D uTexScreen;
layout(binding = 1) uniform sampler2D uTexBloom;

in VP_Shared {
    vec2 pUV;
	vec4 pInstanceColor;
};

vec3 GammaCorrect(vec3 color, float gamma) {
    return pow(color, vec3(1.0 / gamma));
}

void main()
{
	vec3 mainCol = texture(uTexScreen, pUV).rgb;
	vec3 bloomCol = texture(uTexBloom, pUV).rgb;

	vec3 result = mainCol + bloomCol / 20.0;
	result = result / (result + vec3(1.0));
	//result = GammaCorrect(result, 2.2);

	FRAG_COLOR = pInstanceColor * vec4(result, 1.0);
}
