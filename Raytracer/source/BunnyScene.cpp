#include "BunnyScene.h"
#include "Materials.h"
#include "SceneManager.h"
#include "ObjParser.h"

BunnyScene::BunnyScene()
	: GameScene(SceneManager::Get()->GetNextAvailableIndex())
{
	//MATERIALS
	Material* Lambert_GreyBlue = new MaterialLambert{ Elite::RGBColor{ 0.49f, 0.57f, 0.57f }, 1.f };
	m_pMaterials.push_back(Lambert_GreyBlue);
	Material* Lambert_White = new MaterialLambert{ Elite::RGBColor{ 1.f, 1.f, 1.f }, 1.f };
	m_pMaterials.push_back(Lambert_White);
	//GEOMETRY - PLANES
	AddObject(new Plane{ Elite::FPoint3{ 0.f, 0.f, 0.f }, Elite::FVector3{ 0.f, 1.f, 0.f }, Lambert_GreyBlue});
	AddObject(new Plane{ Elite::FPoint3{ 0.f, 0.f, -10.f }, Elite::FVector3{ 0.f, 0.f, 1.f }, Lambert_GreyBlue});
	AddObject(new Plane{ Elite::FPoint3{ 0.f, 10.f, 0.f }, Elite::FVector3{ 0.f, -1.f, 0.f }, Lambert_GreyBlue });
	AddObject(new Plane{ Elite::FPoint3{ 5.f, 0.f, 0.f }, Elite::FVector3{ -1.f, 0.f, 0.f }, Lambert_GreyBlue});
	AddObject(new Plane{ Elite::FPoint3{ -5.f, 0.f, 0.f }, Elite::FVector3{ 1.f, 0.f, 0.f }, Lambert_GreyBlue});
	//GEOMETRY - TRIANGLE MESH
	std::vector<Elite::FPoint3> verticesTriangleMesh;
	std::vector<int> indices;
	ParseOBJ("Resources/lowpoly_bunny.obj", verticesTriangleMesh, indices);
	AddObject(new TriangleMesh{ Elite::FPoint3{ 0, 0.f, 0.f }, verticesTriangleMesh, Lambert_White });
	//LIGHTS
	AddLight(new PointLight{ Elite::FPoint3{ 0.f, 5.f, -5.f }, Elite::RGBColor{ 1.f, .61f, .45f }, 50.f });
	AddLight(new PointLight{ Elite::FPoint3{ -2.5f, 5.f, 5.f }, Elite::RGBColor{ 1.f, .8f, .45f }, 70.f });
	AddLight(new PointLight{ Elite::FPoint3{ 2.5f, 2.5f, 5.f }, Elite::RGBColor{ .34f, .47f, .68f }, 50.f });
}