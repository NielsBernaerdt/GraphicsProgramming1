#include "Objects.h"
#include <iostream>
#include "Ray.h"
#include "Materials.h"

Plane::Plane(Elite::FPoint3 origin, Elite::FVector3 normal, Material* material)
	: m_Origin{origin}
	, m_Normal{normal}
	, m_pMaterial{material}
{
	m_pBoundingBox = new BoundingBox{ {-FLT_MAX, -FLT_MAX, 0}, {FLT_MAX, FLT_MAX, 0} };
}
bool Plane::Hit(const Ray& ray, HitRecord& hitRecord) const
{
	float t{ Elite::Dot(m_Origin - ray.origin, m_Normal) / Elite::Dot(ray.direction, m_Normal) };

	if (t >= ray.tMin && t <= ray.tMax)
	{
		hitRecord.hitPoint = ray.origin + t * ray.direction;
		hitRecord.material = m_pMaterial;
		hitRecord.tValue = t;
		hitRecord.normal = m_Normal;
		return true;
	}
	return false;
}

Sphere::Sphere(Elite::FPoint3 origin, Material* material, float radius)
	: m_Origin{origin}
	, m_pMaterial{material}
	, m_Radius{radius}
{
	Elite::FPoint3 min{ m_Origin.x - m_Radius, m_Origin.y - m_Radius , m_Origin.z - m_Radius };
	Elite::FPoint3 max{ m_Origin.x + m_Radius, m_Origin.y + m_Radius , m_Origin.z + m_Radius };
	m_pBoundingBox = new BoundingBox{ min, max };
}
bool Sphere::Hit(const Ray& ray, HitRecord& hitRecord) const
{
	float a{ Elite::Dot(ray.direction, ray.direction) };
	float b{ Elite::Dot(2 * ray.direction, ray.origin - m_Origin) };
	float c{ Elite::Dot(ray.origin - m_Origin, ray.origin - m_Origin) - (m_Radius * m_Radius) };
	float discriminant{ b * b - 4 * a * c };
	float t{};

	if (discriminant < 0)
	{
		return false;
	}
	t = (-b - sqrt(discriminant)) / (2 * a); //first check with - => is closest
	if (t < ray.tMin)
	{
		t = (-b + sqrt(discriminant)) / (2 * a);
	}
	if (t <= ray.tMax && t >= ray.tMin)
	{
		hitRecord.hitPoint = ray.origin + t * ray.direction;
		hitRecord.material = m_pMaterial;
		hitRecord.tValue = t;
		hitRecord.normal = Elite::FVector3{ hitRecord.hitPoint - m_Origin } / m_Radius;
		return true;
	}
	return false;
}

