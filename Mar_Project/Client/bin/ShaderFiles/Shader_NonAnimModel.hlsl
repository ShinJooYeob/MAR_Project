
#include "Shader_Define.hpp" 

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

cbuffer LightDesc
{
	float4		g_vLightVector;
	float4		g_vLightDiffuse;
	float4		g_vLightAmbient;
	float4		g_vLightSpecular;
};
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

cbuffer HiddenPad
{
	float	g_fVisualValue;
};
cbuffer MixColor
{
	float4			g_vMixColor = float4(1,0.6f,1,1);
};


struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;	
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN_DEFAULT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);



	Out.vTexUV = In.vTexUV;
	
	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	float		fShade = saturate(dot(normalize(g_vLightVector) * -1.f, In.vNormal));

	float4		vReflect = reflect(normalize(g_vLightVector), In.vNormal);
	float4		vLook = normalize(In.vWorldPos - g_CamPosition);

	float		fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), 30.f);

	Out.vColor = (g_vLightDiffuse * vDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
		(g_vLightSpecular * g_vMtrlSpecular) * fSpecular;



	return Out;
}
PS_OUT PS_MAIN_ZTESTALLMOST(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (vDiffuse.a < 1)
		discard;

	float		fShade = saturate(dot(normalize(g_vLightVector) * -1.f, In.vNormal));

	float4		vReflect = reflect(normalize(g_vLightVector), In.vNormal);
	float4		vLook = normalize(In.vWorldPos - g_CamPosition);

	float		fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), 30.f);

	Out.vColor = (g_vLightDiffuse * vDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
		(g_vLightSpecular * g_vMtrlSpecular) * fSpecular;



	return Out;
}

PS_OUT PS_MAIN_HIDDENPAD(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if ((vDiffuse.r + vDiffuse.g + vDiffuse.b) / 3 > g_fVisualValue)
		discard;

	float		fShade = saturate(dot(normalize(g_vLightVector) * -1.f, In.vNormal));

	float4		vReflect = reflect(normalize(g_vLightVector), In.vNormal);
	float4		vLook = normalize(In.vWorldPos - g_CamPosition);

	float		fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), 30.f);

	Out.vColor = (g_vLightDiffuse * vDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
		(g_vLightSpecular * g_vMtrlSpecular) * fSpecular;



	return Out;
}
PS_OUT PS_MAIN_PARTICLEREMOVEALPHA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	float Alpha = (vDiffuse.r);

	if (Alpha < 0.3f)
		discard;

	vDiffuse = vDiffuse*0.3 +  g_vMixColor*0.7;
	vDiffuse.a = Alpha*0.5;

	float		fShade = saturate(dot(normalize(g_vLightVector) * -1.f, In.vNormal));

	//float4		vReflect = reflect(normalize(g_vLightVector), In.vNormal);
	//float4		vLook = normalize(In.vWorldPos - g_CamPosition);

	//float		fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), 30.f);

	Out.vColor = (g_vLightDiffuse * vDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient));
	//+(g_vLightSpecular * g_vMtrlSpecular) * fSpecular;


	return Out;
}
PS_OUT PS_MAIN_SKYBOX(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);


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
	pass UnderAllDiscard		//1
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ZTESTALLMOST();
	}
	pass CullModeNone		//2
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}
	pass UnderAllDiscard_CullModeNone		//3
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ZTESTALLMOST();
	}
	pass HiddenPad		//4
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HIDDENPAD();
	}

	pass ParticleRemoveAlpha		//5
	{
		SetBlendState(ParticleBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PARTICLEREMOVEALPHA();
	}

	pass SkyBox		//6
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);
		//SetRasterizerState(CullMode_cw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SKYBOX();
	}
}