#include "pch.h"
#include "Camera.h"
#include <SDL.h>
#include <iostream>

Camera::Camera(const Elite::FPoint3& position, const Elite::FVector3& viewForward, float fovAngle, float aspectRatio, float nearZ, float farZ) :
	m_Fov(tanf(fovAngle* float(E_TO_RADIANS) / 2.f)),
	m_Position{ position },
	m_ViewForward{ GetNormalized(-viewForward) },
	m_NearPlane(nearZ),
	m_FarPlane(farZ)
{
	CalculateMatrices();
	//use left handed version
	m_Projection = Elite::FMatrix4{
	1 / (aspectRatio * m_Fov),		0,				0,							0,
	0,								1 / m_Fov,		0,							0,
	0,								0,				farZ / (farZ - nearZ),		-(farZ * nearZ) / (farZ - nearZ),
	0,								0,				1,							0 };
}

void Camera::SetAspectRatio(float aspectRatio)
{
	//use left handed version
	m_Projection = Elite::FMatrix4{
	1 / (aspectRatio * m_Fov),		0,				0,												0,
	0,								1 / m_Fov,		0,												0,
	0,								0,				m_FarPlane / (m_FarPlane - m_NearPlane),		-(m_FarPlane * m_NearPlane) / (m_FarPlane - m_NearPlane),
	0,								0,				1,												0 };
}

void Camera::Update(float elapsedSec)
{
	//Keyboard Input
	const uint8_t* pKeyboardState = SDL_GetKeyboardState(0);
	float finalKeyboardSpeed = m_KeyboardSpeed;
	if (pKeyboardState[SDL_SCANCODE_LSHIFT])
		finalKeyboardSpeed *= m_SpeedMultiplier;
	m_RelativeTranslation.x = (pKeyboardState[SDL_SCANCODE_D] - pKeyboardState[SDL_SCANCODE_A]) * finalKeyboardSpeed * elapsedSec;
	m_RelativeTranslation.y = 0;
	m_RelativeTranslation.z = (pKeyboardState[SDL_SCANCODE_S] - pKeyboardState[SDL_SCANCODE_W]) * finalKeyboardSpeed * elapsedSec;
	if (!m_IsUsingSoftware)
		m_RelativeTranslation.z = -m_RelativeTranslation.z;
	//Mouse Input
	int x, y = 0;
	uint32_t mouseState = SDL_GetRelativeMouseState(&x, &y);
	if (mouseState == SDL_BUTTON_LMASK)
	{
		m_RelativeTranslation.z += y * m_MouseSpeed * elapsedSec;
		m_AbsoluteRotation.y -= x * m_RotationSpeed;
	}
	else if (mouseState == SDL_BUTTON_RMASK)
	{
		m_AbsoluteRotation.x -= y * m_RotationSpeed;
		m_AbsoluteRotation.y -= x * m_RotationSpeed;
	}
	else if (mouseState == (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK))
	{
		m_RelativeTranslation.y -= y * m_MouseSpeed * elapsedSec;
	}
	CalculateMatrices();
}

void Camera::CalculateMatrices()
{
	//FORWARD // YAW
	Elite::FMatrix3 yawRotation = Elite::MakeRotationY(m_AbsoluteRotation.y * float(E_TO_RADIANS));
	Elite::FVector3 zAxis = yawRotation * m_ViewForward;
	if (!m_IsUsingSoftware)
	{
		zAxis.x = -zAxis.x;//flip because of DirectX left handed coordinate system
		zAxis.z = -zAxis.z;//flip because of DirectX left handed coordinate system
	}
	m_Forward = zAxis;
	//RIGHT
	Elite::FVector3 xAxis = GetNormalized(Cross(Elite::FVector3{ 0.f,1.f,0.f }, zAxis));
	//FORWARD // PITCH
	Elite::FMatrix3 pitchRotation = MakeRotation(m_AbsoluteRotation.x * float(E_TO_RADIANS), xAxis);
	zAxis = pitchRotation * zAxis;
	//UP
	Elite::FVector3 yAxis = Cross(zAxis, xAxis);

	m_Position += m_RelativeTranslation.x * xAxis;
	m_Position += m_RelativeTranslation.y * yAxis;
	m_Position += m_RelativeTranslation.z * zAxis;
	m_ViewToWorld =
	{
		Elite::FVector4{xAxis},
		Elite::FVector4{yAxis},
		Elite::FVector4{zAxis},
		Elite::FVector4{m_Position.x,m_Position.y,m_Position.z,1.f}
	};
	m_WorldToView = Inverse(m_ViewToWorld);
}