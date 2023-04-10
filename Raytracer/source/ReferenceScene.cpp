#include "ReferenceScene.h"
#include "Materials.h"
#include "SceneManager.h"

ReferenceScene::ReferenceScene()
	: GameScene(SceneManager::Get()->GetNextAvailableIndex())
{
	//MATERIALS
	Material* PBR_RoughMetal = new MaterialPBR{ Elite::RGBColor{ .972f, .960f, .915f }, 1, 1.f };
	m_pMaterials.push_back(PBR_RoughMetal);
	Material* PBR_MediumMetal = new MaterialPBR{ Elite::RGBColor{ .972f, .960f, .915f }, 1, .6f };
	m_pMaterials.push_back(PBR_MediumMetal);
	Material* PBR_SmoothMetal = new MaterialPBR{ Elite::RGBColor{ .972f, .960f, .915f }, 1, .1f };
	m_pMaterials.push_back(PBR_SmoothMetal);
	Material* PBR_RoughPlastic = new MaterialPBR{ Elite::RGBColor{ .75f, .75f, .75f }, 0, 1.f };
	m_pMaterials.push_back(PBR_RoughPlastic);
	Material* PBR_MediumPlastic = new MaterialPBR{ Elite::RGBColor{ .75f, .75f, .75f }, 0, .4f };
	m_pMaterials.push_back(PBR_MediumPlastic);
	Material* PBR_SmoothPlastic = new MaterialPBR{ Elite::RGBColor{ .75f, .75f, .75f }, 0, .1f };
	m_pMaterials.push_back(PBR_SmoothPlastic);
	Material* Lambert_GreyBlue = new MaterialLambert{ Elite::RGBColor{ 0.49f, 0.57f, 0.57f }, 1.f };
	m_pMaterials.push_back(Lambert_GreyBlue);
	Material* Lambert_White = new MaterialLambert{ Elite::RGBColor{ 1.f, 1.f, 1.f }, 1.f };
	m_pMaterials.push_back(Lambert_White);
	//GEOMETRY - PLANES
	AddObject(new Plane { Elite::FPoint3{ 0.f, 0.f, 0.f }, Elite::FVector3{ 0.f, 1.f, 0.f }, Lambert_GreyBlue });
	AddObject(new Plane { Elite::FPoint3{ 0.f, 0.f, -10.f }, Elite::FVector3{ 0.f, 0.f, 1.f }, Lambert_GreyBlue });
	AddObject(new Plane { Elite::FPoint3{ 0.f, 10.f, 0.f }, Elite::FVector3{ 0.f, -1.f, 0.f }, Lambert_GreyBlue });
	AddObject(new Plane { Elite::FPoint3{ 5.f, 0.f, 0.f }, Elite::FVector3{ -1.f, 0.f, 0.f }, Lambert_GreyBlue });
	AddObject(new Plane { Elite::FPoint3{ -5.f, 0.f, 0.f }, Elite::FVector3{ 1.f, 0.f, 0.f }, Lambert_GreyBlue });
	//GEOMETRY - SPHERES
	AddObject(new Sphere{ Elite::FPoint3{ -1.75f, 1.f, 0.f }, PBR_RoughMetal, .75f });
	AddObject(new Sphere{ Elite::FPoint3{ 0.f, 1.f, 0.f }, PBR_MediumMetal, .75f });
	AddObject(new Sphere{ Elite::FPoint3{ 1.75f, 1.f, 0.f }, PBR_SmoothMetal, .75f });
	AddObject(new Sphere{ Elite::FPoint3{ -1.75f, 3.f, 0.f }, PBR_RoughPlastic, .75f });
	AddObject(new Sphere{ Elite::FPoint3{ 0.f, 3.f, 0.f }, PBR_MediumPlastic, .75f });
	AddObject(new Sphere{ Elite::FPoint3{ 1.75f, 3.f, 0.f }, PBR_SmoothPlastic, .75f });
	//GEOMETRY - TRIANGLES
	std::vector<Elite::FPoint3> vertices;
	vertices.push_back({ -.75f, 1.5f, 0.f });
	vertices.push_back({ -.75f, 0.f, 0.f });
	vertices.push_back({ .75f, 0.f, 0.f });
	AddObject(new Triangle{ Elite::FPoint3{ -1.75f,4.5, 0 }, vertices, Lambert_White, Triangle::CullMode::backFace });
	AddObject(new Triangle{ Elite::FPoint3{ 0.f,4.5, 0 }, vertices, Lambert_White, Triangle::CullMode::frontFace });
	AddObject(new Triangle{ Elite::FPoint3{ 1.75f,4.5, 0 }, vertices, Lambert_White, Triangle::CullMode::noCulling });
	//LIGHTS
	AddLight(new PointLight{ Elite::FPoint3{ 0.f, 5.f, -5.f }, Elite::RGBColor{ 1.f, .61f, .45f }, 50.f });
	AddLight(new PointLight{ Elite::FPoint3{ -2.5f, 5.f, 5.f }, Elite::RGBColor{ 1.f, .8f, .45f }, 70.f });
	AddLight(new PointLight{ Elite::FPoint3{ 2.5f, 2.5f, 5.f }, Elite::RGBColor{ .34f, .47f, .68f }, 50.f });
}
