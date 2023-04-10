#include "pch.h"

//Project includes
#include "ERenderer.h"
#include "SceneManager.h"
#include "Texture.h"
#include "LightManager.h"

Elite::Renderer::Renderer(SDL_Window * pWindow, Camera* pCamera)
	: m_pWindow{ pWindow }
	, m_Width{}
	, m_Height{}
	, m_IsInitialized{ false }
	, m_pCamera{pCamera}
{
	m_pWindow = pWindow;
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;
	//fill depth buffer
	for (uint32_t i{}; i < m_Width * m_Height; ++i)
	{
		depthBuffer.push_back(FLT_MAX);
	}

	//Initialize DirectX pipeline
	InitializeDirectX();
	m_IsInitialized = true;
	//Set Camera Params
	m_pCamera->SetAspectRatio(float(m_Width / m_Height));
}

Elite::Renderer::~Renderer()
{
	m_pRenderTargetView->Release();
	m_pRenderTargetBuffer->Release();
	m_pDepthStencilView->Release();
	m_pDepthStencilBuffer->Release();
	m_pSwapChain->Release();
	if (m_pDeviceContext)
	{
		m_pDeviceContext->ClearState();
		m_pDeviceContext->Flush();
		m_pDeviceContext->Release();
	}
	m_pDevice->Release();
	m_pDXGIFactory->Release();
}

void Elite::Renderer::SwapRasterizer()
{ 
	m_UseSoftware = !m_UseSoftware;
	m_pCamera->SetCurrentRasterizer(m_UseSoftware); 
	if (m_UseSoftware)
		std::cout << "Software Rasterizer active. \t\tPress 'E' to switch to DirectX.\n";
	else
		std::cout << "DirectX active.             \t\tPress 'E' to switch to use the software rasterizer.\n";
}
void Elite::Renderer::SwitchCullMode()
{
	int nrCullModes{ 3 };
	m_CullMode = static_cast<CullMode>((static_cast<int>(m_CullMode) + 1) % nrCullModes);
	switch (m_CullMode)
	{
	case CullMode::noCulling:
		std::cout << "Active Cull Mode: no culling    \tPress 'C' to toggle again.\n";
		break;
	case CullMode::frontCulling:
		std::cout << "Active Cull Mode: front culling \tPress 'C' to toggle again.\n";
		break;
	case CullMode::backCulling:
		std::cout << "Active Cull Mode: back culling  \tPress 'C' to toggle again.\n";
		break;
	}
}

void Elite::Renderer::Render()
{
	if (!m_IsInitialized)
		return;
	if (m_UseSoftware)
	{
		SoftwareRender();
	}
	else
	{
		HardwareRender();
	}
}
HRESULT Elite::Renderer::InitializeDirectX()
{
	//Create Device and Device context, using hardware aceleration
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif
	HRESULT result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_pDeviceContext);
	if (FAILED(result))
		return result;
	//Create DXGI Factory to create SwapChain based on hardware
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_pDXGIFactory));
	if (FAILED(result))
		return result;
	//Create SwapChain Descriptor
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferDesc.Width = m_Width;
	swapChainDesc.BufferDesc.Height = m_Height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;
	//Get the handle HWND from the SDL backbuffer
	SDL_SysWMinfo sysWMInfo{};
	SDL_VERSION(&sysWMInfo.version);
	SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
	swapChainDesc.OutputWindow = sysWMInfo.info.win.window;
	//Create SwapChain and hook it into the handle of the SDL window
	result = m_pDXGIFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
	if (FAILED(result))
		return result;
	//Create the Depth/Stencil Buffer and View
	D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width = m_Width;
	depthStencilDesc.Height = m_Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = depthStencilDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	result = m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilBuffer);
	if (FAILED(result))
		return result;
	result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
	if (FAILED(result))
		return result;
	//Create the RenderTargetView
	result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
	if (FAILED(result))
		return result;
	result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, 0, &m_pRenderTargetView);
	if (FAILED(result))
		return result;
	//Bind the Views to the Output Merger Stage
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	//Set the Viewport
	D3D11_VIEWPORT viewPort{};
	viewPort.Width = static_cast<float>(m_Width);
	viewPort.Height = static_cast<float>(m_Height);
	viewPort.TopLeftX = 0.f;
	viewPort.TopLeftY = 0.f;
	viewPort.MinDepth = 0.f;
	viewPort.MaxDepth = 1.f;
	m_pDeviceContext->RSSetViewports(1, &viewPort);

	return result;
}

void Elite::Renderer::HardwareRender()
{
	//Clear Buffers
	RGBColor clearColor{ 0.1f, 0.1f, 0.1f };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	//Render
	for (const auto& mesh : SceneManager::Get()->GetActiveScene()->GetObjects())
	{
		if (mesh->IsActive() == false)
			continue;
		mesh->Render(m_pDeviceContext, m_pCamera);
	}

	//Present
	m_pSwapChain->Present(0, 0);
}

