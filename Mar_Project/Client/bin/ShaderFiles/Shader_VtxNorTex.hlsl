
#include "Shader_Define.hpp" 

cbuffer LightDesc
{
	float3		g_vLightDir = float3(1.f, -1.f, 1.f);
	float4		g_vLightDiffuse = (float4)1.f;
	float4		g_vLightAmbient = (float4)1.f;
	float4		g_vLightSpecular = (float4)1.f;
};

texture2D		g_DiffuseTexture;
float3			g_CamPosition;
float3			g_CamLookDir;

cbuffer MtrlDesc
{
	float4		g_vMtrlAmbient = float4(0.2f, 0.2f, 0.2f, 1.f);
	float4		g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
		
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vWorldNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;

	float		vSpecPower : TEXCOORD1;
};

VS_OUT VS_MAIN_TERRAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	vector			vWorldPosition;
	matrix			matVP;

	vWorldPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out.vPosition = mul(vWorldPosition, matVP);
	Out.vWorldNormal = normalize(mul(vector(In.vNormal,0), g_WorldMatrix).xyz);
	Out.vTexUV = In.vTexUV;

	float3	fSpecDir = g_vLightDir + Out.vWorldNormal * dot((g_vLightDir * -1.f), Out.vWorldNormal) * 2.f;
	
	float CosValue = max(dot(normalize(fSpecDir), normalize(g_CamLookDir * -1.f)), 0);
	//float CosValue = max(dot(normalize(fSpecDir), normalize(g_CamPosition - vWorldPosition.xyz)), 0);

	Out.vSpecPower =  pow(CosValue, 20.f);

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vWorldNormal  : NORMAL;
	float2		vTexUV : TEXCOORD0;


	float		vSpecPower : TEXCOORD1;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_TERRAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;


	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV * 30.f);


	float	fShade = max(dot(normalize(g_vLightDir) * -1.f, In.vWorldNormal), 0.f);

	
	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + g_vLightAmbient * g_vMtrlAmbient)
		+ (g_vLightSpecular * g_vMtrlSpecular) * In.vSpecPower;

	return Out;
}
//(L.Diffus *  M.Diffuse) * (Shade (0 ~ 1) + (L.Ambient * M.Ambient)) + 
//(L.Specular * M.Specular) * 스펙큘러의 세기(0 ~ 1 사이의 실수)

technique11		DefaultTechnique
{
	pass Terrain
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_TERRAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TERRAIN();
	}	
}