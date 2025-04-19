#version 460

#define MAX_POINTLIGHTS 512
#define MAX_SPOTLIGHTS 512

layout(location = 0) out vec3 fMainColor;

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

struct Pointlight
{
	vec3 position;
	float brightness;
	vec3 color;
	float radius;
};

struct Spotlight
{
	vec3 position;
	float cutoff;
	vec3 direction;
	float outercutoff;
	vec3 color;
	float brightness;
	vec3 padding;
	float radius;
};

layout(std140, binding = 1) uniform PointlightUniform
{
	Pointlight uPointlights[MAX_POINTLIGHTS];
	uint uPointlightCount;
};

layout(std140, binding = 2) uniform SpotlightUniform
{
	Spotlight uSpotlights[MAX_SPOTLIGHTS];
	uint uSpotlightCount;
};

vec3 CalcPointlights(vec3 fragPos, vec3 normal)
{	
	vec3 result = vec3(0.0);

	for(uint i = 0; i < uPointlightCount; i++) {
		Pointlight light = uPointlights[i];

		if (light.brightness <= 0.01) {
			continue;
		}

		vec3 difOriginFrag = light.position - fragPos;
		float dist = length(difOriginFrag);

		if (dist >= light.radius) {
			continue;
		}

		vec3 lightDir = normalize(difOriginFrag);

		float hL = dot(normal, lightDir) * 0.5 + 0.5;
		hL *= hL;

		float t = dist / light.radius;
		float falloff = 1.0 - t * t * (3.0 - 2.0 * t);

		result += light.color.rgb * light.brightness * falloff * hL;
	}

	return result;
}

vec3 CalcSpotlights(vec3 fragPos, vec3 normal)
{	
	vec3 result = vec3(0.0);

	for(int i = 0; i < uSpotlightCount; i++) {
		Spotlight light = uSpotlights[i];

		if (light.brightness <= 0.01) {
			continue;
		}

		vec3 difOriginFrag = light.position - fragPos;
		float dist = length(difOriginFrag);

		if (dist >= light.radius) {
			continue;
		}

		vec3 lightDir = normalize(difOriginFrag);

		float intensity = clamp((dot(-lightDir, normalize(light.direction)) - light.outercutoff) / (light.cutoff - light.outercutoff), 0.0, 1.0);

		float hL = dot(normal, lightDir) * 0.5 + 0.5;
		hL *= hL;

		float t = dist / light.radius;
		float falloff = 1.0 - t * t * (3.0 - 2.0 * t);

		result += light.color.rgb * light.brightness * falloff * hL * intensity;
	}

	return result;
}

layout(binding = 0) uniform sampler2D uTexture;

in VP_Shared {
	vec3 pFragPos;
	float pDepth;
    vec3 pNormal;
    vec2 pUV;
	vec4 pInstanceColor;
};

void main()
{
	vec4 result = pInstanceColor * texture(uTexture, pUV);

	if (result.a < 0.01) {
		discard;
	}

	vec3 lighting = CalcPointlights(pFragPos, pNormal);
	lighting += CalcSpotlights(pFragPos, pNormal);

	if (result.r <= 1.0 && result.g <= 1.0 && result.b <= 1.0) {
		result.rgb *= lighting;	// TODO: Hacky for now, use emissiveness map later
	}

	fMainColor = result.rgb;
}
