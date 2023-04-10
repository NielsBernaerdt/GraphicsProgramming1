#pragma once
#include <vector>
#include "Vertex.h"

class Effect;
class Texture;
class Camera;

enum class SamplingState
{
	point,
	linear,
	anisotropic
};

class Triangle
{
public:
	Triangle(Vertex v0, Vertex v1, Vertex v2) {
		m_Triangle.push_back(v0);
		m_Triangle.push_back(v1);
		m_Triangle.push_back(v2);
	}
	std::vector<Vertex>& GetVertices() { return m_Triangle; }
private:
	std::vector<Vertex> m_Triangle;
};

class TriangleMesh
{
public:
	TriangleMesh(ID3D11Device* pDevice, std::vector<Vertex> vertices, std::vector<uint32_t> indices, const std::wstring& effectFile, const std::string& textureFile, bool isActive = true);
	~TriangleMesh();
	TriangleMesh(const TriangleMesh& other) = delete;
	TriangleMesh(TriangleMesh&& other) noexcept = delete;
	TriangleMesh& operator=(const TriangleMesh& other) = delete;
	TriangleMesh& operator=(TriangleMesh&& other) noexcept = delete;

	void Update(float deltaT);
	void ToggleDisability();
	int SwitchSamplingState();
	bool IsActive() { return m_IsActive; }

//Hardware Rasterizer
	void Render(ID3D11DeviceContext* pDeviceContext, Camera* pCamera);
//Software Rasterizer
	const std::vector<Triangle*>& GetTriangles() const { return m_pTriangles; }
	Texture* GetDiffuseMap() { return m_pDiffuseMap; }
	void SetNormalMap(const std::string& assetFile);
	Texture* GetNormalMap() { return m_pNormalMap; }
	void SetSpecularMap(const std::string& assetFile);
	Texture* GetSpecularMap() { return m_pSpecularMap; }
	void SetGlossinessMap(const std::string& assetFile);
	Texture* GetGlossinessMap() { return m_pGlossinessMap; }
private:
	void RotateSoftware(float deltaT, float angle);
	void RotateHardware(float deltaT, float angle);

	Elite::FPoint3 m_Origin{ 0,0,0 };
	Elite::FMatrix4 m_World;
	float accAngle{};
	bool m_IsActive{ true };
	SamplingState m_Samplingstate{ SamplingState::point };
//Hardware Rasterizer
	Effect* m_pEffect;
	Texture* m_pDiffuseMap;
	Texture* m_pNormalMap;
	Texture* m_pSpecularMap;
	Texture* m_pGlossinessMap;
	ID3D11Device* m_pDevice;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11Buffer* m_pIndexBuffer;
	uint32_t m_AmountIndices{};
//Software Rasterizer
	std::vector<Triangle*> m_pTriangles;
	std::vector<Vertex> m_pWorldVertices;
};