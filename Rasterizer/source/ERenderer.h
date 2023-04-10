/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render too + DirectX initialization.
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include <cstdint>
#include <vector>
#include "Vertex.h"
#include "Camera.h"

struct SDL_Window;
struct SDL_Surface;
class Texture;

enum class CullMode
{
	noCulling,
	frontCulling,
	backCulling
};

namespace Elite
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow, Camera* pCamera);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void SwapRasterizer();
		void SwitchCullMode();
		void SwitchSamplingState();
		void Render();
		bool SaveBackbufferToImage() const;
		ID3D11Device* GetDevice() { return m_pDevice; }

	private:
		SDL_Window* m_pWindow;
		Camera* m_pCamera;
		uint32_t m_Width;
		uint32_t m_Height;
		bool m_UseSoftware{ true };
		CullMode m_CullMode{ CullMode::noCulling };
//HARDWARE
		bool m_IsInitialized;
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGIFactory* m_pDXGIFactory;
		IDXGISwapChain* m_pSwapChain;
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11Resource* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;
		HRESULT InitializeDirectX();
		void HardwareRender();
//SOFTWARE
		SDL_Surface* m_pFrontBuffer = nullptr;
		SDL_Surface* m_pBackBuffer = nullptr;
		uint32_t* m_pBackBufferPixels = nullptr;
		std::vector<float> depthBuffer;

		void SoftwareRender();
		void GetWorldToScreen(std::vector<Vertex>& transformedVertices
			, const std::vector<Vertex>& worldVertices
			, float width, float height, float fovAngle) const;
		Elite::RGBColor PixelShading(const Vertex& v, Texture* normalMap, Texture* specularMap);
	};
}

#endif