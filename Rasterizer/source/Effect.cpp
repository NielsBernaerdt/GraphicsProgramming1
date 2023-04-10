#include "pch.h"
#include "Effect.h"
#include <sstream>

Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
    m_pEffect = LoadEffect(pDevice, assetFile);

    m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
    if (!m_pTechnique->IsValid())
        std::wcout << L"Technique not valid\n";
//Matrices
    m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
    if (!m_pMatWorldViewProjVariable->IsValid())
        std::wcout << L"m_pMatWorldViewProjVariable not valid\n";

    m_pMatWorld = m_pEffect->GetVariableByName("gWorld")->AsMatrix();
    m_pMatViewInverse = m_pEffect->GetVariableByName("viewInverse")->AsMatrix();
//TextureMaps
    m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
    if (!m_pDiffuseMapVariable->IsValid())
        std::wcout << L"Variable gDiffuseMap not found\n";

    m_pNormalMapvariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
    m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
    m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
}

Effect::~Effect()
{
    m_pEffect->Release();
    //if(m_pMatWorldViewProjVariable->IsValid())m_pMatWorldViewProjVariable->Release();
    //m_pMatWorld->Release();
    //m_pMatViewInverse->Release();
    //m_pDiffuseMapVariable->Release();
    //m_pNormalMapvariable->Release();
    //m_pSpecularMapVariable->Release();
    //m_pGlossinessMapVariable->Release();
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
    HRESULT result = S_OK;
    ID3D10Blob* pErrorBlob = nullptr;
    ID3DX11Effect* pEffect;

    DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined (_DEBUG)
    shaderFlags |= D3DCOMPILE_DEBUG;
    shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    result = D3DX11CompileEffectFromFile(assetFile.c_str(),
        nullptr,
        nullptr,
        shaderFlags,
        0,
        pDevice,
        &pEffect,
        &pErrorBlob);
    if (FAILED(result))
    {
        if (pErrorBlob != nullptr)
        {
            char* pErrors = (char*)pErrorBlob->GetBufferPointer();

            std::wstringstream ss;
            for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); ++i)
                ss << pErrors[i];

            OutputDebugStringW(ss.str().c_str());
            pErrorBlob->Release();
            pErrorBlob = nullptr;

            std::wcout << ss.str() << std::endl;
        }
        else
        {
            std::wstringstream ss;
            ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
            std::wcout << ss.str() << std::endl;
            return nullptr;
        }
    }
    return pEffect;
}
