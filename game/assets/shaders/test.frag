#version 460

#define MAX_POINTLIGHTS 512
#define MAX_SPOTLIGHTS 512

const float LIGHT_CONSTANT = 1.0;
const float LIGHT_LINEAR = 0.09;
const float LIGHT_QUADRATIC = 0.032;

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
    vec3 pNormal;
    vec2 pUV;
	vec4 pInstanceColor;
};

void main()
{
	//vec3 lighting = CalcPointlights(pFragPos, pNormal);
	//lighting += CalcSpotlights(pFragPos, pNormal);

	FRAG_COLOR = pInstanceColor * texture(uTexture, pUV);// * vec4(lighting, 1.0);
}
