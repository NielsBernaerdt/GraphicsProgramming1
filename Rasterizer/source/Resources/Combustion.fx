float4x4 gWorldViewProj : WORLDVIEWPROJECTION;
Texture2D gDiffuseMap;

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Border;// or Mirror or Clamp or Border
	AddressV = Border;// or Mirror or Clamp or Border
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};
RasterizerState gRasterizerState
{
	CullMode = none;
	FrontCounterClockwise = true;
};
BlendState gBlendState
{
	BlendEnable[0] = true;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	BlendOp = add;
	SrcBlendAlpha = zero;
	DestBlendAlpha = zero;
	BlendOpAlpha = add;
	RenderTargetWritemask[0] = 0x0F;
};
DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = zero;
	DepthFunc = less;
	StencilEnable = false;

	StencilReadMask = 0x0F;
	StencilWriteMask = 0x0F;

	FrontFaceStencilFunc = always;
	BackFaceStencilFunc = always;

	FrontFaceStencilDepthFail = keep;
	BackFaceStencilDepthFail = keep;

	FrontFaceStencilPass = keep;
	BackFaceStencilPass = keep;

	FrontFaceStencilFail = keep;
	BackFaceStencilFail = keep;
};
struct VS_INPUT {
	float3 Position : POSITION;
	float3 Color : COLOR;
	float2 texCoord : TEXCOORD;
};
struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float3 Color : COLOR;
	float2 texCoord : TEXCOORD;
};
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input) {
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
	output.Color = input.Color;
	output.texCoord = input.texCoord;
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET{
	float4 diffuseColor = gDiffuseMap.Sample(samPoint,input.texCoord);
	float3 color_rgb = diffuseColor.rgb;
	float color_a = diffuseColor.a;
	return float4(color_rgb, color_a);
}
//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 DefaultTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}