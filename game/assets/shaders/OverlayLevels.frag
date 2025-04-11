#version 460

layout(location = 0) out vec3 fColor;

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

in VP_Shared {
    vec2 pUV;
	vec4 pInstanceColor;
};

void main()
{
	float whitePoint = 1.0;
	float blackPoint = 0.99;

	fColor = texture(uTexScreen, pUV).rgb * whitePoint - (blackPoint * whitePoint), 1.0;
}
