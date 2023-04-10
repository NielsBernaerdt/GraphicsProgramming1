#pragma once
#include "ERGBColor.h"
#include "Ray.h"

namespace BRDF
{
	Elite::RGBColor Lambert(const Elite::RGBColor& diffuseColor, float diffuseReflectance);
	Elite::RGBColor Phong(const Elite::RGBColor& phongColor, float phongReflectance, float phongExponent
		, const Elite::FVector3& lightDir, const Elite::FVector3& viewDir, const Elite::FVector3& normal);
	float NormalDistributionFunction(const Elite::FVector3& viewDir, const Elite::FVector3& lightDir, const Elite::FVector3& normal, float roughnessSquared);
	Elite::RGBColor FresnelFunction(const Elite::FVector3& viewDir, const Elite::FVector3& lightDir, const Elite::RGBColor& baseReflectivity, int metalnessValue);
	float SchlickGeometryFunction(const Elite::FVector3& viewDir, const Elite::FVector3& normal, float roughnessSquared);
	float SmithGeometryFunction(const Elite::FVector3& viewDir, const Elite::FVector3& lightDir, const Elite::FVector3& normal, float roughnessSquared);
}

struct Material
{
	virtual Elite::RGBColor Shade(const HitRecord& hit, const Elite::FVector3& lightDir, const Elite::FVector3& viewDir) = 0;
};

struct MaterialLambert : public Material
{
	Elite::RGBColor m_DiffuseColor{};
	float m_DiffuseReflectance{ 1.f };

	MaterialLambert(const Elite::RGBColor& color, float reflectance);
	Elite::RGBColor Shade(const HitRecord& hit, const Elite::FVector3& lightDir, const Elite::FVector3& viewDir) override;
};

struct MaterialLambertPhong : public Material
{
	Elite::RGBColor m_DiffuseColor{};
	float m_DiffuseReflectance{ 1.f };
	float m_PhongReflectance{ 1.0f };
	float m_PhongExponent{ 60.f };

	MaterialLambertPhong(const Elite::RGBColor& color, float diffuseReflectance, float phongReflectance, float phongExponent);
	Elite::RGBColor Shade(const HitRecord& hit, const Elite::FVector3& lightDir, const Elite::FVector3& viewDir) override;
};

struct MaterialPBR : public Material
{
	Elite::RGBColor m_Albedo{};
	int m_Metalness{};
	float m_Roughness{};

	MaterialPBR(const Elite::RGBColor& albedo, int metalness, float roughness);
	Elite::RGBColor Shade(const HitRecord& hit, const Elite::FVector3& lightDir, const Elite::FVector3& viewDir) override;
};