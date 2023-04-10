#pragma once
#include <vector>
#include "Mesh.h"

class GameScene
{
public:
	GameScene(int sceneIndex);
	~GameScene();
	GameScene(const GameScene& other) = delete;
	GameScene(GameScene&& other) noexcept = delete;
	GameScene& operator=(const GameScene& other) = delete;
	GameScene& operator=(GameScene&& other) noexcept = delete;

	void Update(float deltaT);
	void ToggleDisabilityObject(int index);
	void SwitchSamplingState();
	std::vector<TriangleMesh*>& GetObjects() { return m_pObjects; }
	int GetSceneIndex() const { return m_SceneIndex; }

protected:
	std::vector<TriangleMesh*> m_pObjects;
	int m_SceneIndex;

	TriangleMesh* AddObject(TriangleMesh* object);
};