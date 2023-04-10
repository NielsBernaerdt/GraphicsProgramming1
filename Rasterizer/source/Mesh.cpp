#include "pch.h"
#include "Mesh.h"
#include "Effect.h"
#include "Texture.h"
#include "Camera.h"

TriangleMesh::TriangleMesh(ID3D11Device* pDevice, std::vector<Vertex> vertices, std::vector<uint32_t> indices, const std::wstring& effectFile, const std::string& textureFile, bool isActive)
{
//Software Rasterizer
	size_t nrTriangles{ vertices.size() };
	nrTriangles -= nrTriangles % 3;
	for (size_t i{}; i < nrTriangles; i += 3)
	{
		m_pTriangles.push_back(new Triangle{ vertices[i], vertices[i + 2], vertices[i + 1] });
	}
	m_IsActive = isActive;
//Hardware Rasterizer
	m_pEffect = new Effect(pDevice, effectFile);
	m_pDiffuseMap = new Texture(textureFile, pDevice);
	//Create Vertex Layout
	HRESULT result = S_OK;
	static const uint32_t numElements{ 5 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "TEXCOORD";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 28;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "NORMAL";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 36;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[4].SemanticName = "TANGENT";
	vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[4].AlignedByteOffset = 48;
	vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;


	//Create Vertex Buffer
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * (uint32_t)vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData{ 0 };
	initData.pSysMem = vertices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		return;
	//Create Input Layout
	D3DX11_PASS_DESC passDesc;
	m_pEffect->GetEffectTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
	result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pVertexLayout);
	if (FAILED(result))
		return;
	//Create Index Buffer
	m_AmountIndices = (uint32_t)indices.size();
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_AmountIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
		return;
	m_pDevice = pDevice;
}

TriangleMesh::~TriangleMesh()
{
	delete m_pEffect;
	delete m_pDiffuseMap;
	m_pVertexBuffer->Release();
	m_pVertexLayout->Release();
	m_pIndexBuffer->Release();

	for (auto t : m_pTriangles)
	{
		delete t;
	}
	delete m_pNormalMap;
	delete m_pSpecularMap;
	delete m_pGlossinessMap;
}

void TriangleMesh::Render(ID3D11DeviceContext* pDeviceContext, Camera* pCamera)
{
	//Set Vertex Buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//Set Index Buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set Input Layout
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	//Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//Set Matrices	
	//Set WorldViewProjectionMatrix
	Elite::FMatrix4 worldViewProjectionMatrix{ pCamera->GetProjectionmatrix() * pCamera->GetWorldToView() * m_World};
	m_pEffect->GetWVPMatrix()->SetMatrix(reinterpret_cast<float*>(&worldViewProjectionMatrix));

	//Set WorldMatrix
	m_pEffect->GetWorldMatrix()->SetMatrix(reinterpret_cast<float*>(&m_World));

	//Set ViewInverse
	m_pEffect->GetWorldMatrix()->SetMatrix(reinterpret_cast<float*>(&m_World));
//Set Textures
	//Set Diffuse Map
	if(m_pEffect->GetDiffuseMap())
		m_pEffect->GetDiffuseMap()->SetResource(m_pDiffuseMap->GetShaderResourceView());

	//Set Normal Map
	if(m_pEffect->GetNormalMap() && m_pNormalMap)
	m_pEffect->GetNormalMap()->SetResource(m_pNormalMap->GetShaderResourceView());

	//Set Specular Map
	if(m_pEffect->GetSpecularMap() && m_pSpecularMap)
	m_pEffect->GetSpecularMap()->SetResource(m_pSpecularMap->GetShaderResourceView());

	//Set Normal Map
	if(m_pEffect->GetGlossinessMap() && m_pGlossinessMap)
	m_pEffect->GetGlossinessMap()->SetResource(m_pGlossinessMap->GetShaderResourceView());

	//Render a Triangle
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pEffect->GetEffectTechnique()->GetDesc(&techDesc);
	for(UINT p{0}; p < techDesc.Passes; ++p)
	{
		m_pEffect->GetEffectTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_AmountIndices, 0, 0);
	}
}

