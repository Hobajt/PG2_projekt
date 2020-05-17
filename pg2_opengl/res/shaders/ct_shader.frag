#version 460 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : require	//uint64_t

//====== Constants ======
float PI = 3.14159;
float _1_PI = (1.0 / PI);
float _1_2PI = (1.0 / (2*PI));
//=======================

in VS_OUT {
	flat int matIdx;
	vec2 texCoords;

	vec3 v_normal;
	vec3 v_light;
	vec3 v_view;

	vec3 p_pos;
	vec3 p_view;
	vec3 p_light;

	vec3 L_light;

	mat3 TBN;
} data;

out vec4 FragColor;

uniform uint64_t tex_irradianceMap;
uniform uint64_t tex_environmentMap;
uniform uint64_t tex_integrationMap;

uniform int envMap_maxLevel;

uniform int forceColorRMA;

//====== Material structure ======
struct Material {
	vec3 diffuse;
	uint64_t texDiffuse;

	vec3 rma;
	uint64_t texRma;		//roughness, metalness, ior

	vec3 normal;
	uint64_t texNormal;
};

layout(std430, binding = 0) readonly buffer Materials {
	Material materials[];
};

//====== Functions ======
vec2 SphereCoords(vec3 n);
vec3 Tex2D(uint64_t tex, vec2 coords);

float FresnelSchlick(float cosTheta, float F0);
float DistributionGGX(float cosThetaN, float alpha);
float GeometricAttenuation(float alpha, float cosThetaO, float cosThetaI);

vec3 IrradianceMap(vec3 n);
vec3 PrefEnvMap(vec3 omegaR, float alpha);
vec2 BRDFIntMap(float cosThetaOmegaO, float alpha);

//=================================

void main( void ) {
	Material mat = materials[data.matIdx];

	vec3 v_light = normalize(data.p_light - data.p_pos);
	vec3 v_view = normalize(data.p_view - data.p_pos);

	//normal
	vec3 normal = data.v_normal;
	if(mat.texNormal != 0) normal = data.TBN * normalize(2.f * Tex2D(mat.texNormal, data.texCoords) - 1f);
	if(dot(normal, v_view) < 0)
		normal *= -1.f;

	//RMA,ior
	vec3 rma;
	float roughness; 
	float metalness;
	if(mat.texRma != 0)
		rma = Tex2D(mat.texRma, data.texCoords);
	else {
		rma = mat.rma;
		rma.z = 1.f;	//non-texture z-coord contains ior instead of ao
	}
	if(forceColorRMA != 0) {
		roughness = mat.rma.x;
		metalness = mat.rma.y;
	}
	else {
		roughness = rma.x;
		metalness = rma.y;
	}
	float ao		= rma.z;

	float ior = mat.rma.z;
	float F0 = (1-ior)/(1+ior); F0 *= F0;

	vec3 omegaO = v_view;
	vec3 omegaI = reflect(-omegaO, normal);
	vec3 omegaH = normalize(v_light + v_view);

	float cosThetaO = max(dot(normal, omegaO), 0.01);
	float cosThetaI = max(dot(normal, omegaI), 0.01);
	float cosThetaH = max(dot(omegaO, omegaH), 0.01);
	float cosThetaN = max(dot(normal, omegaH), 0.01);

	vec3 albedo = mat.diffuse.rgb * Tex2D(mat.texDiffuse, data.texCoords);

	float ks = FresnelSchlick(cosThetaO, F0);
	float kd = (1-ks) * (1-metalness);

	//ambient approximation - IBL
	vec3 Ld = albedo * IrradianceMap(normal);
	vec3 Ls = PrefEnvMap(omegaI, roughness);
	vec2 sb = BRDFIntMap(cosThetaO, roughness);
	vec3 Lo_ambient = (kd * Ld + (ks * sb.x + sb.y) * Ls) * ao;

	//direct lighting
	float D = DistributionGGX(cosThetaN, roughness);
	float F = FresnelSchlick(cosThetaH, F0);
	float G = GeometricAttenuation(roughness, cosThetaO, cosThetaI);
	float specular = (D * F * G) / (4*cosThetaO*cosThetaI);
	vec3 Lo = (kd * albedo * _1_PI + specular) * cosThetaI * data.L_light;

	vec3 clr = vec3(0,0,0);
	clr += Lo;
	clr += Lo_ambient;

	//gamma corection
	clr = clr / (clr + vec3(1.0));
	clr = pow(clr, vec3(1.0/2.2)); 

	FragColor = vec4(clr, 1.f);
//	FragColor = vec4((normal+1)*0.5,1.0);
}

//====== Functions ======

vec2 SphereCoords(vec3 n) {
	return vec2( (atan(n.y, n.x)+PI) * _1_2PI, acos(n.z) * _1_PI);
}

vec3 Tex2D(uint64_t tex, vec2 coords) {
	return texture(sampler2D(tex), coords).rgb;
}

float FresnelSchlick(float cosThetaH, float F0) {
	float tmp = 1 - cosThetaH;
	return F0 + (1 - F0) * tmp * tmp * tmp * tmp * tmp;
}

float DistributionGGX(float cosThetaN, float alpha) {
	float denom  = cosThetaN * cosThetaN * (alpha * alpha - 1) + 1;
	return (alpha * alpha) / (PI * denom * denom);
}

float GeometricAttenuation(float alpha, float cosThetaO, float cosThetaI) {
	float a2 = alpha * alpha;
	return (2*cosThetaO*cosThetaI)/(cosThetaO * sqrt(a2+(1-a2)*cosThetaI*cosThetaI) + cosThetaI * sqrt(a2+(1-a2)*cosThetaO*cosThetaO));
}

vec3 IrradianceMap(vec3 n) {
	return Tex2D(tex_irradianceMap, SphereCoords(n));
}

vec3 PrefEnvMap(vec3 omegaR, float alpha) {
	return textureLod(sampler2D(tex_environmentMap), SphereCoords(omegaR), alpha * envMap_maxLevel).rgb;
}

vec2 BRDFIntMap(float cosThetaO, float alpha) {
	return Tex2D(tex_integrationMap, vec2(cosThetaO, alpha)).rg;
}