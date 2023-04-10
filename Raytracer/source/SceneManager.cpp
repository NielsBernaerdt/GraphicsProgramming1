#include "SceneManager.h"

SceneManager* SceneManager::m_pInstance{};
bool SceneManager::m_IsInitialized{};

SceneManager::~SceneManager()
{
	for (GameScene* scene : m_pScenes)
	{
		delete scene;
		scene = nullptr;
	}
}

GameScene* SceneManager::AddGameScene(GameScene* pScene)
{
	m_pScenes.push_back(pScene);
	if (m_ActiveScene == nullptr)
		m_ActiveScene = pScene;
	return pScene;
}

void SceneManager::NextScene()
{
	for (unsigned int i = 0; i < m_pScenes.size(); ++i)
	{
		if (m_pScenes[i]->GetSceneIndex() == m_ActiveScene->GetSceneIndex())
		{
			auto nextScene{ 0 };
			if (i >= m_pScenes.size() - 1)
				nextScene = 0;
			else
				nextScene = i + 1;
			m_ActiveScene = m_pScenes[nextScene];
			std::cout << "New scene opened. Press 'F4' to open the next scene.\n";
			break;
		}
	}
}