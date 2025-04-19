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
layout(location = 1) uniform vec2 uTexelSize; 

in VP_Shared {
    vec2 pUV;
	vec4 pInstanceColor;
};

vec3 PowVec3(vec3 v, float p)
{
    return vec3(pow(v.x, p), pow(v.y, p), pow(v.z, p));
}

float KarisAverage(vec3 col)
{
    // PowVec3(col, invGamma)
    return 1.0 / (1.0 + (dot(PowVec3(col, 0.454545), vec3(0.2126, 0.7152, 0.0722)) * 0.25));
}

void main()
{
    float x = uTexelSize.x;
    float y = uTexelSize.y;

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

    if (uTexLevel == 0) {
        // version one

        vec3 groups[5];
        groups[0] = (a + b + d + e) / 4.0;
        groups[1] = (b + c + e + f) / 4.0;
        groups[2] = (d + e + g + h) / 4.0;
        groups[3] = (e + f + h + i) / 4.0;
        groups[4] = (j + k + l + m) / 4.0;
        float kw0 = KarisAverage(groups[0]);
        float kw1 = KarisAverage(groups[1]);
        float kw2 = KarisAverage(groups[2]);
        float kw3 = KarisAverage(groups[3]);
        float kw4 = KarisAverage(groups[4]);
        vec3 col = (kw0 * groups[0] + kw1 * groups[1] + kw2 * groups[2] + kw3 *  groups[3] + kw4 * groups[4]) / 
        (kw0 + kw1 + kw2 + kw3 + kw4);

        float brightness = dot(col.rgb, vec3(0.2126, 0.7152, 0.0722));
        float threshold = 1.0;
        float knee = 0.5;
        float t = clamp((brightness - (threshold - knee)) / (2.0 * knee), 0.0, 1.0);
        fColor = col * (t * t * (3.0 - 2.0 * t));


        // TODO: Have graphical settings, below for low and above for high.
        // version two 
        // vec3 col = e * 0.125;
        // col += (a + c + g + i) * 0.03125;
        // col += (b + d + f + h) * 0.0625;
        // col += (j + k + l + m) * 0.125;

        // float brightness = dot(col.rgb, vec3(0.2126, 0.7152, 0.0722));
        // float threshold = 1.0;
        // float knee = 0.5;
        // float t = clamp((brightness - (threshold - knee)) / (2.0 * knee), 0.0, 1.0);

        // fColor = col * (t * t * (3.0 - 2.0 * t));
    } else {
        vec3 col = e * 0.125;
        col += (a + c + g + i) * 0.03125;
        col += (b + d + f + h) * 0.0625;
        col += (j + k + l + m) * 0.125;
        fColor = col;
    }

}