Triangle::Triangle(Elite::FPoint3 origin, Elite::FPoint3 v0, Elite::FPoint3 v1, Elite::FPoint3 v2
	, Material* material, CullMode cullMode)
	: m_Origin(origin)
	, m_pMaterial(material)
	, m_CullMode(cullMode)
{
	m_RelativeVertices.push_back(v0);
	m_RelativeVertices.push_back(v1);
	m_RelativeVertices.push_back(v2);
	m_WorldVertices.push_back(v0 + static_cast<Elite::FVector3>(origin));
	m_WorldVertices.push_back(v1 + static_cast<Elite::FVector3>(origin));
	m_WorldVertices.push_back(v2 + static_cast<Elite::FVector3>(origin));

	float minX{ m_WorldVertices[0].x }, maxX{ m_WorldVertices[0].x };
	float minY{ m_WorldVertices[0].y }, maxY{ m_WorldVertices[0].y };
	float minZ{ m_WorldVertices[0].z }, maxZ{ m_WorldVertices[0].z };
	for (const auto& v : m_WorldVertices)
	{
		//Store parameters for bounding box
		if (v.x < minX)
			minX = v.x;
		if (v.x > maxX)
			maxX = v.x;
		if (v.y < minY)
			minY = v.y;
		if (v.y > maxY)
			maxY = v.y;
		if (v.z < minZ)
			minZ = v.z;
		if (v.z > maxZ)
			maxZ = v.z;
	}
	m_pBoundingBox = new BoundingBox{ {minX, minY, minZ},{maxX, maxY, maxZ} };
}
Triangle::Triangle(Elite::FPoint3 origin, std::vector<Elite::FPoint3> vertices, Material* material, CullMode cullMode)
	: m_Origin(origin)
	, m_RelativeVertices(vertices)
	, m_pMaterial(material)
	, m_CullMode(cullMode)
{
	m_WorldVertices.push_back(m_RelativeVertices[0] + static_cast<Elite::FVector3>(origin));
	m_WorldVertices.push_back(m_RelativeVertices[1] + static_cast<Elite::FVector3>(origin));
	m_WorldVertices.push_back(m_RelativeVertices[2] + static_cast<Elite::FVector3>(origin));

	float minX{ m_WorldVertices[0].x }, maxX{ m_WorldVertices[0].x };
	float minY{ m_WorldVertices[0].y }, maxY{ m_WorldVertices[0].y };
	float minZ{ m_WorldVertices[0].z }, maxZ{ m_WorldVertices[0].z };
	for (const auto& v : m_WorldVertices)
	{
		//Store parameters for bounding box
		if (v.x < minX)
			minX = v.x;
		if (v.x > maxX)
			maxX = v.x;
		if (v.y < minY)
			minY = v.y;
		if (v.y > maxY)
			maxY = v.y;
		if (v.z < minZ)
			minZ = v.z;
		if (v.z > maxZ)
			maxZ = v.z;
	}
	m_pBoundingBox = new BoundingBox{ {minX, minY, minZ},{maxX, maxY, maxZ} };

}
bool Triangle::Hit(const Ray& ray, HitRecord& hitRecord) const
{
	const Elite::FPoint3& wv0{ m_WorldVertices[0] };
	const Elite::FPoint3& wv1{ m_WorldVertices[1] };
	const Elite::FPoint3& wv2{ m_WorldVertices[2] };

	auto v1{ wv1 - wv0 };
	auto v2{ wv2 - wv0 };
	auto normal{ Elite::Cross(v1, v2) };
	//Back-face
	if (m_CullMode == CullMode::backFace && Elite::Dot(normal, ray.direction) > 0)
		return false;
	//Front-face
	if (m_CullMode == CullMode::frontFace && Elite::Dot(normal, ray.direction) < 0)
		return false;

	Elite::FPoint3 center{ (wv0.x + wv1.x + wv2.x) / 3
							, (wv0.y + wv1.y + wv2.y) / 3
							, (wv0.z + wv1.z + wv2.z) / 3 };
	auto L{ center - ray.origin };
	auto t{ (Elite::Dot(L, normal) / Elite::Dot(ray.direction, normal)) };
	if (t < ray.tMin || t > ray.tMax)
		return false;
	auto intersectionPoint{ ray.origin + t * ray.direction };

	// Check 1
	Elite::FVector3 edgeA{ wv1 - wv0 };
	Elite::FVector3 pointToSideA{ intersectionPoint - wv0 };
	if (Elite::Dot(normal, Elite::Cross(edgeA, pointToSideA)) < 0)
		return false;
	// Check 2
	Elite::FVector3 edgeB{ wv2 - wv1 };
	Elite::FVector3 pointToSideB{ intersectionPoint - wv1 };
	if (Elite::Dot(normal, Elite::Cross(edgeB, pointToSideB)) < 0)
		return false;
	// Check 3
	Elite::FVector3 edgeC{ wv0 - wv2 };
	Elite::FVector3 pointToSideC{ intersectionPoint - wv2 };
	if (Elite::Dot(normal, Elite::Cross(edgeC, pointToSideC)) < 0)
		return false;

	hitRecord.hitPoint = intersectionPoint;
	hitRecord.normal = normal;
	hitRecord.tValue = t;
	hitRecord.material = m_pMaterial;
	return true;
}

void Triangle::Rotate(float deltaTime)
{
	Elite::FMatrix4 translateBack{
	1,	0,	0,	m_Origin.x,
	0,	1,	0,	m_Origin.y,
	0,	0,	1,	m_Origin.z,
	0,	0,	0,	1	};
	Elite::FMatrix4 translateToOrigin{
	1,	0,	0,	-m_Origin.x,
	0,	1,	0,	-m_Origin.y,
	0,	0,	1,	-m_Origin.z,
	0,	0,	0,	1 };
	Elite::FMatrix4 rotMatrix{
	cos(deltaTime),		0,				sin(deltaTime),		0,
	0,					1,				0,					0,
	-sin(deltaTime),	0,				cos(deltaTime),		0,
	0,					0,				0,					1 };
	Elite::FPoint4 p;
	for (auto& v : m_WorldVertices)
	{
		p = { v.x, v.y, v.z, 1 };
		v = (translateBack * rotMatrix * translateToOrigin * p).xyz;
	}
}

