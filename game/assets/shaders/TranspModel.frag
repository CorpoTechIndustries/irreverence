#version 460

#define MAX_POINTLIGHTS 512
#define MAX_SPOTLIGHTS 512

const float LIGHT_CONSTANT = 1.0;
const float LIGHT_LINEAR = 0.09;
const float LIGHT_QUADRATIC = 0.032;

layout(location = 1) out vec4 fAccumColor;
layout(location = 2) out float fReveal;

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
		vec3 lightDir = normalize(difOriginFrag);

		// Diffuse Lighting
		float diff = light.brightness * max(dot(normal, lightDir), 0.0);

		// Attenuation
		float dist = length(difOriginFrag);
		float attenuation = 1.0f / (LIGHT_CONSTANT + LIGHT_LINEAR * dist + LIGHT_QUADRATIC * (dist * dist));

		result += light.color.rgb * diff * attenuation;
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
		vec3 lightDir = normalize(difOriginFrag);

		// Diffuse Lighting
		float diff = light.brightness * max(dot(normal, lightDir), 0.0);

		// Attenuation
   	 	float intensity = clamp((dot(lightDir, normalize(-light.direction)) - light.outercutoff) / (light.cutoff - light.outercutoff), 0.0, 1.0);
		float dist = length(difOriginFrag);
		float attenuation = ( 1.0 / (LIGHT_CONSTANT + LIGHT_LINEAR * dist + LIGHT_QUADRATIC * (dist * dist)) ) * intensity;

		result += light.color.rgb * diff * attenuation;
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

	// TODO: Combine with Opaque Shader and just do a simple if check

	vec3 lighting = CalcPointlights(pFragPos, pNormal);
	lighting += CalcSpotlights(pFragPos, pNormal);

    float weight = clamp(pow(min(1.0, result.a * 10.0) + 0.01, 3.0) * 1e8 * 
                         pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

	fAccumColor = vec4(result.rgb * result.a, result.a) * weight;
	fReveal = result.a;
}
