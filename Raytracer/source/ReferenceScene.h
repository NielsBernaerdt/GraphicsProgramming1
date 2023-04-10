#pragma once
#include "GameScene.h"

class ReferenceScene : public GameScene
{
public:
	ReferenceScene();
	~ReferenceScene() = default;
	ReferenceScene(const ReferenceScene& other) = delete;
	ReferenceScene(ReferenceScene&& other) noexcept = delete;
	ReferenceScene& operator=(const ReferenceScene& other) = delete;
	ReferenceScene& operator=(ReferenceScene&& other) noexcept = delete;
};