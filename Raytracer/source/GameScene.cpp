#include "GameScene.h"

GameScene::~GameScene()
{
	//OBJECTS
	for (auto object : m_pObjects)
	{
		delete object;
		object = nullptr;
	}
	//LIGHTS
	for (auto light : m_pLights)
	{
		delete light;
		light = nullptr;
	}
	//MATERIALS
	for (auto material : m_pMaterials)
	{
		delete material;
		material = nullptr;
	}
}