#include "pch.h"
#include "SceneDualRasterizer.h"
#include "SceneManager.h"
#include "EOBJParser.h"
#include "Mesh.h"
#include "LightManager.h"
#include "Lights.h"

SceneDualRasterizer::SceneDualRasterizer(ID3D11Device* pDevice)
	: GameScene(SceneManager::Get()->GetNextAvailableIndex())
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	Elite::ParseOBJ("Resources/vehicle.obj", vertices, indices);
	auto vehicleMesh = AddObject(new TriangleMesh(pDevice, vertices, indices, L"Resources/PosCol3D.fx", "Resources/vehicle_diffuse.png"));
	vehicleMesh->SetNormalMap("Resources/vehicle_normal.png");
	vehicleMesh->SetSpecularMap("Resources/vehicle_specular.png");
	vehicleMesh->SetGlossinessMap("Resources/vehicle_gloss.png");

	Elite::ParseOBJ("Resources/fireFX.obj", vertices, indices);
	auto fireFXMesh = AddObject(new TriangleMesh(pDevice, vertices, indices, L"Resources/Combustion.fx", "Resources/fireFX_diffuse.png", false));

	LightManager::Get()->AddLight(new DirectionalLight({ .577f, -.577f, -.577f }, { 1,1,1 }, 7.f));
}