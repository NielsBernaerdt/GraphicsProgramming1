#pragma once
#include "EMath.h"

class Camera
{
public:
	Camera(const Elite::FPoint3& position = { 0.f, 0.f, 50.f }
		, const Elite::FVector3& viewForward = { 0.f, 0.f, -1.f }
		, float fovAngle = 60.f
		, float aspectRatio = 1.f
		, float nearZ = 0.1f
		, float farZ = 100.f);
	~Camera() = default;

	Camera(const Camera&) = delete;
	Camera(Camera&&) noexcept = delete;
	Camera& operator=(const Camera&) = delete;
	Camera& operator=(Camera&&) noexcept = delete;

	void Update(float elapsedSec);


	Elite::FPoint3 GetPosition() { return m_Position; }
	Elite::FVector3 GetForward() { return m_Forward; }
	const float GetFov() const { return m_Fov; }
	const Elite::FMatrix4& GetWorldToView() const { return m_WorldToView; }
	const Elite::FMatrix4& GetViewToWorld() const { return m_ViewToWorld; }

	float GetNearClippingPlane() { return m_NearPlane; }
	float GetFarClippingPlane() { return m_FarPlane; }


	//////////
	void SetAspectRatio(float aspectRatio);
	const Elite::FMatrix4& GetProjectionmatrix() const { return m_Projection; }
	void SetCurrentRasterizer(bool isSoftware) { m_IsUsingSoftware = isSoftware; }

private:
	float m_Fov{};

	const float m_KeyboardSpeed{ 2.f };
	const float m_SpeedMultiplier{ 10.f };
	const float m_RotationSpeed{ .5f };
	const float m_MouseSpeed{ 1.f };

	Elite::FMatrix4 m_WorldToView{};
	Elite::FMatrix4 m_ViewToWorld{};
	Elite::FPoint2 m_AbsoluteRotation{}; //Pitch(x) & Yaw(y) only
	Elite::FPoint3 m_RelativeTranslation{};
	Elite::FPoint3 m_Position{};
	const Elite::FVector3 m_ViewForward{};
	Elite::FVector3 m_Forward{};
	float m_NearPlane{};
	float m_FarPlane{};

	void CalculateMatrices();


	////
	bool m_IsUsingSoftware{true};
	Elite::FMatrix4 m_Projection{};
};