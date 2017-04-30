#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//-------------------------------------------------------------------------------------------------
// input
//-------------------------------------------------------------------------------------------------

layout(location = 0) in vec2 inUV;

layout(push_constant) uniform Scene
{
	layout(offset = 64) float depth;
}scene;

layout(set = 0, binding = 0) uniform sampler3D imageSampler;

//-------------------------------------------------------------------------------------------------
// output
//-------------------------------------------------------------------------------------------------

layout(location = 0) out vec4 outColor;

//-------------------------------------------------------------------------------------------------
// entry
//-------------------------------------------------------------------------------------------------

void main()
{
	outColor = texture(imageSampler, vec3(inUV, scene.depth));
}
