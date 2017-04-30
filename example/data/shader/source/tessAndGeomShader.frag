#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//-------------------------------------------------------------------------------------------------
// input
//-------------------------------------------------------------------------------------------------

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inWorldNormal;

layout(push_constant) uniform Constant
{
	layout(offset = 160) vec4 lightDirection;
}constant;

//-------------------------------------------------------------------------------------------------
// output
//-------------------------------------------------------------------------------------------------

layout(location = 0) out vec4 outColor;

//-------------------------------------------------------------------------------------------------
// entry
//-------------------------------------------------------------------------------------------------

void main()
{
	float NdotL = clamp(dot(inWorldNormal, constant.lightDirection.xyz), 0.0, 1.0);

	outColor = vec4(NdotL, NdotL, NdotL, 1.0);
}