void TriangleMesh::SetNormalMap(const std::string& assetFile)
{
	if (m_pNormalMap)
	{
		delete m_pNormalMap;
		m_pNormalMap = nullptr;
	}
	m_pNormalMap = new Texture(assetFile, m_pDevice);
}

void TriangleMesh::SetSpecularMap(const std::string& assetFile)
{
	if (m_pSpecularMap)
	{
		delete m_pSpecularMap;
		m_pSpecularMap = nullptr;
	}
	m_pSpecularMap = new Texture(assetFile, m_pDevice);
}

void TriangleMesh::SetGlossinessMap(const std::string& assetFile)
{
	if (m_pGlossinessMap)
	{
		delete m_pGlossinessMap;
		m_pGlossinessMap = nullptr;
	}
	m_pGlossinessMap = new Texture(assetFile, m_pDevice);
}

void TriangleMesh::Update(float deltaT)
{
	float angle{ 45 };
	angle *= float(E_TO_RADIANS);
/////////////////////
	m_World = { 1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1 };

	RotateHardware(deltaT, angle);
	RotateSoftware(deltaT, angle);
}

void TriangleMesh::ToggleDisability()
{
	m_IsActive = !m_IsActive;
	if (m_IsActive)
		std::cout << "FireFX mesh active.  \t\t\tPress 'T' to disable this mesh.\n";
	else
		std::cout << "FireFX mesh disabled.\t\t\tPress 'T' to activate this mesh.\n";
}

int TriangleMesh::SwitchSamplingState()
{
	int nrSamplingStates{ 3 };
	m_Samplingstate = static_cast<SamplingState>((static_cast<int>(m_Samplingstate) + 1) % nrSamplingStates);
	return static_cast<int>(m_Samplingstate);
}

void TriangleMesh::RotateSoftware(float deltaT, float angle)
{
	float rotAngle{ deltaT * angle };

	Elite::FMatrix4 translateBack{
	1,	0,	0,	m_Origin.x,
	0,	1,	0,	m_Origin.y,
	0,	0,	1,	m_Origin.z,
	0,	0,	0,	1 };
	Elite::FMatrix4 translateToOrigin{
	1,	0,	0,	-m_Origin.x,
	0,	1,	0,	-m_Origin.y,
	0,	0,	1,	-m_Origin.z,
	0,	0,	0,	1 };
	Elite::FMatrix4 rotMatrix{
	cos(rotAngle),		0,				sin(rotAngle),		0,
	0,					1,				0,					0,
	-sin(rotAngle),	0,					cos(rotAngle),		0,
	0,					0,				0,					1 };
	Elite::FPoint4 p;
	for (auto& t : m_pTriangles)
	{
		for (auto& v : t->GetVertices())
		{
			p = { v.position.x, v.position.y, v.position.z, 1 };
			v.position = Elite::FPoint4{ (translateBack * rotMatrix * translateToOrigin * p).xyz, v.position.w };
			v.normal = static_cast<Elite::FVector3>((translateBack * rotMatrix * translateToOrigin * static_cast<Elite::FPoint4>(v.normal)).xyz);
		}
	}
}
void TriangleMesh::RotateHardware(float deltaT, float angle)
{
	accAngle += deltaT * -angle;


	Elite::FMatrix4 translateBack{
	1,	0,	0,	m_Origin.x,
	0,	1,	0,	m_Origin.y,
	0,	0,	1,	m_Origin.z,
	0,	0,	0,	1 };
	Elite::FMatrix4 translateToOrigin{
	1,	0,	0,	-m_Origin.x,
	0,	1,	0,	-m_Origin.y,
	0,	0,	1,	-m_Origin.z,
	0,	0,	0,	1 };
	Elite::FMatrix4 rotMatrix{
	cos(accAngle),		0,				sin(accAngle),		0,
	0,					1,				0,					0,
	-sin(accAngle),	0,					cos(accAngle),		0,
	0,					0,				0,					1 };

	m_World = translateBack * rotMatrix * translateToOrigin;
}