#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//-------------------------------------------------------------------------------------------------
// input
//-------------------------------------------------------------------------------------------------

layout(triangles, equal_spacing, ccw) in;

layout(location = 0) in vec3 inNormal[];
layout(location = 1) in vec2 inUV[];

//-------------------------------------------------------------------------------------------------
// output
//-------------------------------------------------------------------------------------------------

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outUV;

//-------------------------------------------------------------------------------------------------
// entry
//-------------------------------------------------------------------------------------------------

void main()
{
	vec3 p0 = gl_in[0].gl_Position.xyz;
	vec3 p1 = gl_in[1].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz;

	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	float w = gl_TessCoord.z;
	
	gl_Position = vec4(p0 * u + p1 * v + p2 * w, 1.0);

	outNormal = vec3(inNormal[0] * u + inNormal[1] * v + inNormal[2] * w);
	outUV = vec2(inUV[0] * u + inUV[1] * v + inUV[2] * w);
}
