#pragma once
#include <memory>
#include "EMath.h"
#include "ERGBColor.h"

struct Material;

struct Ray
{	
	Elite::FPoint3 origin{};
	Elite::FVector3 direction{};
	float tMin = 0.0001f;
	float tMax = FLT_MAX;
};

struct HitRecord
{
	Elite::FPoint3 hitPoint{};
	Material* material;
	float tValue = -1.f;
	Elite::FVector3 normal{};
};