#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//-------------------------------------------------------------------------------------------------
// input
//-------------------------------------------------------------------------------------------------

layout(push_constant) uniform Scene
{
	vec4 lightDirection;
}scene;

layout(set = 0, binding = 1) uniform sampler2D imageSampler;

layout(location = 0) in vec3 inWorldNormal;
layout(location = 1) in vec2 inUV;

//-------------------------------------------------------------------------------------------------
// output
//-------------------------------------------------------------------------------------------------

layout(location = 0) out vec4 outColor;

//-------------------------------------------------------------------------------------------------
// entry
//-------------------------------------------------------------------------------------------------

void main()
{
	float NdotL = clamp(dot(inWorldNormal, scene.lightDirection.xyz), 0.0, 1.0);

	outColor = texture(imageSampler, inUV) * vec4(NdotL, NdotL, NdotL, 1.0);
}
