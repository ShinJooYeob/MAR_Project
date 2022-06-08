
#include "Shader_Define.hpp" 

texture2D			g_DiffuseTexture;
texture2D			g_SpecularTexture;
//texture2D			g_AmbientTexture;
//texture2D			g_EmissiveTexture;
//texture2D			g_HeightTexture;
texture2D			g_NormalTexture;
//texture2D			g_ShininessTexture;
//texture2D			g_OpacityTexture;
//texture2D			g_DisplaceTexture;
//texture2D			g_LightMapTexture;
//texture2D			g_ReflectTexture;
//texture2D			g_BaseColorTexture;
//texture2D			g_NormalCamTexture;
//texture2D			g_EmissionColorTexture;
//texture2D			g_MetalTexture;
//texture2D			g_DiffuseRoughTexture;
//texture2D			g_AmbientOcculusionTexture;

texture2D			g_BackBufferTexture;
texture2D			g_NoiseTexture;
texture2D			g_BurnRampTexture;



cbuffer AttechMatrix
{
	matrix g_AttechMatrix;
};

//cbuffer LightDesc
//{
//	float4		g_vLightVector;
//	float4		g_vLightDiffuse;
//	float4		g_vLightAmbient;
//	float4		g_vLightSpecular;
//};
//cbuffer CameraDesc
//{
//	float4			g_CamPosition;
//	float4			g_CamLookDir;
//};

//cbuffer MtrlDesc
//{
//	float4		g_vMtrlAmbient = float4(0.2f, 0.2f, 0.2f, 1.f);
//	float4		g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);
//};

cbuffer DeltaTime
{
	float			g_fDeltaTime = 0;
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
	float4		vProjPos : TEXCOORD2;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;

};

struct VS_OUT_SHADOW
{
	float4		vPosition : SV_POSITION;
	float4		vClipPosition : TEXCOORD1;
};

VS_OUT_SHADOW VS_Shadow(VS_IN In)
{

	VS_OUT_SHADOW			Out = (VS_OUT_SHADOW)0;


	Out.vPosition = mul(vector(In.vPosition, 1), g_WorldMatrix);
	Out.vPosition = mul(Out.vPosition, g_LightViewMatrix);
	Out.vPosition = mul(Out.vPosition, g_LightProjMatrix);

	Out.vClipPosition = Out.vPosition;
	return Out;
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
	Out.vProjPos = Out.vPosition;


	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));

	
	return Out;
}

VS_OUT VS_MAIN_ATTACHBONE(VS_IN In)
{


	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matrix			WorldMatrix = g_AttechMatrix;

	matWV = mul(WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), WorldMatrix);

	Out.vProjPos = Out.vPosition;

	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));

	return Out;
}




struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
};

struct PS_OUT
{
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
	vector		vSpecular : SV_TARGET3;
	vector		vEmissive : SV_TARGET4;
};

struct PS_OUT_NODEFERRED
{
	vector		vDiffuse : SV_TARGET0;
};

PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.0f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}
PS_OUT PS_MAIN_ZTESTALLMOST(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);


	if (vDiffuse.a < 1)
		discard;

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.0f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexUV);


	return Out;
}

