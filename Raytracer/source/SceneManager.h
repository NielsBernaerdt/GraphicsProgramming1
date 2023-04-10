#pragma once
#include "GameScene.h"
#include <vector>
#include <iostream>

class SceneManager final
{
public:
	static SceneManager* Create()
	{
		if (!m_IsInitialized)
		{
			m_pInstance = new SceneManager();
			m_IsInitialized = true;
		}
		return m_pInstance;
	}
	static SceneManager* Get()
	{
		if (!m_IsInitialized) {
			std::cout << "Singleton not yet initialized\n";
			return nullptr;
		}
		if (!m_pInstance)
		{
			std::cout << "Singleton is initialized but instance in NULL\n";
		}
		return m_pInstance;
	}
	static void Destroy()
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
	SceneManager(const SceneManager& other) = delete;
	SceneManager(SceneManager&& other) noexcept = delete;
	SceneManager& operator=(const SceneManager& other) = delete;
	SceneManager& operator=(SceneManager&& other) noexcept = delete;

	GameScene* AddGameScene(GameScene* pScene);
	void NextScene();
	GameScene* GetActiveScene() const { return m_ActiveScene; }
	GameScene* GetScene(int index) const { return m_pScenes[index]; }
	int GetNextAvailableIndex() const { return static_cast<int>(m_pScenes.size()); }

private:
	SceneManager() = default;
	~SceneManager();
	static bool m_IsInitialized;
	static SceneManager* m_pInstance;

	std::vector<GameScene*> m_pScenes{};
	GameScene* m_ActiveScene{};
};