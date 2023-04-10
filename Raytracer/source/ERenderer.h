/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render to, does traverse the pixels 
// and traces the rays using a tracer
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include <cstdint>
#include <vector>
#include <iostream>

#include "Camera.h"
#include "Ray.h"

struct SDL_Window;
struct SDL_Surface;

enum class ActiveLightEquationTerms
{
	both = 0,
	irradiance = 1,
	brdf = 2
};

namespace Elite
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow, Camera* camera);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render();
		bool SaveBackbufferToImage() const;
		void ToggleShadows();
		void ToggleLightEquation();
		void ToggleAcceleratedStructures();

	private:
		SDL_Window* m_pWindow = nullptr;
		SDL_Surface* m_pFrontBuffer = nullptr;
		SDL_Surface* m_pBackBuffer = nullptr;
		uint32_t* m_pBackBufferPixels = nullptr;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		Camera* m_pCamera;
		float m_FOV{};
		float m_AspectRatio{};
		bool m_CalculateShadows{ true };
		bool m_UseAcceleratedStructures{ false };
		const int m_NrLETModex{ 3 };
		ActiveLightEquationTerms m_ActiveLET{ ActiveLightEquationTerms::both };
		//Small Optimizations by not creating these objects every pixel.
		std::vector<FPoint4> m_PixelWorldPos;
		Elite::FPoint3 m_PixelRayOrigin{};
		Elite::FVector3 m_PixelRayDirection{};
		HitRecord m_TempCompareHit;

		void CalculatePixelHit(HitRecord& hit, int column, int row);
		void CalculatePixelColor(RGBColor& color, const HitRecord& hit);
		bool DoesLightReachTarget(const FVector3& lightDir, const HitRecord& hit);
	};
}
#endif