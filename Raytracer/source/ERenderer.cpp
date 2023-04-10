//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "ERenderer.h"
#include "ERGBColor.h"
#include <iostream>
#include "Materials.h"
#include "SceneManager.h"

Elite::Renderer::Renderer(SDL_Window* pWindow, Camera* camera)
	: m_pCamera(camera)
{
	//Initialize
	m_pWindow = pWindow;
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	//FOV & ASPECT RATIO
	m_FOV = m_pCamera->GetFov();
	m_AspectRatio = float(m_Width / m_Height);

	//PIXEL WORLD POSITIONS
	for (uint32_t r = 0; r < m_Height; ++r)
	{
		for (uint32_t c = 0; c < m_Width; ++c)
		{
			m_PixelWorldPos.push_back({ (2 * (c + 0.5f) / m_Width - 1) * m_AspectRatio * m_FOV
								, (1 - 2 * (r + 0.5f) / m_Height) * m_FOV
								, -1.f
								, 1.f });
		}
	}
}

void Elite::Renderer::Render()
{
	SDL_LockSurface(m_pBackBuffer);

	//Loop over all the pixels
	for (uint32_t r = 0; r < m_Height; ++r)
	{
		for (uint32_t c = 0; c < m_Width; ++c)
		{
//---------------------------CALCULATE PIXEL HITS---------------------------//
			HitRecord pixelHit{};
			CalculatePixelHit(pixelHit, c, r);
//---------------------------RENDER EQUATION---------------------------//
			RGBColor finalColor{0.f, 0.f, 0.f};
			if (pixelHit.tValue > 0) //If an object has been hit
			{
				CalculatePixelColor(finalColor, pixelHit);
			}
			int i = c + (r * int(m_Width));
			finalColor.MaxToOne();
			m_pBackBufferPixels[i] = SDL_MapRGB(m_pBackBuffer->format, Uint8(finalColor.r * 255.f), Uint8(finalColor.g * 255.f), Uint8(finalColor.b * 255.f) );
		}
	}
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Elite::Renderer::CalculatePixelHit(HitRecord& hit, int c, int r)
{
	m_PixelRayOrigin = { (m_pCamera->GetViewToWorld() * m_PixelWorldPos[c + r * int(m_Width)]).xyz };
	m_PixelRayDirection = { GetNormalized(m_PixelRayOrigin - m_pCamera->GetPosition()) };
	Ray ray { m_PixelRayOrigin, m_PixelRayDirection };
	for (const auto& object : SceneManager::Get()->GetActiveScene()->GetObjects())
	{
		if (m_UseAcceleratedStructures == true
			&& object->GetBoundingBox()->Hit(ray) == false)
			continue;
		object->Hit(ray, m_TempCompareHit);
		if (hit.tValue < 0 || m_TempCompareHit.tValue <= 0 || m_TempCompareHit.tValue <= hit.tValue)
			hit = m_TempCompareHit;
	}
}
void Elite::Renderer::CalculatePixelColor(RGBColor& finalColor, const HitRecord& pixelHit)
{
	Elite::FVector3 lightToPointDir{};
	float lambert{};
	for (const auto& light : SceneManager::Get()->GetActiveScene()->GetLights()) //for all lights
	{
		lightToPointDir = light->GetDirection(pixelHit.hitPoint);
		if (m_CalculateShadows == true)
		{
			if(DoesLightReachTarget(lightToPointDir, pixelHit) == false)
				continue;
		}
		//if lambert > 0: calculate radiance
		lambert = Dot(pixelHit.normal, GetNormalized(lightToPointDir));
		if (lambert > 0)
		{
			Elite::RGBColor irradiance{ light->CalculateRadiance(pixelHit.hitPoint) };
			switch (m_ActiveLET)
			{
			case ActiveLightEquationTerms::both:
				finalColor += pixelHit.material->Shade(pixelHit, GetNormalized(lightToPointDir), m_pCamera->GetForward()) * lambert * irradiance;
				break;
			case ActiveLightEquationTerms::irradiance:
				finalColor += irradiance * lambert;
				break;
			case ActiveLightEquationTerms::brdf:
				finalColor += pixelHit.material->Shade(pixelHit, GetNormalized(lightToPointDir), m_pCamera->GetForward()) * lambert;
				break;
			}
		}
	}
}

bool Elite::Renderer::DoesLightReachTarget(const FVector3& lightToPointDir, const HitRecord& pixelHit)
{
	Ray pointToLightRay{};
	//Check if hitpoint sees light
	bool canSeeLight{ true };
	pointToLightRay = {
		/*origin*/ pixelHit.hitPoint,
		/*direction*/ GetNormalized(lightToPointDir),
		/*tMin*/ .01f,
		/*tMax*/ Magnitude(lightToPointDir)
	};
	for (const auto& obstacle : SceneManager::Get()->GetActiveScene()->GetObjects())
	{
		HitRecord lightHit{};
		if (obstacle->Hit(pointToLightRay, lightHit) == true)
		{
			canSeeLight = false;
			break;
		}
	}
	if (canSeeLight == false)
		return false;
	return true;
}

bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

void Elite::Renderer::ToggleShadows()
{ 
	m_CalculateShadows = !m_CalculateShadows;
	if(m_CalculateShadows)
		std::cout << "Shadows turned on.                             \tPress 'Q' to turn them off.\n";
	else
		std::cout << "Shadows turned off.                            \tPress 'Q' to turn them on.\n";
}
void Elite::Renderer::ToggleLightEquation() 
{
	m_ActiveLET = static_cast<ActiveLightEquationTerms>((static_cast<int>(m_ActiveLET) + 1) % m_NrLETModex);
	switch (m_ActiveLET)
	{
	case ActiveLightEquationTerms::both:
		std::cout << "Active light equation: Complete light equation \tPress 'E' to toggle again.\n";
		break;
	case ActiveLightEquationTerms::brdf:
		std::cout << "Active light equation: BRDF \t\t\tPress 'E' to toggle again.\n";
		break;
	case ActiveLightEquationTerms::irradiance:
		std::cout << "Active light equation: Irradiance \t\tPress 'E' to toggle again.\n";
		break;
	}
}
void Elite::Renderer::ToggleAcceleratedStructures() 
{ 
	m_UseAcceleratedStructures = !m_UseAcceleratedStructures;
	if (m_UseAcceleratedStructures)
		std::cout << "Accelerated Structures turned on.              \tPress 'C' to turn them off.\n";
	else
		std::cout << "Accelerated Structures turned off.             \tPress 'C' to turn them on.\n";
}