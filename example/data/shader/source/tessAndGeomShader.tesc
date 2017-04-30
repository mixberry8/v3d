#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//-------------------------------------------------------------------------------------------------
// input
//-------------------------------------------------------------------------------------------------

layout(location = 0) in vec3 inNormal[];
layout(location = 1) in vec2 inUV[];

layout(push_constant) uniform Constant
{
	float level;
}constant;

//-------------------------------------------------------------------------------------------------
// output
//-------------------------------------------------------------------------------------------------

layout(vertices = 3) out;

layout(location = 0) out vec3 outNormal[3];
layout(location = 1) out vec2 outUV[3];

//-------------------------------------------------------------------------------------------------
// entry
//-------------------------------------------------------------------------------------------------

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	outNormal[gl_InvocationID] = inNormal[gl_InvocationID];
	outUV[gl_InvocationID] = inUV[gl_InvocationID];

	gl_TessLevelOuter[gl_InvocationID] = float(constant.level);
	gl_TessLevelInner[0] = float(constant.level);
}
