cbuffer	RenderingPipeLine
{
	matrix			g_WorldMatrix;
	matrix			g_ViewMatrix;
	matrix			g_ProjMatrix;
};
cbuffer	ShadowPipeLine
{
	matrix			g_LightViewMatrix;
	matrix			g_LightProjMatrix;
};



cbuffer LightDesc
{
	vector			g_vLightDir;
	vector			g_vLightPos;
	float			g_fLightRange;
	vector			g_vLightDiffuse;
	vector			g_vLightAmbient;
	vector			g_vLightSpecular;
	vector			g_vCamPosition;
};

cbuffer MtrlDesc
{
	vector			g_vMtrlDiffuse = (vector)1.f;
	vector			g_vMtrlAmbient = (vector)1.f;
	vector			g_vMtrlSpecular = (vector)1.f;
};

cbuffer Matrices
{
	matrix			g_ProjMatrixInv;
	matrix			g_ViewMatrixInv;
};

texture2D			g_TargetTexture;
texture2D			g_DiffuseTexture;
texture2D			g_NormalTexture;
texture2D			g_DepthTexture;
texture2D			g_ShadeTexture;
texture2D			g_SpecularTexture;


texture2D			g_ShadowMapTexture;

cbuffer ScreenSizeBuffer
{
	float				fScreemWidth = 1280;
	float				fScreemHeight = 780;
};

sampler DefaultSampler = sampler_state
{
	// D3D11_SAMPLER_DESC
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};


struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};
struct VS_OUT_BLUR
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;

	float2 texCoord1 : TEXCOORD1;
	float2 texCoord2 : TEXCOORD2;
	float2 texCoord3 : TEXCOORD3;
	float2 texCoord4 : TEXCOORD4;
	float2 texCoord5 : TEXCOORD5;
	float2 texCoord6 : TEXCOORD6;
	float2 texCoord7 : TEXCOORD7;
	float2 texCoord8 : TEXCOORD8;
	float2 texCoord9 : TEXCOORD9;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}
VS_OUT_BLUR VS_VerticleBlur(VS_IN In)
{
	VS_OUT_BLUR			Out = (VS_OUT_BLUR)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	float texelSize = 1.f / fScreemWidth;


	Out.texCoord1 =In.vTexUV + float2(texelSize * -4.0f, 0.0f);
	Out.texCoord2 =In.vTexUV + float2(texelSize * -3.0f, 0.0f);
	Out.texCoord3 =In.vTexUV + float2(texelSize * -2.0f, 0.0f);
	Out.texCoord4 =In.vTexUV + float2(texelSize * -1.0f, 0.0f);
	Out.texCoord5 =In.vTexUV + float2(texelSize *  0.0f, 0.0f);
	Out.texCoord6 =In.vTexUV + float2(texelSize *  1.0f, 0.0f);
	Out.texCoord7 =In.vTexUV + float2(texelSize *  2.0f, 0.0f);
	Out.texCoord8 =In.vTexUV + float2(texelSize *  3.0f, 0.0f);
	Out.texCoord9 =In.vTexUV + float2(texelSize *  4.0f, 0.0f);



	return Out;
}

VS_OUT_BLUR VS_HorizBlur(VS_IN In)
{
	VS_OUT_BLUR			Out = (VS_OUT_BLUR)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	float texelSize = 1.f / fScreemHeight;


	Out.texCoord1 = In.vTexUV + float2( 0.0f, texelSize * -4.0f);
	Out.texCoord2 = In.vTexUV + float2(0.0f, texelSize * -3.0f);
	Out.texCoord3 = In.vTexUV + float2(0.0f, texelSize * -2.0f);
	Out.texCoord4 = In.vTexUV + float2(0.0f, texelSize * -1.0f);
	Out.texCoord5 = In.vTexUV + float2(0.0f, texelSize *  0.0f);
	Out.texCoord6 = In.vTexUV + float2(0.0f, texelSize *  1.0f);
	Out.texCoord7 = In.vTexUV + float2(0.0f, texelSize *  2.0f);
	Out.texCoord8 = In.vTexUV + float2(0.0f, texelSize *  3.0f);
	Out.texCoord9 = In.vTexUV + float2(0.0f, texelSize *  4.0f);



	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_IN_BLUR
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;

	float2 texCrd1 : TEXCOORD1;
	float2 texCrd2 : TEXCOORD2;
	float2 texCrd3 : TEXCOORD3;
	float2 texCrd4 : TEXCOORD4;
	float2 texCrd5 : TEXCOORD5;
	float2 texCrd6 : TEXCOORD6;
	float2 texCrd7 : TEXCOORD7;
	float2 texCrd8 : TEXCOORD8;
	float2 texCrd9 : TEXCOORD9;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};