PS_OUT PS_MAIN_HIDDENPAD(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);




	vector		NoiseDesc = g_NoiseTexture.Sample(DefaultSampler, In.vTexUV) - g_fVisualValue;

	if (NoiseDesc.r < 0)
		discard;

	if (NoiseDesc.r < 0.15 && g_fVisualValue > 0 && g_fVisualValue < 1)
	{
		vector		BurnRampDesc = pow(g_BurnRampTexture.Sample(DefaultSampler, float2(NoiseDesc.r *(1 / 0.15), 0)),1.5f);

		vDiffuse =BurnRampDesc;
		//Out.vEmissive = max((max(BurnRampDesc.r, BurnRampDesc.g), BurnRampDesc.b) - 0.15f, 0);
		Out.vEmissive = 1.f;
		//o.Emission = tex2D(_BurnRamp, float2(test *(1 / _BurnSize), 0));
		//o.Albedo *= o.Emission;
	}
	



	//if ((vDiffuse.r + vDiffuse.g + vDiffuse.b) / 3 > g_fVisualValue)
	//	discard;

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.0f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}
PS_OUT_NODEFERRED PS_MAIN_PARTICLEREMOVEALPHA(PS_IN In)
{
	PS_OUT_NODEFERRED		Out = (PS_OUT_NODEFERRED)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	float Alpha = (vDiffuse.r);

	if (Alpha < 0.3f)
		discard;

	vDiffuse = vDiffuse*0.3 +  g_vMixColor*0.7;
	vDiffuse.a = Alpha*0.5;

	Out.vDiffuse = vDiffuse;


	return Out;
}
PS_OUT_NODEFERRED PS_MAIN_SKYBOX(PS_IN In)
{
	PS_OUT_NODEFERRED		Out = (PS_OUT_NODEFERRED)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vDiffuse.a = 1;

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
PS_OUT_NODEFERRED PS_Distortion(PS_IN In)
{
	PS_OUT_NODEFERRED		Out = (PS_OUT_NODEFERRED)0;


	vector BlurDesc = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//float Alpha = max(max(Out.vDiffuse.r, Out.vDiffuse.g), Out.vDiffuse.b);

	//if (Alpha < 0.3)
	//	discard;


	float2 PosToUv = float2(In.vPosition.x / 1280, In.vPosition.y / 720);


	float2 TargetUV = float2(PosToUv.x + (0.5f - (BlurDesc.x))*0.15625f, PosToUv.y + (0.5f - (BlurDesc.y))*0.25f);
	//float2 TargetUV = In.vTexUV + float2(PosToUv.x + (0.5f - (Noise.x))*0.0015625f, PosToUv.y + (0.5f - (Noise.y))*0.0025f);

	vector BackBuffer = g_BackBufferTexture.Sample(DefaultSampler, TargetUV);

	Out.vDiffuse = BackBuffer;

	Out.vDiffuse.a = g_vMixColor.a;
	//Out.vDiffuse.a = min(Alpha, In.vColor.a);

	return Out;
}

PS_OUT_NODEFERRED PS_Distortion_Ball(PS_IN In)
{
	PS_OUT_NODEFERRED		Out = (PS_OUT_NODEFERRED)0;


	

	vector BlurDesc = g_NoiseTexture.Sample(DefaultSampler, In.vTexUV );
	vector DiffuseDesc = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	float Alpha = max(DiffuseDesc.r - 0.5f , 0) * 2.f;


	float2 PosToUv = float2(In.vPosition.x / 1280, In.vPosition.y / 720) ;


	float2 TargetUV = saturate(float2(PosToUv.x + (0.5f - (BlurDesc.x))*0.15625f, PosToUv.y + (0.5f - (BlurDesc.y))*0.25f));
	//float2 TargetUV = In.vTexUV + float2(PosToUv.x + (0.5f - (Noise.x))*0.0015625f, PosToUv.y + (0.5f - (Noise.y))*0.0025f);

	vector BackBuffer = g_BackBufferTexture.Sample(DefaultSampler, TargetUV);

	Out.vDiffuse = BackBuffer * (1 - Alpha) + (Alpha * g_vMixColor);
	//Out.vDiffuse.a = (Alpha - 0.45f) * 2.f;

	/*if (Alpha < 0.1)
		discard;*/

	//Out.vDiffuse.a = g_vMixColor.a;
	//Out.vDiffuse.a = min(Alpha, In.vColor.a);

	return Out;
}

PS_OUT_NODEFERRED PS_Distortion_Ball_NoColor(PS_IN In)
{
	PS_OUT_NODEFERRED		Out = (PS_OUT_NODEFERRED)0;




	vector BlurDesc = g_NoiseTexture.Sample(DefaultSampler, In.vTexUV);
	vector DiffuseDesc = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	float Alpha = max(DiffuseDesc.r - 0.5f, 0) * 2.f;


	float2 PosToUv = float2(In.vPosition.x / 1280, In.vPosition.y / 720);


	float2 TargetUV = saturate(float2(PosToUv.x + (0.5f - (BlurDesc.x))*0.15625f, PosToUv.y + (0.5f - (BlurDesc.y))*0.25f));
	//float2 TargetUV = In.vTexUV + float2(PosToUv.x + (0.5f - (Noise.x))*0.0015625f, PosToUv.y + (0.5f - (Noise.y))*0.0025f);

	vector BackBuffer = g_BackBufferTexture.Sample(DefaultSampler, TargetUV);

	Out.vDiffuse =pow( BackBuffer,1.f/1.5f);
	//Out.vDiffuse.a = (Alpha - 0.45f) * 2.f;

	/*if (Alpha < 0.1)
	discard;*/

	//Out.vDiffuse.a = g_vMixColor.a;
	//Out.vDiffuse.a = min(Alpha, In.vColor.a);

	return Out;
}



PS_OUT PS_Emissive(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);

	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 300.0f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
	Out.vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vEmissive = 1.f;

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

	pass MeshParticleRemoveAlpha	//5
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

	pass AttachBone		//7
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_cw);
		//SetRasterizerState(CullMode_cw);

		VertexShader = compile vs_5_0 VS_MAIN_ATTACHBONE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}
	pass AttachBone_ccw		//8
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);
		//SetRasterizerState(CullMode_cw);

		VertexShader = compile vs_5_0 VS_MAIN_ATTACHBONE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}
	pass ShadowMap		//9
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_Shadow();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Shadow();
	}
	pass ShadowMap_CullCW		//10
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_cw);

		VertexShader = compile vs_5_0 VS_Shadow();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Shadow();
	}
	pass Distortion_WriteZbuffer		//11
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Distortion();
	}
	pass Distortion_Ball_WriteZbuffer		//12
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Distortion_Ball();
	}

	pass Distortion_Ball_WriteZbuffer_NoColor		//13
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Distortion_Ball_NoColor();
	}

	pass Distortion_Emissive		//14
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Emissive();
	}
	pass Distortion_Emissive_CullNone		//15
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Emissive();
	}



	
}