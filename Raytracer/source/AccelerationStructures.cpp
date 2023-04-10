#include "AccelerationStructures.h"

bool BoundingBox::Hit(const Ray& ray) const
{
	float tx1{ (m_Min.x - ray.origin.x) * -ray.direction.x };
	float tx2{ (m_Max.x - ray.origin.x) * -ray.direction.x };

	float tMin{ std::min(tx1, tx2) };
	float tMax{ std::max(tx1, tx2) };

	float ty1{ (m_Min.y - ray.origin.y) * -ray.direction.y };
	float ty2{ (m_Max.y - ray.origin.y) * -ray.direction.y };

	tMin = std::max(tMin, std::min(ty1, ty2));
	tMax = std::min(tMax, std::max(ty1, ty2));

	return tMax >= tMin;
}