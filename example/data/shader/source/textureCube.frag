#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//-------------------------------------------------------------------------------------------------
// input
//-------------------------------------------------------------------------------------------------

layout(location = 0) in vec4  inColor;
layout(location = 1) in float inLodBias;
layout(location = 2) in vec3  inEyePos;
layout(location = 3) in vec3  inWorldPos;
layout(location = 4) in vec3  inWorldNormal;

layout(set = 0, binding = 0) uniform samplerCube imageSampler;

//-------------------------------------------------------------------------------------------------
// output
//-------------------------------------------------------------------------------------------------

layout(location = 0) out vec4 outColor;

//-------------------------------------------------------------------------------------------------
// entry
//-------------------------------------------------------------------------------------------------

void main()
{
	vec3 refVec = reflect(inEyePos - inWorldPos, inWorldNormal);
	
	outColor = inColor * texture(imageSampler, refVec, inLodBias);
}