TriangleMesh::TriangleMesh(Elite::FPoint3 origin, std::vector<Elite::FPoint3> vertexBuffer, std::vector<IDList> indexBuffer, Material* material)
	: m_Origin(origin)
	, m_VertexBuffer(vertexBuffer)
	, m_IndexBuffer(indexBuffer)
	, m_pMaterial(material)
{
	float minX{ vertexBuffer[0].x }, maxX{ vertexBuffer[0].x };
	float minY{ vertexBuffer[0].y }, maxY{ vertexBuffer[0].y };
	float minZ{ vertexBuffer[0].z }, maxZ{ vertexBuffer[0].z };
	for(size_t i{}; i < m_IndexBuffer.size(); ++i)
	{
		m_pTriangles.push_back(new Triangle{ m_Origin
				, m_VertexBuffer[m_IndexBuffer[i].index0]
				, m_VertexBuffer[m_IndexBuffer[i].index1]
				, m_VertexBuffer[m_IndexBuffer[i].index2]
				, m_pMaterial
				, Triangle::CullMode::backFace});

		//Store parameters for bounding box
		if (vertexBuffer[i].x < minX)
			minX = vertexBuffer[i].x;
		if (vertexBuffer[i].x > maxX)
			maxX = vertexBuffer[i].x;
		if (vertexBuffer[i].y < minY)
			minY = vertexBuffer[i].y;
		if (vertexBuffer[i].y > maxY)
			maxY = vertexBuffer[i].y;
		if (vertexBuffer[i].z < minZ)
			minZ = vertexBuffer[i].z;
		if (vertexBuffer[i].z > maxZ)
			maxZ = vertexBuffer[i].z;
	}
	m_pBoundingBox = new BoundingBox{ {minX, minY, minZ},{maxX, maxY, maxZ} };
}
TriangleMesh::TriangleMesh(Elite::FPoint3 origin, std::vector<Elite::FPoint3> vertexBuffer, Material* pmaterial)
	:m_Origin(origin)
	, m_VertexBuffer(vertexBuffer)
	, m_pMaterial(pmaterial)
{
	float minX{ vertexBuffer[0].x }, maxX{ vertexBuffer[0].x };
	float minY{ vertexBuffer[0].y }, maxY{ vertexBuffer[0].y };
	float minZ{ vertexBuffer[0].z }, maxZ{ vertexBuffer[0].z };
	for (size_t i{}; i < vertexBuffer.size(); i += 3)
	{
		m_pTriangles.push_back(new Triangle{ m_Origin
				, m_VertexBuffer[i]
				, m_VertexBuffer[i + 1]
				, m_VertexBuffer[i + 2]
				, m_pMaterial
				, Triangle::CullMode::backFace });
		//Store parameters for bounding box
		if (vertexBuffer[i].x < minX)
			minX = vertexBuffer[i].x;
		if (vertexBuffer[i].x > maxX)
			maxX = vertexBuffer[i].x;
		if (vertexBuffer[i].y < minY)
			minY = vertexBuffer[i].y;
		if (vertexBuffer[i].y > maxY)
			maxY = vertexBuffer[i].y;
		if (vertexBuffer[i].z < minZ)
			minZ = vertexBuffer[i].z;
		if (vertexBuffer[i].z > maxZ)
			maxZ = vertexBuffer[i].z;
	}
	m_pBoundingBox = new BoundingBox{ {minX, minY, minZ},{maxX, maxY, maxZ} };
}
TriangleMesh::~TriangleMesh()
{
	for(auto t : m_pTriangles)
	{
		delete t;
		t = nullptr;
	}
}
bool TriangleMesh::Hit(const Ray& ray, HitRecord& hitRecord) const
{
	bool didHit{ false };
	for (const auto& t : m_pTriangles)
	{
		if (t->Hit(ray, hitRecord))
			didHit = true;
	}
	return didHit;
}

void TriangleMesh::Rotate(float deltaTime)
{
	for (const auto& t : m_pTriangles)
	{
		t->Rotate(deltaTime);
	}
}