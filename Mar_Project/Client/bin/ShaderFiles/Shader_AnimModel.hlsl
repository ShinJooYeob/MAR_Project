
#include "Shader_Define.hpp" 

struct BoneMatrixArray
{
	matrix				BoneMatrices[150];
};

cbuffer Matrices
{
	BoneMatrixArray		g_BoneMatrices;
};


cbuffer AttechMatrix
{
	matrix g_AttechMatrix;
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
texture2D			g_MskingTextrue;


//cbuffer MtrlDesc
//{
//	float4		g_vMtrlAmbient = float4(0.4f, 0.4f, 0.4f, 1.f);
//	float4		g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);
//};


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
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct VS_OUT_SHADOW
{
	float4		vPosition : SV_POSITION;
	float4		vClipPosition : TEXCOORD1;
};

VS_OUT_SHADOW VS_Shadow(VS_IN In)
{
	
	VS_OUT_SHADOW			Out = (VS_OUT_SHADOW)0;

	matrix			matWV, matWVP;


	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix = g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector		vLocalPosition = mul(vector(In.vModelDataPosition, 1.f), BoneMatrix);


	Out.vPosition = mul(vLocalPosition, g_WorldMatrix);
	Out.vPosition = mul(Out.vPosition, g_LightViewMatrix);
	Out.vPosition = mul(Out.vPosition, g_LightProjMatrix);

	Out.vClipPosition = Out.vPosition;
	return Out;
};
VS_OUT_SHADOW VS_Shadow_NoWeightW(VS_IN In)
{

	VS_OUT_SHADOW			Out = (VS_OUT_SHADOW)0;

	matrix			matWV, matWVP;


	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		BoneMatrix = g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * In.vBlendWeight.w;

	vector		vLocalPosition = mul(vector(In.vModelDataPosition, 1.f), BoneMatrix);


	Out.vPosition = mul(vLocalPosition, g_WorldMatrix);
	Out.vPosition = mul(Out.vPosition, g_LightViewMatrix);
	Out.vPosition = mul(Out.vPosition, g_LightProjMatrix);

	Out.vClipPosition = Out.vPosition;
	return Out;
};


VS_OUT VS_MAIN_DEFAULT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;


	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
	
	matrix		BoneMatrix = g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector		vLocalPosition = mul(vector(In.vModelDataPosition, 1.f), BoneMatrix);
	vector		vLocalNormal = mul(vector(In.vModelDataNormal, 0.f), BoneMatrix);


	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vLocalPosition, matWVP);
	Out.vNormal = normalize(mul(vLocalNormal, matWVP));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vLocalPosition, g_WorldMatrix);
	Out.vProjPos = Out.vPosition;


	return Out;
}

VS_OUT VS_MAIN_NOWEIGHTW(VS_IN In)
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
	Out.vNormal = normalize(mul(vLocalNormal, matWVP));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vLocalPosition, g_WorldMatrix);
	Out.vProjPos = Out.vPosition;


	return Out;
}

VS_OUT VS_MAIN_ATTACHEDNOWEIGHTW(VS_IN In)
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

	matrix			WorldMatrix = g_AttechMatrix;

	matWV = mul(WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vLocalPosition, matWVP);
	Out.vNormal = normalize(mul(vLocalNormal, matWVP));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vLocalPosition, WorldMatrix);
	Out.vProjPos = Out.vPosition;


	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
	vector		vShadow : SV_TARGET3;
};
struct PS_OUT_NODEFERRED
{
	vector		vDiffuse : SV_TARGET0;
};
PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (vDiffuse.a < 0.1f)
		discard;

	//float		fShade = saturate(dot(normalize(g_vLightVector) * -1.f, In.vNormal));

	//float4		vReflect = reflect(normalize(g_vLightVector), In.vNormal);
	//float4		vLook = normalize(In.vWorldPos - g_CamPosition);

	//float		fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), 30.f);

	//Out.vColor = (g_vLightDiffuse * vDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
	//	(g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.0f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);

	return Out;
}

PS_OUT_NODEFERRED PS_MAIN_LOBYALICE(PS_IN In)
{
	PS_OUT_NODEFERRED		Out = (PS_OUT_NODEFERRED)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (vDiffuse.a < 0.1f)
		discard;

	Out.vDiffuse = vDiffuse;

	return Out;
}
PS_OUT PS_MAIN_MSKINGTEX(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	//vector		vMsk = g_OpacityTexture.Sample(PointSampler, In.vTexUV);
	//vector		vDiffuse = vMsk;
	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) * g_MskingTextrue.Sample(PointSampler, In.vTexUV);


	if (vDiffuse.a < 0.1f)
		discard;


	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.0f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);

	return Out;
}

struct PS_IN_SHADOW
{
	float4		vPosition : SV_POSITION;
	float4		vClipPosition : TEXCOORD1;
};

struct PS_OUT_SHADOW
{
	vector		vDiffuse : SV_TARGET0;
};


PS_OUT_SHADOW PS_Shadow(PS_IN_SHADOW In)
{
	PS_OUT_SHADOW		Out = (PS_OUT_SHADOW)0;

	float Depth = In.vClipPosition.z / In.vClipPosition.w;

	//Out.vDiffuse = float4(Depth.xxx, 1);


	Out.vDiffuse = float4(Depth.x, In.vClipPosition.z, In.vClipPosition.w, 1);

	return Out;
}

technique11		DefaultTechnique
{
	pass Default		//0
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}	
	pass NOWEIGHTW		//1
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_NOWEIGHTW();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}
	pass Player		//2
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_NOWEIGHTW();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}
	pass Player_CullModeNone		//3
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_MAIN_NOWEIGHTW();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}
	pass Monster_CullModeNone		//4
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}
	pass LobyAlice //5
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_NOWEIGHTW();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LOBYALICE();
	}

	pass AttachedWeapon //6
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_ATTACHEDNOWEIGHTW();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}
	pass AttachedWeaponMSK //7
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_ATTACHEDNOWEIGHTW();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MSKINGTEX();
	}
	pass AttachedWeaponNonCull //8
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_MAIN_ATTACHEDNOWEIGHTW();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}

	pass ShadowMap //9
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_Shadow();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Shadow();
	}
	pass ShadowMap_NoWeightW //10
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_Shadow_NoWeightW();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Shadow();
	}
	pass ShadowMap_NoWeightW_CullNone //11
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_Shadow_NoWeightW();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Shadow();
	}

}