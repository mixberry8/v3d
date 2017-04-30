#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//-------------------------------------------------------------------------------------------------
// input
//-------------------------------------------------------------------------------------------------

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout(location = 0) in vec3 inNormal[];
layout(location = 1) in vec2 inUV[];

layout(push_constant) uniform Constant
{
	layout(offset = 16) float height;
	layout(offset = 32) mat4 worldMatrix;
	layout(offset = 96) mat4 worldViewProjectionMatrix;
}constant;

layout(set = 0, binding = 0) uniform sampler2D heightSampler;

//-------------------------------------------------------------------------------------------------
// output
//-------------------------------------------------------------------------------------------------

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec3 outWorldNormal;

//-------------------------------------------------------------------------------------------------
// entry
//-------------------------------------------------------------------------------------------------

void main()
{
	vec4 p0 = gl_in[0].gl_Position;
	vec4 p1 = gl_in[1].gl_Position;
	vec4 p2 = gl_in[2].gl_Position;

	p0.xyz += inNormal[0] * texture(heightSampler, inUV[0]).x * constant.height;
	p1.xyz += inNormal[1] * texture(heightSampler, inUV[1]).x * constant.height;
	p2.xyz += inNormal[2] * texture(heightSampler, inUV[2]).x * constant.height;

	vec3 vec0 = p1.xyz - p0.xyz;
	vec3 vec1 = p2.xyz - p0.xyz;
	vec3 localNormal = normalize(cross(vec0, vec1));

	outWorldNormal = mat3(constant.worldMatrix) * localNormal;
	
	gl_Position = constant.worldViewProjectionMatrix * p0;
	outUV = inUV[0];
	EmitVertex();

	gl_Position = constant.worldViewProjectionMatrix * p1;
	outUV = inUV[1];
	EmitVertex();

	gl_Position = constant.worldViewProjectionMatrix * p2;
	outUV = inUV[2];
	EmitVertex();

	EndPrimitive();
}