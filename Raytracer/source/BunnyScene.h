#pragma once
#include "GameScene.h"
class BunnyScene : public GameScene
{
public:
	BunnyScene();
	~BunnyScene() = default;
	BunnyScene(const BunnyScene& other) = delete;
	BunnyScene(BunnyScene&& other) noexcept = delete;
	BunnyScene& operator=(const BunnyScene& other) = delete;
	BunnyScene& operator=(BunnyScene&& other) noexcept = delete;
};