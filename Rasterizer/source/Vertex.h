#pragma once
#include "EMath.h"
#include "ERGBColor.h"

struct Vertex
{
	Elite::FPoint4 position{};
	Elite::RGBColor color{};
	Elite::FVector2 uv{};
	Elite::FVector3 normal{};
	Elite::FVector3 tangent{};
};