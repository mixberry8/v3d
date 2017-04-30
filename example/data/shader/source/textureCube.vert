#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//-------------------------------------------------------------------------------------------------
// input
//-------------------------------------------------------------------------------------------------

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

layout(push_constant) uniform Scene
{
	float lodBias;
	vec4 eyePos;
	vec4 lightDirection;
	mat4 worldMatrix;
	mat4 viewProjectionMatrix;
}scene;

//-------------------------------------------------------------------------------------------------
// output
//-------------------------------------------------------------------------------------------------

out gl_PerVertex 
{
   vec4 gl_Position;
};

layout(location = 0) out vec4  outColor;
layout(location = 1) out float outLodBias;
layout(location = 2) out vec3  outEyePos;
layout(location = 3) out vec3  outWorldPos;
layout(location = 4) out vec3  outWorldNormal;

//-------------------------------------------------------------------------------------------------
// entry
//-------------------------------------------------------------------------------------------------

void main()
{
    gl_Position = scene.viewProjectionMatrix * vec4(inPos, 1.0);

	vec3 worldNormal = mat3(scene.worldMatrix) * inNormal;
	float NdotL = clamp(dot(worldNormal, scene.lightDirection.xyz), 0.0, 1.0);
	
	outColor = vec4(NdotL, NdotL, NdotL, 1.0);
	outLodBias = scene.lodBias;
	outEyePos = scene.eyePos.xyz;
	outWorldPos = vec3(scene.worldMatrix * vec4(inPos, 1.0));
	outWorldNormal = worldNormal;
}
