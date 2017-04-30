#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//-------------------------------------------------------------------------------------------------
// input
//-------------------------------------------------------------------------------------------------

layout(set = 0, binding = 0) uniform Mesh
{
	mat4 worldMatrix;
	mat4 viewProjectionMatrix;
}mesh;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

//-------------------------------------------------------------------------------------------------
// output
//-------------------------------------------------------------------------------------------------

out gl_PerVertex 
{
   vec4 gl_Position;
};

layout(location = 0) out vec3 outWorldNormal;
layout(location = 1) out vec2 outUV;

//-------------------------------------------------------------------------------------------------
// entry
//-------------------------------------------------------------------------------------------------

void main()
{
    gl_Position = mesh.viewProjectionMatrix * vec4(inPos, 1.0);

	outWorldNormal = mat3(mesh.worldMatrix) * inNormal;
	outUV = inUV;
}
