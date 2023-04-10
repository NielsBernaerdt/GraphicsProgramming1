#pragma once
#include "EMath.h"
#include "Ray.h"

class BoundingBox
{
public:
	BoundingBox(Elite::FPoint3 min, Elite::FPoint3 max) : m_Min(min), m_Max(max) {};
	~BoundingBox() = default;
	bool Hit(const Ray& ray) const;
private:
	Elite::FPoint3 m_Min{};
	Elite::FPoint3 m_Max{};
};