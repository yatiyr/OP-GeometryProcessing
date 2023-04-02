#type vertex
#version 460 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

layout (location = 0) in vec3  a_Position;
layout (location = 1) in vec3  a_Normal;
layout (location = 2) in vec2  a_TexCoords;
layout (location = 3) in vec3  a_Tangent;
layout (location = 4) in vec3  a_Bitangent;
layout (location = 5) in ivec4 a_BoneIDs;
layout (location = 6) in vec4  a_BoneWeights;


struct VS_OUT
{
	vec3 FragPos;
	vec4 FragPosViewSpace;
	vec3 Normal;
	vec2 TexCoords;
	mat3 TBN;
};

layout (location = 0) out VS_OUT vs_out;


// -------------- UNIFORM BUFFERS ------------- //
#include UniformBuffers.glsl
// -------------------------------------------- //

void main()
{

	mat3 modelMatrixN = transpose(inverse(mat3(u_Model)));

	vec3 T = normalize(modelMatrixN * a_Tangent);
	vec3 B = normalize(modelMatrixN * a_Bitangent);
	vec3 N = normalize(modelMatrixN * a_Normal);
	vs_out.TBN = mat3(T, B, N);
	

	vs_out.FragPos = vec3(u_Model * vec4(a_Position, 1.0));
	vs_out.FragPosViewSpace = u_View * vec4(vs_out.FragPos, 1.0);
	vs_out.Normal  = N;
	vs_out.TexCoords = a_TexCoords;

	gl_Position =  u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 460 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //


layout(location = 0) out vec4 FragColor;

struct VS_OUT
{
	vec3 FragPos;
	vec4 FragPosViewSpace;
	vec3 Normal;
	vec2 TexCoords;
	mat3 TBN;
};

layout (location = 0) in VS_OUT fs_in;

layout (location = 0) uniform float u_Roughness;
layout (location = 1) uniform float u_Metalness;
layout (location = 2) uniform vec3 u_Albedo;

layout (binding = 0) uniform samplerCube u_IrradianceMap;
layout (binding = 1) uniform samplerCube u_PrefilterMap;
layout (binding = 2) uniform sampler2D u_BrdfLUT;
layout (binding = 3) uniform sampler2D u_BayerDithering;

// ------------- GLOBAL VARIABLES ------------- //
#include GlobalVariables.glsl
// -------------- UNIFORM BUFFERS ------------- //
#include UniformBuffers.glsl
// ----------------- UTILS -------------------- //
#include Utils.glsl
// -------------- PBR FUNCS ------------------- //
#include PbrFunctions.glsl
// -------------------------------------------- //

void main()
{

	vec2 texCoords = fs_in.TexCoords;
	vec3 fragPos = fs_in.FragPos;
	
	vec3 viewDir = u_ViewPos - fs_in.FragPos;


	vec3 color      = u_Albedo;
	float roughness = u_Roughness;
	float metalness = u_Metalness;
	// float ao = texture(u_AoMap, texCoords * u_TilingFactor).r;

	vec3 normal = vec3(0.0, 0.0, 1.0);
	normal = normalize(normalize(fs_in.TBN * normal));

	vec3 reflectionVec = normalize(reflect(-viewDir, normal)); 

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, color, metalness);


	vec3 Lo = vec3(0.0);
	float shadow = 0.0;

	// One directional light hardcoded to the shader for now
	

		
	vec3 radiance = vec3(5.0,3.7,1.0);
	vec3 lightDir = normalize(vec3(0.45, 1.0, 0.55));
	vec3 halfwayDir = normalize(viewDir + lightDir);
		

	float NDF = DistributionGGX(normal, halfwayDir, roughness);
	float G   = GeometrySmith(normal, viewDir, lightDir, roughness);
	vec3  F   = FresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);

	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.01;
	vec3 specular = numerator / denominator;

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metalness;

	float backFacingFactor = dot(normalize(fs_in.Normal), lightDir) > 0 ? 1.0 : 0.0;

	float NdotL = max(dot(normal, lightDir), 0.0);


	Lo += (kD * color / PI + specular) * radiance * NdotL * backFacingFactor;
	


	

	kS = FresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness); 
	kD = 1.0 - kS;
	kD *= 1.0 - metalness;

	vec3 irradiance = texture(u_IrradianceMap, normal).rgb;
	vec3 diffuse    = irradiance * color;

	// sample both pre-filter map and the BRDF lut and combine them together as per the split-sum approximation to get the IBL specular part
	const float MAX_REFLECTION_LOD = 4.0;

	vec3 prefilteredColor = textureLod(u_PrefilterMap, reflectionVec, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 brdf = texture(u_BrdfLUT, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;		
	specular = prefilteredColor * (kS * brdf.x + brdf.y);
	vec3 ambient    = (kD * diffuse + specular); 
	//vec3 ambient = vec3(0.03) * color * ao;

	vec3 lighting    =  (ambient + Lo);

	FragColor = vec4(lighting, 1.0);

	FragColor += vec4(vec3(texture(u_BayerDithering, gl_FragCoord.xy / 8.0).r / 64.0 - (1.0 / 128.0)), 0.0);

}