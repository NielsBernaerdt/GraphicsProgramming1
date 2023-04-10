#include "pch.h"
#include "Texture.h"

Texture::Texture(std::string filePath, ID3D11Device* pDevice)
{
	m_pSurface = IMG_Load(filePath.c_str());
	//Create Texture2D
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = m_pSurface->w;
	desc.Height = m_pSurface->h;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = m_pSurface->pixels;
	initData.SysMemPitch = static_cast<UINT>(m_pSurface->pitch);
	initData.SysMemSlicePitch = static_cast<UINT>(m_pSurface->h * m_pSurface->pitch);

	HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &m_pTexture);
	//Create ShaderResourceView
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pTextureResourceView);
}
Texture::~Texture()
{
	SDL_FreeSurface(m_pSurface);

	m_pTexture->Release();
	m_pTextureResourceView->Release();
}

Elite::RGBColor Texture::Sample(const Elite::FVector2& uv) {
	if (uv.x >= 0 && uv.x <= 1 && uv.y <= 1 && uv.y >= 0)
	{
		auto texturePixels = (uint32_t*)m_pSurface->pixels;
		Uint8 r{}, g{}, b{};

		int colum = int(uv.x * m_pSurface->w);
		int row = int(uv.y * m_pSurface->h);
		int index = int(colum) + int(row) * m_pSurface->w;
		SDL_GetRGB(texturePixels[index], m_pSurface->format, &r, &g, &b);

		return Elite::RGBColor{ r / 255.f, g / 255.f, b / 255.f };
	}
	return Elite::RGBColor{ 0, 0, 0 };
}