
#include "Shader_Define.hpp" 

cbuffer LightDesc
{
	float4		g_vLightVector;
	float4		g_vLightDiffuse;
	float4		g_vLightAmbient;
	float4		g_vLightSpecular;
};

texture2D		g_DiffuseTexture;

cbuffer CameraDesc
{
	float4			g_CamPosition;
	float4			g_CamLookDir;
};

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
	float4		vWorldNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;

	float4		vWorldPos : TEXCOORD1;
	float		bIsNotDraw : TEXCOORD2;
};

VS_OUT VS_MAIN_TERRAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	if (In.vPosition.y <= -99999.f)
	{
		Out.bIsNotDraw = 1.f;
	}
	else
	{
		matrix			matWV, matWVP;

		matWV = mul(g_WorldMatrix, g_ViewMatrix);
		matWVP = mul(matWV, g_ProjMatrix);

		Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
		Out.vWorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
		Out.vTexUV = In.vTexUV;
		Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	}
	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vWorldNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;

	float4		vWorldPos : TEXCOORD1;
	float		bIsNotDraw : TEXCOORD2;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_TERRAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (In.bIsNotDraw)
	{
		discard;
	}
	else {

		vector	vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV * 30.f);

		float	fShade = max(dot(normalize(g_vLightVector) * -1.f, In.vWorldNormal), 0.f);

		vector	vReflect = reflect(normalize(g_vLightVector), In.vWorldNormal);
		vector	vLook = normalize(In.vWorldPos - g_CamPosition);

		float	fSpecular = pow(max(dot(normalize(vReflect) * -1.f, vLook), 0.f), 30.f);

		Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient))
			+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

	}

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