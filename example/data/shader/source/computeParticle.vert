#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//-------------------------------------------------------------------------------------------------
// input
//-------------------------------------------------------------------------------------------------

layout(location = 0) in vec3 inPos;

layout(push_constant) uniform Scene
{
	mat4 viewProjectionMatrix;
	float size;
}scene;

//-------------------------------------------------------------------------------------------------
// output
//-------------------------------------------------------------------------------------------------

out gl_PerVertex 
{
   vec4 gl_Position;
   float gl_PointSize;
};

//-------------------------------------------------------------------------------------------------
// entry
//-------------------------------------------------------------------------------------------------

void main()
{
    gl_Position = scene.viewProjectionMatrix * vec4(inPos, 1.0);
	gl_PointSize = scene.size;
}
