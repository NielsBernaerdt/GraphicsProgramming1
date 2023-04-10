#pragma once
class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~Effect();
	Effect(const Effect& other) = delete;
	Effect(Effect&& other) noexcept = delete;
	Effect& operator=(const Effect& other) = delete;
	Effect& operator=(Effect&& other) noexcept = delete;

	ID3DX11Effect* GetEffect() { return m_pEffect; }
	ID3DX11EffectTechnique* GetEffectTechnique() { return m_pTechnique; }
	ID3DX11EffectMatrixVariable* GetWVPMatrix() { return m_pMatWorldViewProjVariable; }
	ID3DX11EffectMatrixVariable* GetWorldMatrix() { return m_pMatWorld; }
	ID3DX11EffectMatrixVariable* GetViewInvMatrix() { return m_pMatViewInverse; }
	ID3DX11EffectShaderResourceVariable* GetDiffuseMap() { return m_pDiffuseMapVariable; }
	ID3DX11EffectShaderResourceVariable* GetNormalMap() { return m_pNormalMapvariable; }
	ID3DX11EffectShaderResourceVariable* GetGlossinessMap() { return m_pGlossinessMapVariable; }
	ID3DX11EffectShaderResourceVariable* GetSpecularMap() { return m_pSpecularMapVariable; }
private:
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable;
	ID3DX11EffectMatrixVariable* m_pMatWorld;
	ID3DX11EffectMatrixVariable* m_pMatViewInverse;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pNormalMapvariable;
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable;

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
};