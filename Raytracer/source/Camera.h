#pragma once
#include "EMath.h"

class Camera
{
public:

	Camera(const Elite::FPoint3& position = { 0.f, 3.f, 9.f }, const Elite::FVector3& viewForward = { 0.f, 0.f, -1.f }, float fovAngle = 45.f);
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


private:
	float m_Fov{};

	const float m_KeyboardSpeed{ 2.f };
	const float m_SpeedMultiplier{ 10.f };
	const float m_RotationSpeed{ .1f };
	const float m_MouseSpeed{ 1.f };

	Elite::FMatrix4 m_WorldToView{};
	Elite::FMatrix4 m_ViewToWorld{};
	Elite::FPoint2 m_AbsoluteRotation{}; //Pitch(x) & Yaw(y) only
	Elite::FPoint3 m_RelativeTranslation{};
	Elite::FPoint3 m_Position{};
	const Elite::FVector3 m_ViewForward{};
	Elite::FVector3 m_Forward{};

	void CalculateMatrices();
};