#pragma once
#include "Lights.h"
#include <vector>
#include <iostream>

class LightManager final
{
public:
	static LightManager* Create()
	{
		if (!m_IsInitialized)
		{
			m_pInstance = new LightManager();
			m_IsInitialized = true;
		}
		return m_pInstance;
	}
	static LightManager* Get()
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
	LightManager(const LightManager& other) = delete;
	LightManager(LightManager&& other) noexcept = delete;
	LightManager& operator=(const LightManager& other) = delete;
	LightManager& operator=(LightManager&& other) noexcept = delete;

	Light* AddLight(Light* pLight) { m_pLights.push_back(pLight); return pLight; }
	const std::vector<Light*>& GetLights() const { return m_pLights; }

private:
	LightManager() = default;
	~LightManager() {
		for (Light* light : m_pLights)
		{
			delete light;
			light = nullptr;
		}
	}
	static bool m_IsInitialized;
	static LightManager* m_pInstance;

	std::vector<Light*> m_pLights{};
};