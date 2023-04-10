#pragma once
#include "Materials.h"
#include <vector>
#include "Objects.h"
#include "Lights.h"

class GameScene
{
public:
	GameScene(int sceneIndex) : m_SceneIndex(sceneIndex) {}
	~GameScene();
	GameScene(const GameScene& other) = delete;
	GameScene(GameScene&& other) noexcept = delete;
	GameScene& operator=(const GameScene& other) = delete;
	GameScene& operator=(GameScene&& other) noexcept = delete;

	std::vector<Object*>& GetObjects() { return m_pObjects; }
	std::vector<Light*>& GetLights() { return m_pLights; }
	int GetSceneIndex() const { return m_SceneIndex; }

protected:
	std::vector<Object*> m_pObjects;
	std::vector<Light*> m_pLights;
	std::vector<Material*> m_pMaterials;
	int m_SceneIndex;

	void AddObject(Object* object) { m_pObjects.push_back(object); }
	void AddLight(Light* light) { m_pLights.push_back(light); }
};