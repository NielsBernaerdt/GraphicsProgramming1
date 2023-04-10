#pragma once
#include "EMath.h"
#include "ERGBColor.h"
#include "Ray.h"

class Light
{
public:
	virtual Elite::RGBColor CalculateRadiance(const Elite::FPoint3& pointToShade) const = 0;
	virtual Elite::FVector3 GetDirection(const Elite::FPoint3& pointToShade) const = 0; //NON NORMALIZED
};

class PointLight : public Light
{
public:
	PointLight(Elite::FPoint3 pos, Elite::RGBColor color, float intensity)
		: m_Position(pos)
		, m_Color(color)
		, m_Intensity(intensity) {}

	Elite::RGBColor CalculateRadiance(const Elite::FPoint3& pointToShade) const override;
	Elite::FVector3 GetDirection(const Elite::FPoint3& pointToShade) const override; //NON NORMALIZED

private:
	Elite::FPoint3 m_Position;
	Elite::RGBColor m_Color;
	float m_Intensity;
};

class DirectionalLight : public Light
{
public:
	DirectionalLight(Elite::FVector3 dir, Elite::RGBColor color, float intensity)
		: m_Direction(dir)
		, m_Color(color)
		, m_Intensity(intensity) {}

	Elite::RGBColor CalculateRadiance(const Elite::FPoint3& pointToShade) const override;
	Elite::FVector3 GetDirection(const Elite::FPoint3& pointToShade) const override; //NON NORMALIZED

private:
	Elite::FVector3 m_Direction;
	Elite::RGBColor m_Color;
	float m_Intensity;
};