struct PS_OUT_AfterDeferred
{
	vector		vColor : SV_TARGET0;
	vector		vColor2 : SV_TARGET1;
};
struct PS_OUT_LIGHT
{
	vector		vShade : SV_TARGET0;
	vector		vSpecular : SV_TARGET1;
};


PS_OUT PS_MAIN_RECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_TargetTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}


PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT	Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.x * 300.f;

	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	Out.vShade = g_vLightDiffuse * saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient);
	//Out.vShade = 0.5f;

	Out.vShade.a = 1.f;

	vector		vReflect = reflect(normalize(g_vLightDir), vNormal);
	vector		vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 * w나누기. */
	//vWorldPos.x = (In.vTexUV.x * 2.f - 1.f);
	//vWorldPos.y = (In.vTexUV.y * -2.f + 1.f);
	//vWorldPos.z = vDepthDesc.y; /* 0 ~ 1 */
	//vWorldPos.w = 1.f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
	vWorldPos.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vWorldPos.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vWorldPos.z = vDepthDesc.y * fViewZ; /* 0 ~ f */  
	vWorldPos.w = 1.f * fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f);
	Out.vSpecular.a = 0.f;


	return Out;	
}
PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT	Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.x * 300.f;

	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vWorldPos;

	vWorldPos.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vWorldPos.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vWorldPos.z = vDepthDesc.y * fViewZ; /* 0 ~ f */
	vWorldPos.w = 1.f * fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vLightDir = vWorldPos - g_vLightPos;
	float		fDistance = length(vLightDir);

	float		fAtt = max(g_fLightRange - fDistance, 0.f) / g_fLightRange;

	Out.vShade = (g_vLightDiffuse * saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
	//Out.vShade = 0.5f;

	Out.vShade.a = 1.f;

	vector		vReflect = reflect(normalize(vLightDir), vNormal);


	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f) * fAtt;
	Out.vSpecular.a = 0.f;

	return Out;
}
PS_OUT_AfterDeferred PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT_AfterDeferred		Out = (PS_OUT_AfterDeferred)0;

	vector		vDiffuseDesc = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vShadeDesc = g_ShadeTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vSpecularDesc = g_SpecularTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vShadowMapDesc = g_ShadowMapTexture.Sample(DefaultSampler, In.vTexUV);

	
	//vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	//float		fViewZ = vDepthDesc.x * 300.f;

	Out.vColor = vDiffuseDesc * vShadeDesc + vSpecularDesc;



	if (Out.vColor.a == 0.0f)
		discard;

	Out.vColor *= (1 - vShadowMapDesc);
	

	Out.vColor2 = Out.vColor;
	return Out;
}


