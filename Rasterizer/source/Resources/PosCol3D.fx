float4x4 gWorldViewProj : WORLDVIEWPROJECTION;
float4x4 gWorld : WORLD;
float4x4 viewInverse : VIEWINVERSE;

Texture2D gDiffuseMap;
Texture2D gNormalMap;
Texture2D gSpecularMap;
Texture2D gGlossinessMap;

float3 gLightDirection = float3(0.577f, -0.577f, 0.577f);
float gPI = 3.141592f;
float gLightIntensity = 7.0f;
float gShininess = 25.0f;

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Border;// or Mirror or Clamp or Border
	AddressV = Border;// or Mirror or Clamp or Border
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Border;
	AddressV = Border;
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};
SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	AddressU = Border;
	AddressV = Border;
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};
RasterizerState gRasterizerState
{
	CullMode = BACK;
	FrontCounterClockwise = true;
};
BlendState EnableBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};
DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};
struct VS_INPUT {
	float3 Position : POSITION;
	float3 Color : COLOR;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};
struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float3 Color : COLOR;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input) {
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
	output.Color = input.Color;
	output.TexCoord = input.TexCoord;
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET{
	//float3 nSample = (float3)gNormalMap.Sample(samPoint, input.TexCoord);
	//nSample = (2 * nSample) - float3(1.0f, 1.0f, 1.0f);
	//float3 normal = normalize(mul(nSample, float3x3(input.Tangent, cross(input.Normal, input.Tangent), input.Normal)));

	////hardcode directional light
	//float3 lightColor = { 1,1,1 };
	//float3 finalColor = {0,0,0};


	//float3 specular = gSpecularMap.Sample(samPoint, input.TexCoord).xyz;

	//float glossiness = gGlossinessMap.Sample(samPoint, input.TexCoord).r;

	//float obersedArea = dot(normal, gLightDirection);
	//obersedArea = 1 - obersedArea;
	//float3 x = reflect(-gLightDirection, normal);
	//float3 viewDirection = normalize(input.Position - viewInverse[3].xyz).xyz;

	//const float cosA = dot((gLightDirection - (2 * dot(normal, gLightDirection)) * normal), viewDirection);
	//float phongRefelction = pow(cosA, gShininess + glossiness);
	//float3 phongTotal = specular * phongRefelction;

	float4 diffuse;
	diffuse = gDiffuseMap.Sample(samPoint, input.TexCoord);
	//dfinalColor = lightColor * gLightIntensity * (diffuse + phongTotal) * obersedArea;


	
	return (diffuse);
}
//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 DefaultTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(EnableDepth, 0);
		SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}