
#include "Shader_Define.hpp" 

struct BoneMatrixArray
{
	matrix				BoneMatrices[128];
};

cbuffer Matrices
{
	BoneMatrixArray		g_BoneMatrices;
};

texture2D			g_DiffuseTexture;
texture2D			g_SpecularTexture;
//texture2D			g_AmbientTexture;
//texture2D			g_EmissiveTexture;
//texture2D			g_HeightTexture;
texture2D			g_NormalTexture;
//texture2D			g_ShininessTexture;
texture2D			g_OpacityTexture;
//texture2D			g_DisplaceTexture;
//texture2D			g_LightMapTexture;
//texture2D			g_ReflectTexture;
//texture2D			g_BaseColorTexture;
//texture2D			g_NormalCamTexture;
//texture2D			g_EmissionColorTexture;
//texture2D			g_MetalTexture;
//texture2D			g_DiffuseRoughTexture;
//texture2D			g_AmbientOcculusionTexture;

struct VS_IN
{
	float3		vModelDataPosition : POSITION;
	float3		vModelDataNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;	
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;	
};

VS_OUT VS_MAIN_DEFAULT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;


	float		fWeightX = 1.f - (In.vBlendWeight.y + In.vBlendWeight.z + In.vBlendWeight.w);
	
	matrix		BoneMatrix = g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * In.vBlendWeight.w;

	vector		vLocalPosition = mul(vector(In.vModelDataPosition, 1.f), BoneMatrix);
	vector		vLocalNormal = mul(vector(In.vModelDataNormal, 0.f), BoneMatrix);


	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vLocalPosition, matWVP);
	Out.vNormal = mul(vLocalNormal, matWVP);
	Out.vTexUV = In.vTexUV;


	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.a = 1;

	return Out;
}

technique11		DefaultTechnique
{
	pass Rect
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}	
}