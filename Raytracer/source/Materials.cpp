#include "Materials.h"
#include <iostream>
//BRDF Functions---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Elite::RGBColor BRDF::Lambert(const Elite::RGBColor& diffuseColor, float diffuseReflectance)
{
	return diffuseColor * diffuseReflectance / float(E_PI);
}
Elite::RGBColor BRDF::Phong(const Elite::RGBColor& phongColor, float phongReflectance, float phongExponent
	, const Elite::FVector3& lightDir, const Elite::FVector3& viewDir, const Elite::FVector3& normal)
{
	Elite::FVector3 reflect{ lightDir - 2 * Elite::Dot(normal, lightDir) * normal };
	return phongColor * phongReflectance * pow(Elite::Dot(reflect, viewDir), phongExponent);
}
float BRDF::NormalDistributionFunction(const Elite::FVector3& viewDir, const Elite::FVector3& lightDir, const Elite::FVector3& normal, float roughnessSquared)
{
	Elite::FVector3 halfVector{ Elite::GetNormalized(viewDir + lightDir) };
	return (roughnessSquared * roughnessSquared) / (float(E_PI)
		* (((Elite::Dot(normal, halfVector) * Elite::Dot(normal, halfVector)) * ((roughnessSquared * roughnessSquared) - 1) + 1)
			* ((Elite::Dot(normal, halfVector) * Elite::Dot(normal, halfVector)) * ((roughnessSquared * roughnessSquared) - 1) + 1)));
}
Elite::RGBColor BRDF::FresnelFunction(const Elite::FVector3& viewDir, const Elite::FVector3& lightDir, const Elite::RGBColor& albedoValue, int metalnessValue)
{
	Elite::FVector3 halfVector{ Elite::GetNormalized(viewDir + lightDir) };
	Elite::RGBColor baseReflectance{ (metalnessValue == 0) ? Elite::RGBColor(0.04f, 0.04f, 0.04f) : albedoValue };
	Elite::RGBColor one{ 1, 1, 1 };
	return baseReflectance + (one - baseReflectance) * float(pow(1 - Elite::Dot(halfVector, viewDir), 5));
}
float BRDF::SchlickGeometryFunction(const Elite::FVector3& viewDir, const Elite::FVector3& normal, float roughnessSquared)
{
	float kDirect{ (roughnessSquared + 1) * (roughnessSquared + 1) / 8 };
	return Elite::Dot(normal, viewDir) / (Elite::Dot(normal, viewDir) * (1 - kDirect) + kDirect);
}
float BRDF::SmithGeometryFunction(const Elite::FVector3& viewDir, const Elite::FVector3& lightDir, const Elite::FVector3& normal, float roughnessSquared)
{
	return SchlickGeometryFunction(viewDir, normal, roughnessSquared) * SchlickGeometryFunction(lightDir, normal, roughnessSquared);
}
//Material structs-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MaterialLambert::MaterialLambert(const Elite::RGBColor& color, float reflectance)
	: m_DiffuseColor(color)
	, m_DiffuseReflectance(reflectance) 
{
}
Elite::RGBColor MaterialLambert::Shade(const HitRecord& hit, const Elite::FVector3& lightDir, const Elite::FVector3& viewDir)
{
	return BRDF::Lambert(m_DiffuseColor, m_DiffuseReflectance);
}

MaterialLambertPhong::MaterialLambertPhong(const Elite::RGBColor& color, float diffuseReflectance, float phongReflectance, float phongExponent)
	: m_DiffuseColor(color)
	, m_DiffuseReflectance(diffuseReflectance)
	, m_PhongReflectance(phongReflectance)
	, m_PhongExponent(phongExponent)
{
}
Elite::RGBColor MaterialLambertPhong::Shade(const HitRecord& hit, const Elite::FVector3& lightDir, const Elite::FVector3& viewDir)
{
	return BRDF::Lambert(m_DiffuseColor, m_DiffuseReflectance)
		+ BRDF::Phong(m_DiffuseColor, m_PhongReflectance, m_PhongExponent, lightDir, viewDir, hit.normal);
}

MaterialPBR::MaterialPBR(const Elite::RGBColor& albedo, int metalness, float roughness)
	: m_Albedo(albedo)
	, m_Metalness(metalness)
	, m_Roughness(roughness)
{
}
Elite::RGBColor MaterialPBR::Shade(const HitRecord& hit, const Elite::FVector3& lightDir, const Elite::FVector3& viewDir)
{
	auto F { BRDF::FresnelFunction(viewDir, lightDir, m_Albedo, m_Metalness) };
	auto D{ BRDF::NormalDistributionFunction(viewDir, lightDir, hit.normal, m_Roughness * m_Roughness) };
	auto G{ BRDF::SmithGeometryFunction(viewDir, lightDir, hit.normal, m_Roughness * m_Roughness) };
	auto specular{ (F * D * G) / (4 * (Elite::Dot(viewDir, hit.normal) * Elite::Dot(lightDir, hit.normal))) };
	auto kd{ (m_Metalness == 0) ? 1 - F.r : 0 };
	auto diffuse{ BRDF::Lambert(m_Albedo, kd) };
	return diffuse + specular;
}