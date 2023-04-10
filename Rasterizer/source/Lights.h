#pragma once
#include "EMath.h"
#include "ERGBColor.h"

class Light
{
public:
	Light(Elite::RGBColor color, float intensity) : m_Color(color), m_Intensity(intensity) {}
	virtual Elite::RGBColor CalculateRadiance(const Elite::FPoint3& pointToShade) const = 0;
	virtual Elite::FVector3 GetDirection(const Elite::FPoint3& pointToShade) const = 0; //NON NORMALIZED
	const Elite::RGBColor& GetColor() const { return m_Color; }
	float GetIntensity() const { return m_Intensity; }
protected:
	Elite::RGBColor m_Color;
	float m_Intensity;
};

class PointLight : public Light
{
public:
	PointLight(Elite::FPoint3 pos, Elite::RGBColor color, float intensity)
		: Light(color, intensity)
		, m_Position(pos) {}

	Elite::RGBColor CalculateRadiance(const Elite::FPoint3& pointToShade) const override;
	Elite::FVector3 GetDirection(const Elite::FPoint3& pointToShade) const override; //NON NORMALIZED

private:
	Elite::FPoint3 m_Position;
};

class DirectionalLight : public Light
{
public:
	DirectionalLight(Elite::FVector3 dir, Elite::RGBColor color, float intensity)
		: Light(color, intensity)
		, m_Direction(dir) {}

	Elite::RGBColor CalculateRadiance(const Elite::FPoint3& pointToShade) const override;
	Elite::FVector3 GetDirection(const Elite::FPoint3& pointToShade) const override; //NON NORMALIZED

private:
	Elite::FVector3 m_Direction;
};