#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
	float time;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 tangentToWorld[3];

out gl_PerVertex {
    vec4 gl_Position;
};

mat3 CreateTangentToWorld(vec3 normal, vec3 tangent){
	vec3 binormal = cross(normal, tangent);
	return mat3(tangent, binormal, normal);
}

void main() {
    gl_Position  = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
	fragTexCoord = inTexCoord;
	
	vec3 normalWorld  = (ubo.model * vec4(inNormal,  0.0)).xyz;
	vec3 tangentWorld = (ubo.model * vec4(inTangent, 0.0)).xyz;
	mat3 tbn = CreateTangentToWorld(normalWorld, tangentWorld);
	tangentToWorld[0].xyz = tbn[0];
	tangentToWorld[1].xyz = tbn[1];
	tangentToWorld[2].xyz = tbn[2];
}