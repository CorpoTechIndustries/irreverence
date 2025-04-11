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

layout(binding = 0) uniform sampler2D uTex;

layout(location = 0) uniform uint uTexLevel; 
layout(location = 1) uniform vec2 uTexSize; 

in VP_Shared {
    vec2 pUV;
	vec4 pInstanceColor;
};

void main()
{
    vec2 texelSize = 1.0 / uTexSize;
    float x = texelSize.x;
    float y = texelSize.y;

    vec3 a = textureLod(uTex, vec2(pUV.x - 2 * x, pUV.y + 2 * y), uTexLevel).rgb;
    vec3 b = textureLod(uTex, vec2(pUV.x, pUV.y + 2 * y), uTexLevel).rgb;
    vec3 c = textureLod(uTex, vec2(pUV.x + 2 * x, pUV.y + 2 * y), uTexLevel).rgb;

    vec3 d = textureLod(uTex, vec2(pUV.x - 2 * x, pUV.y), uTexLevel).rgb;
    vec3 e = textureLod(uTex, pUV, uTexLevel).rgb;
    vec3 f = textureLod(uTex, vec2(pUV.x + 2 * x, pUV.y), uTexLevel).rgb;

    vec3 g = textureLod(uTex, vec2(pUV.x - 2 * x, pUV.y - 2 * y), uTexLevel).rgb;
    vec3 h = textureLod(uTex, vec2(pUV.x, pUV.y - 2 * y), uTexLevel).rgb;
    vec3 i = textureLod(uTex, vec2(pUV.x + 2 * x, pUV.y - 2 * y), uTexLevel).rgb;

    vec3 j = textureLod(uTex, vec2(pUV.x - x, pUV.y + y), uTexLevel).rgb;
    vec3 k = textureLod(uTex, vec2(pUV.x + x, pUV.y + y), uTexLevel).rgb;
    vec3 l = textureLod(uTex, vec2(pUV.x - x, pUV.y - y), uTexLevel).rgb;
    vec3 m = textureLod(uTex, vec2(pUV.x + x, pUV.y - y), uTexLevel).rgb;

    fColor = e * 0.125;
    fColor += (a + c + g + i) * 0.03125;
    fColor += (b + d + f + h) * 0.0625;
    fColor += (j + k + l + m) * 0.125;
    fColor = max(fColor, 0.0001);
}
