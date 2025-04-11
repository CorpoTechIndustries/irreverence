#version 460

const float EPSILON = 0.00001;

layout(location = 0) out vec4 fMainColor;

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

layout(binding = 0) uniform sampler2D uTexAccum;
layout(binding = 1) uniform sampler2D uTexReveal;

bool isApproxEqual(float a, float b)
{
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

float max3(vec3 v)
{
    return max(max(v.x, v.y), v.z);
}

in VP_Shared {
    vec2 pUV;
	vec4 pInstanceColor;
};

void main()
{
    ivec2 coords = ivec2(pUV);

    float revealage = texture(uTexReveal, pUV).r;

    // save the blending and color texture fetch cost if there is not a transparent fragment
    if (isApproxEqual(revealage, 1.0)) {
        discard;
    }

    vec4 accumulation = texture(uTexAccum, pUV);

    if (isinf(max3(abs(accumulation.rgb)))) {
        accumulation.rgb = vec3(accumulation.a);
    }

    vec3 average_color = accumulation.rgb / max(accumulation.a, EPSILON);

    fMainColor = vec4(average_color, 1.0 - revealage);
}
