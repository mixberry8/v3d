#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//-------------------------------------------------------------------------------------------------
// input
//-------------------------------------------------------------------------------------------------

layout(location = 0) in vec4 inPos;
layout(location = 1) in vec2 inUV;

layout(push_constant) uniform Scene
{
	mat4 matrix;
}scene;

//-------------------------------------------------------------------------------------------------
// output
//-------------------------------------------------------------------------------------------------

layout(location = 0) out vec2 outUV;

out gl_PerVertex 
{
   vec4 gl_Position;
};

//-------------------------------------------------------------------------------------------------
// entry
//-------------------------------------------------------------------------------------------------

void main()
{
    gl_Position = scene.matrix * inPos;
	outUV = inUV;
}
