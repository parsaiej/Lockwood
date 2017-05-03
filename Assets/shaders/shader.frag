#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D sampler0;
layout(binding = 2) uniform sampler2D sampler1;
layout(binding = 3) uniform sampler2D sampler2;
layout(binding = 4) uniform sampler2D sampler3;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 tangentToWorld[3];
layout(location = 4) in vec3 eyeDir;

layout(location = 0) out vec4 outColor;

vec3 NormalInTangentSpace(vec2 uv){
	vec4 s = texture(sampler0, uv);
	
	vec3 n = s.xyz * 2 - 1;
	n.xy *= -1;
	
	return n;
}

vec3 PerPixelWorldNormal(vec2 uv, vec3 tangentToWorld[3]){
	vec3 tangent  = tangentToWorld[0].xyz;
	vec3 binormal = tangentToWorld[1].xyz;
	vec3 normal   = tangentToWorld[2].xyz;
	
	normal  = normalize(normal);
	tangent = normalize(tangent - normal * dot(tangent, normal));
	vec3 newB = cross(normal, tangent);
	binormal = newB * sign(dot(newB, binormal));
	
	vec3 normalTangent = NormalInTangentSpace(uv);
	vec3 normalWorld = normalize(tangent * normalTangent.x + binormal * normalTangent.y + normal * normalTangent.z);
	
	return normalWorld;
}

void main() {
	vec3 normal = PerPixelWorldNormal(fragTexCoord, tangentToWorld);
	float nl = max(dot(normal, vec3(0, 1, 0)), 0.0);

	vec3   a = texture(sampler1, fragTexCoord).rgb;
	float rg = texture(sampler2, fragTexCoord).r;
	float ao = texture(sampler3, fragTexCoord).r;
	
	
	//calculate specular
	vec3 h = normalize(vec3(0, 1, 0) + -eyeDir);
	float nh = pow(max(dot(h, normal), 0.0), rg * 1000.0);
	
    outColor = vec4((a*(nl+(ao*0.25))) + (0.5*vec3(nh)), 1.0);
}