#pragma once
#include "GameScene.h"

class Mesh;

class SceneDualRasterizer : public GameScene
{
public:
	SceneDualRasterizer(ID3D11Device* pDevice);
	~SceneDualRasterizer() = default;
	SceneDualRasterizer(const SceneDualRasterizer& other) = delete;
	SceneDualRasterizer(SceneDualRasterizer&& other) noexcept = delete;
	SceneDualRasterizer& operator=(const SceneDualRasterizer& other) = delete;
	SceneDualRasterizer& operator=(SceneDualRasterizer&& other) noexcept = delete;
};