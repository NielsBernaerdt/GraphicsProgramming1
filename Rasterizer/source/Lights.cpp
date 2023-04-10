#include "pch.h"
#include "Lights.h"

Elite::RGBColor PointLight::CalculateRadiance(const Elite::FPoint3& pointToShade) const
{
	return m_Color * (m_Intensity / Elite::SqrMagnitude(m_Position - pointToShade));
}

Elite::FVector3 PointLight::GetDirection(const Elite::FPoint3& pointToShade) const
{ //NON NORMALIZED
	return m_Position - pointToShade;
}

Elite::RGBColor DirectionalLight::CalculateRadiance(const Elite::FPoint3&) const
{
	return m_Color * m_Intensity;
}

Elite::FVector3 DirectionalLight::GetDirection(const Elite::FPoint3&) const
{
	return m_Direction;
}