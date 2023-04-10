#pragma once
#include <memory>
#include "ERGBColor.h"
#include "EMath.h"
#include <vector>
#include "AccelerationStructures.h"

struct Ray;
struct HitRecord;
struct Material;

class Object
{
public:
	virtual ~Object() { delete m_pBoundingBox; }
	virtual bool Hit(const Ray& ray, HitRecord& hitRecord) const = 0;
	virtual void Rotate(float dT) = 0;
	BoundingBox* GetBoundingBox() { return m_pBoundingBox; }
protected:
	BoundingBox* m_pBoundingBox;
};

class Plane : public Object
{
public:
	Plane(Elite::FPoint3 origin, Elite::FVector3 normal, Material* material = nullptr);
	~Plane() override = default;
	bool Hit(const Ray& ray, HitRecord& hitRecord) const override;
	void Rotate(float dT) override {}
private:
	Elite::FPoint3 m_Origin {};
	Elite::FVector3 m_Normal {};
	Material* m_pMaterial;
};

class Sphere : public Object
{
public:
	Sphere(Elite::FPoint3 origin, Material* material = nullptr, float radius =  10.f);
	~Sphere() override = default;
	bool Hit(const Ray& ray, HitRecord& hitRecord) const override;
	void Rotate(float dT) override {}
private:
	Elite::FPoint3 m_Origin {};
	Material* m_pMaterial;
	float m_Radius{};
};

class Triangle : public Object
{
public:
	enum class CullMode
	{
		frontFace,
		backFace,
		noCulling
	};
	Triangle(Elite::FPoint3 origin, Elite::FPoint3 v0, Elite::FPoint3 v1, Elite::FPoint3 v2
		, Material* material = nullptr, CullMode cullMode = CullMode::noCulling);
	Triangle(Elite::FPoint3 origin, std::vector<Elite::FPoint3> vertices, Material* material = nullptr, CullMode cullMode = CullMode::noCulling);
	~Triangle() override = default;
	bool Hit(const Ray& ray, HitRecord& hitRecord) const override;
	void Rotate(float dT) override;
private:
	Elite::FPoint3 m_Origin;
	std::vector<Elite::FPoint3> m_RelativeVertices;
	std::vector<Elite::FPoint3> m_WorldVertices;
	Material* m_pMaterial;
	CullMode m_CullMode;
};

class TriangleMesh : public Object
{
public:
	struct IDList
	{
		int index0{};
		int index1{};
		int index2{};
	};

	TriangleMesh(Elite::FPoint3 origin, std::vector<Elite::FPoint3> vertexBuffer, std::vector<IDList> indexBuffer, Material* material = nullptr);
	TriangleMesh(Elite::FPoint3 origin, std::vector<Elite::FPoint3> vertexBuffer, Material* pmaterial = nullptr);
	~TriangleMesh() override;
	bool Hit(const Ray& ray, HitRecord& hitRecord) const override;
	void Rotate(float dT) override;
private:
	Elite::FPoint3 m_Origin;
	std::vector<Elite::FPoint3> m_VertexBuffer;
	std::vector<IDList> m_IndexBuffer;
	Material* m_pMaterial;
	std::vector<Triangle*> m_pTriangles;
};