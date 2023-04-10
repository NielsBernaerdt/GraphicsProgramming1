#include "pch.h"
#include "GameScene.h"


GameScene::GameScene(int sceneIndex)
	: m_SceneIndex(sceneIndex) 
{}

GameScene::~GameScene()
{
	for (auto object : m_pObjects)
	{
		delete object;
		object = nullptr;
	}
}

void GameScene::Update(float deltaT)
{
	for (const auto& object : m_pObjects)
		object->Update(deltaT);
}

void GameScene::SwitchSamplingState()
{
	int currentSamplingState{};
	for (const auto& object : m_pObjects)
		currentSamplingState = object->SwitchSamplingState();

	if(currentSamplingState == 0)
		std::cout << "Active Sampling State: point       \tPress 'F' to toggle again.\n";
	else if (currentSamplingState == 1)
		std::cout << "Active Sampling State: linear      \tPress 'F' to toggle again.\n";
	else if (currentSamplingState == 2)
		std::cout << "Active Sampling State: anisotropic \tPress 'F' to toggle again.\n";
}

void GameScene::ToggleDisabilityObject(int index)
{
	m_pObjects[index]->ToggleDisability();
}

TriangleMesh* GameScene::AddObject(TriangleMesh* object)
{
	m_pObjects.push_back(object);
	return object;
}