void Elite::Renderer::SoftwareRender()
{
	SDL_LockSurface(m_pBackBuffer);

	for (size_t i{}; i < m_pBackBuffer->w * m_pBackBuffer->h; ++i)
	{
		m_pBackBufferPixels[i] = SDL_MapRGB(m_pBackBuffer->format,
			static_cast<uint8_t>(25),
			static_cast<uint8_t>(25),
			static_cast<uint8_t>(25));
		depthBuffer[i] = FLT_MAX;
	}
	std::vector<Triangle*> pTriangles;
	for (auto object : SceneManager::Get()->GetActiveScene()->GetObjects())
	{
		if (object->IsActive() == false)
			continue;
		for (auto triangle : object->GetTriangles())
		{
			//SCREENSPACE CONVERSION//
			std::vector<Vertex> worldVertices = triangle->GetVertices();
			std::vector<Vertex> ssVertices;
			GetWorldToScreen(ssVertices
				, worldVertices
				, float(m_Width)
				, float(m_Height)
				, m_pCamera->GetFov());
			//BOUNDING BOX//
			FPoint2 topleft{ float(m_Width), float(m_Height) };
			FPoint2 bottomright{ 0, 0 };
			for (auto& v : ssVertices)
			{
				if (v.position.x <= topleft.x && v.position.x >= 0)
					topleft.x = v.position.x;
				if (v.position.y <= topleft.y && v.position.y >= 0)
					topleft.y = v.position.y;
				if (v.position.x >= bottomright.x && v.position.x <= m_Width)
					bottomright.x = v.position.x;
				if (v.position.y >= bottomright.y && v.position.y <= m_Height)
					bottomright.y = v.position.y;
			}
			//PIXEL LOOP
			for (uint32_t r = uint32_t(topleft.y); r < bottomright.y; ++r)
			{
				for (uint32_t c = uint32_t(topleft.x); c < bottomright.x; ++c)
				{
					FPoint2 pixelPoint{ (float)c, (float)r };
					bool pixelInTriangle{ true };
					//LITTLE OPTIMIZATION BY STORING INSTEAD OF ACCESSING USING []
					Elite::FPoint4 ssV0Pos{ ssVertices[0].position };
					Elite::FPoint4 ssV1Pos{ ssVertices[1].position };
					Elite::FPoint4 ssV2Pos{ ssVertices[2].position };
					//
					// Check 1
					FVector2 edgeA = ssV1Pos.xy - ssV0Pos.xy;
					FVector2 pointToSideA = pixelPoint - ssV0Pos.xy;
					float crossA{ Elite::Cross(edgeA, pointToSideA) };
					float Weight2{};
					if (crossA > 0)
						pixelInTriangle = false;
					else
						Weight2 = crossA / Elite::Cross(ssV0Pos.xy - ssV1Pos.xy
							, ssV0Pos.xy - ssV2Pos.xy);
					// Check 2
					FVector2 edgeB = ssVertices[2].position.xy - ssVertices[1].position.xy;
					FVector2 pointToSideB = pixelPoint - ssVertices[1].position.xy;
					float crossB{ Elite::Cross(edgeB, pointToSideB) };
					float Weight0{};
					if (crossB > 0)
						pixelInTriangle = false;
					else
						Weight0 = crossB / Elite::Cross(ssV0Pos.xy - ssV1Pos.xy
							, ssV0Pos.xy - ssV2Pos.xy);
					// Check 3
					FVector2 edgeC = ssV0Pos.xy - ssV2Pos.xy;
					FVector2 pointToSideC = pixelPoint - ssV2Pos.xy;
					float crossC{ Elite::Cross(edgeC, pointToSideC) };
					float Weight1{};
					if (crossC > 0)
						pixelInTriangle = false;
					else
						Weight1 = crossC / Elite::Cross(ssV0Pos.xy - ssV1Pos.xy
							, ssV0Pos.xy - ssV2Pos.xy);
					if (pixelInTriangle)
					{
						//used for interpolation
						float linearDepth{ 1 / ((Weight0 / ssV0Pos.w)
										+ (Weight1 / ssV1Pos.w)
										+ (Weight2 / ssV2Pos.w)) };
						Elite::FVector3 interpolatedTangent{ (ssVertices[0].tangent / ssV0Pos.w) * Weight0
							+ (ssVertices[1].tangent / ssV1Pos.w) * Weight1
							+ (ssVertices[2].tangent / ssV2Pos.w) * Weight2 };
						Elite::FVector3 interpolatedNormal{ (ssVertices[0].normal / ssV0Pos.w) * Weight0
							+ (ssVertices[1].normal / ssV1Pos.w) * Weight1
							+ (ssVertices[2].normal / ssV2Pos.w) * Weight2 };
						Elite::FVector2 interpolatedUV{ (ssVertices[0].uv / ssV0Pos.w) * Weight0
												+ (ssVertices[1].uv / ssV1Pos.w) * Weight1
												+ (ssVertices[2].uv / ssV2Pos.w) * Weight2 };
						interpolatedUV *= linearDepth;
						RGBColor finalColor{ object->GetDiffuseMap()->Sample(interpolatedUV)};

						//only used for comparing depth
						float bufferDepth{ 1 / ((Weight0 / ssV0Pos.z)
										+ (Weight1 / ssV1Pos.z)
										+ (Weight2 / ssV2Pos.z)) };
						Remap(bufferDepth, 0.985f, 1.f);

						if ((bufferDepth >= 0 && bufferDepth <= 1.f)
							&& bufferDepth <= depthBuffer[c + (r * m_Width)])
						{
							depthBuffer[c + (r * m_Width)] = bufferDepth;

							finalColor = PixelShading({ {}, finalColor, interpolatedUV, interpolatedNormal, interpolatedTangent }, object->GetNormalMap(), object->GetSpecularMap());
							finalColor.MaxToOne();
							m_pBackBufferPixels[c + (r * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
								static_cast<uint8_t>(finalColor.r * 255),
								static_cast<uint8_t>(finalColor.g * 255),
								static_cast<uint8_t>(finalColor.b * 255));
						}
					}
				}
			}
		}
	}

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

void Elite::Renderer::GetWorldToScreen(std::vector<Vertex>& transformedVertices, const std::vector<Vertex>& worldVertices, float width, float height, float fov) const
{
	//WORLD MATRIX// -> in this case the points are already defined in world space
	std::vector<Elite::FPoint4> temp;
	for (const auto& ov : worldVertices)
	{
		temp.push_back({ ov.position.x, ov.position.y, ov.position.z, 1 });
	}
	//VIEW MATRIX//	
	auto viewMatrix{ m_pCamera->GetWorldToView() };
	//PROJECTION MATRIX//
	float aspectRatio = width / height;
	float farPlane{ m_pCamera->GetFarClippingPlane() };
	float nearPlane{ m_pCamera->GetNearClippingPlane() };
	Elite::FMatrix4 projectionmatrix{
	1 / (aspectRatio * fov),		0,				0,										0,
	0,								1 / fov,		0,										0,
	0,								0,				-farPlane / (farPlane - nearPlane),		-(farPlane * nearPlane) / (farPlane - nearPlane),
	0,								0,				-1,										0 };
	//WorldViewProjectionMatrix//
	Elite::FMatrix4 worldViewProjectionMatrix{ projectionmatrix * viewMatrix /** worldMatrix*/ };
	//Apply matrix & perspective divide
	std::vector<Elite::FPoint4> ndc;
	for (const auto& v : temp)
	{
		auto test{ worldViewProjectionMatrix * v };
		ndc.push_back(Elite::FPoint4{ test.x / test.w, test.y / test.w, test.z / test.w, test.w });
	}
	//transform NDC -> ScreenSpace
	int i{};
	for (const auto& n : ndc)
	{
		transformedVertices.push_back(Vertex{ Elite::FPoint4{ (n.x + 1) / 2 * width, (1 - n.y) / 2 * height, n.z, n.w}
										, worldVertices[i].color
										, worldVertices[i].uv
										, worldVertices[i].normal
										, worldVertices[i].tangent });
		++i;
	}
}

Elite::RGBColor Elite::Renderer::PixelShading(const Vertex& v, Texture* normalMap, Texture* specularMap)
{
	RGBColor lightColor = LightManager::Get()->GetLights()[0]->GetColor();
	float intensity = 80;
	FVector3 lightDirection = LightManager::Get()->GetLights()[0]->GetDirection({});
	FVector3 viewDir{ m_pCamera->GetForward() };
	Elite::RGBColor diffuse{ v.color };
	float gShininess{ 15 };
	//Calculate New Normal
	Elite::FVector3 newNormal;
	if (normalMap)
	{
		Elite::FVector3 binormal{ Elite::Cross(v.tangent, v.normal) };
		Elite::FMatrix3 tangentSpaceAxis{ v.tangent, binormal, v.normal };
		Elite::RGBColor sampledNormalMap{ normalMap->Sample(v.uv) };
		Elite::FVector3 normalmapNormal{ (2.f * (sampledNormalMap.r)) - 1.f
										, (2.f * (sampledNormalMap.g)) - 1.f
										, (2.f * (sampledNormalMap.b)) - 1.f };
		newNormal = tangentSpaceAxis * normalmapNormal;
	}
	else
		newNormal = v.normal;
	//Calculate Specular
	Elite::RGBColor phong{};
	if (specularMap)
	{
		Elite::FVector3 reflectDir = lightDirection - 2 * v.normal * Elite::Dot(lightDirection, v.normal);
		auto test = Elite::Dot(reflectDir, -viewDir);
		float specAngle = std::max(test, 0.f);
		float temp{ std::pow(specAngle, gShininess / 4.f) };
		const float cosA = Dot((lightDirection - (2 * Elite::Dot(v.normal, lightDirection)) * v.normal), viewDir);
		auto phongRefelction = specularMap->Sample(v.uv) * temp;
		phong = phongRefelction;
	}
	//PIXELSHADING	
	auto lambertsCosineLaw{ Elite::Dot(-newNormal, lightDirection) };
	if (lambertsCosineLaw < 0.f)
		return { 0,0,0 };
	else
		return lightColor * intensity * (diffuse + phong) * lambertsCosineLaw;
}