#version 460

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

layout(location = 3) in vec4 aInstanceColor;
layout(location = 4) in mat4 aInstanceModelmat;

out VP_Shared {
	vec3 pFragPos;
    vec3 pNormal;
    vec2 pUV;
	vec4 pInstanceColor;
};

void main()
{
	gl_Position = vec4(aPos.xyz, 1.0);

	pFragPos = aPos;
	pNormal = aNormal;
	pUV = aUV;

	pInstanceColor = aInstanceColor;
}