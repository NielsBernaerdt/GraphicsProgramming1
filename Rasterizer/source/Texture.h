#pragma once
#include <SDL_image.h>
#include <SDL_surface.h>
#include <string>
#include "EMath.h"
#include "ERGBColor.h"

class Texture
{
public:
	Texture(std::string filePath, ID3D11Device* pDevice);
	~Texture();

	Elite::RGBColor Sample(const Elite::FVector2& uv);
	ID3D11ShaderResourceView* GetShaderResourceView() { return m_pTextureResourceView; }
private:
	SDL_Surface* m_pSurface;
	ID3D11Texture2D* m_pTexture;
	ID3D11ShaderResourceView* m_pTextureResourceView;
};