PS_OUT_AfterDeferred PS_DOWNSCALING(PS_IN In)
{
	PS_OUT_AfterDeferred		Out = (PS_OUT_AfterDeferred)0;

	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.x * 300.f;
	vector		vWorldPos;

	vWorldPos.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vWorldPos.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vWorldPos.z = vDepthDesc.y * fViewZ; /* 0 ~ f */
	vWorldPos.w = 1.f * fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	float4 ClipPos = mul(vWorldPos, g_LightViewMatrix);
	ClipPos = mul(ClipPos, g_LightProjMatrix);

	float CurrentDepth = ClipPos.z / ClipPos.w;
	float2 UV = ClipPos.xy / ClipPos.w;

	UV.y = UV.y * -0.5f + 0.5f;
	UV.x = UV.x * 0.5f + 0.5f;

	vector		vShadowDesc = g_ShadowMapTexture.Sample(DefaultSampler, UV);

	if (vShadowDesc.a == 0.0f)
		discard;

	float ShadowDepth = g_ShadowMapTexture.Sample(DefaultSampler, UV).r;


	//0.000125
	//if (CurrentDepth > ShadowDepth + 0.000125)
	//if (CurrentDepth > ShadowDepth )
	if (CurrentDepth > ShadowDepth + 0.00000025f)
	{
		Out.vColor = vector(0.5f, 0.5f, 0.5f,1);
	}
	Out.vColor2 = Out.vColor;
	return Out;
}
PS_OUT PS_UPSCALING(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor= g_ShadowMapTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_Blur(PS_IN_BLUR In)
{
	PS_OUT		Out = (PS_OUT)0;

	float weight0, weight1, weight2, weight3, weight4;
	float normalization;
	float4 color;

	weight0 = 1.0f;
	weight1 = 0.9f;
	weight2 = 0.55f;
	weight3 = 0.18f;
	weight4 = 0.1f;

	//weight0 = 	weight1 = 	weight2 =	weight3 = 	weight4 = 0.1f;

	normalization = (weight0 + 3.5f * (weight1 + weight2 + weight3 + weight4));

	// 가중치들을 정규화합니다.
	weight0 = weight0 / normalization;
	weight1 = weight1 / normalization;
	weight2 = weight2 / normalization;
	weight3 = weight3 / normalization;
	weight4 = weight4 / normalization;

	// 색깔을 검정색으로 초기화합니다.
	color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 수평선상의 아홉 픽셀값들을 가중치를 곱해 더합니다.
	color += g_ShadowMapTexture.Sample(DefaultSampler, In.texCrd1) * weight4;
	color += g_ShadowMapTexture.Sample(DefaultSampler, In.texCrd2) * weight3;
	color += g_ShadowMapTexture.Sample(DefaultSampler, In.texCrd3) * weight2;
	color += g_ShadowMapTexture.Sample(DefaultSampler, In.texCrd4) * weight1;
	color += g_ShadowMapTexture.Sample(DefaultSampler, In.texCrd5) * weight0;
	color += g_ShadowMapTexture.Sample(DefaultSampler, In.texCrd6) * weight1;
	color += g_ShadowMapTexture.Sample(DefaultSampler, In.texCrd7) * weight2;
	color += g_ShadowMapTexture.Sample(DefaultSampler, In.texCrd8) * weight3;
	color += g_ShadowMapTexture.Sample(DefaultSampler, In.texCrd9) * weight4;

	// 알파 채널을 1로 설정합니다.

		color.a = 1.0f;


	Out.vColor = color;
	return Out;
}

BlendState	NonBlending
{
	BlendEnable[0] = false;
};

BlendState	Blending_One
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	BlendOp = Add;
	SrcBlend = one;
	DestBlend = one;


};

DepthStencilState  NonZTestAndWriteState
{
	DepthEnable = false;
	DepthWriteMask = zero;
};
DepthStencilState  ZTestAndWriteState
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less;
};
RasterizerState CullMode_ccw
{
	FillMode = Solid;
	CullMode = back;
	FrontCounterClockwise = false;
};

technique11		DefaultTechnique
{
	pass DebugBuffer // 0
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}	

	pass Light_Directional// 1
	{
		SetBlendState(Blending_One, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}
	 
	pass Light_Point// 2
	{
		SetBlendState(Blending_One, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	pass Blend// 3
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}
	pass Debug// 4
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

	pass DownFittering// 5
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DOWNSCALING();
	}

	pass UpFittering// 6
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_UPSCALING();
	}
	pass VerticleBlur// 7
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_VerticleBlur();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Blur();
	}

	pass HorizonBlur// 8
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_HorizBlur();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Blur();
	}
}