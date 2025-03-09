#version 460

out vec4 FRAG_COLOR;

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