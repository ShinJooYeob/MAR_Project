
#include "Shader_Define.hpp"

texture2D			g_DiffuseTexture;
texture2D			g_SourTexture;
texture2D			g_BackBufferTexture;
texture2D			g_BlurTargetTexture;
texture2D			g_DepthTexture;
texture2D			g_NoiseTexture;


cbuffer Blur 
{
	float2				g_vCenter = float2(0.5f, 0.5f);
	float				g_fZoomSize = 0.25f;
	float				g_fZoomPower = 0.5f;
	float				g_fTime;
}
cbuffer ForNoise
{
	float3				g_vScale = float3(1, 2, 3);
	float3				g_vScrollSpeeds = float3(2.f,3.f,4.f);
}


cbuffer Particle {

	float  g_fAlphaTestValue = 0.1f;
	float2 g_vUVPos = float2(0,0);
	float2 g_vUVSize = float2(1, 1);
	float4 g_vColor = float4(1.f, 1.f, 1.f, 1.f);

	float g_CutY = 0;

};

cbuffer DistortionBuffer
{
	float2 distortion1 = float2(0.1f, 0.2f);
	float2 distortion2 = float2(0.1f, 0.3f);
	float2 distortion3 = float2(0.1f, 0.1f);
	float distortionScale = 0.8f;
	float distortionBias = 0.5f;
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

struct VS_Noise_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float2		texCoords1 : TEXCOORD1;
	float2		texCoords2 : TEXCOORD2;
	float2		texCoords3 : TEXCOORD3;
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
struct VS_OUT_SOFT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};


VS_OUT VS_MAIN_RECT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);


	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_MAIN_PARTICLE(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;


	Out.vTexUV.x = g_vUVPos.x + In.vTexUV.x*g_vUVSize.x;
	Out.vTexUV.y = g_vUVPos.y + In.vTexUV.y*g_vUVSize.y;

	//Out.vTexUV = In.vTexUV;


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


	Out.texCoord1 = In.vTexUV + float2(texelSize * -4.0f, 0.0f);
	Out.texCoord2 = In.vTexUV + float2(texelSize * -3.0f, 0.0f);
	Out.texCoord3 = In.vTexUV + float2(texelSize * -2.0f, 0.0f);
	Out.texCoord4 = In.vTexUV + float2(texelSize * -1.0f, 0.0f);
	Out.texCoord5 = In.vTexUV + float2(texelSize *  0.0f, 0.0f);
	Out.texCoord6 = In.vTexUV + float2(texelSize *  1.0f, 0.0f);
	Out.texCoord7 = In.vTexUV + float2(texelSize *  2.0f, 0.0f);
	Out.texCoord8 = In.vTexUV + float2(texelSize *  3.0f, 0.0f);
	Out.texCoord9 = In.vTexUV + float2(texelSize *  4.0f, 0.0f);



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


	Out.texCoord1 = In.vTexUV + float2(0.0f, texelSize * -4.0f);
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

VS_OUT_SOFT VS_MAIN_SOFTRECT(VS_IN In)
{
	VS_OUT_SOFT			Out;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	Out.vTexUV.x = g_vUVPos.x + In.vTexUV.x*g_vUVSize.x;
	Out.vTexUV.y = g_vUVPos.y + In.vTexUV.y*g_vUVSize.y;

	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_Noise_OUT VS_MAIN_Noise(VS_IN In)
{
	VS_Noise_OUT			Out;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	Out.vTexUV.x = g_vUVPos.x + In.vTexUV.x*g_vUVSize.x;
	Out.vTexUV.y = g_vUVPos.y + In.vTexUV.y*g_vUVSize.y;


	// ?????? ?????? ???????? ?????? ?????? ???? ?? ?????? ?????? ???? ???? ???????? ??????????.
	Out.texCoords1 = (Out.vTexUV * g_vScale.x);
	Out.texCoords1.y = Out.texCoords1.y + (g_fTime * g_vScrollSpeeds.x);

	// ?????? ?????? ???????? ?????? ?????? ???? ?? ?????? ?????? ???? ???? ???????? ??????????.
	Out.texCoords2 = (Out.vTexUV * g_vScale.y);
	Out.texCoords2.y = Out.texCoords2.y + (g_fTime * g_vScrollSpeeds.y);

	// ?????? ?????? ???????? ?????? ?????? ???? ?? ?????? ?????? ???? ???? ???????? ??????????.
	Out.texCoords3 = (Out.vTexUV * g_vScale.z);
	Out.texCoords3.y = Out.texCoords3.y + (g_fTime * g_vScrollSpeeds.z);

	return Out;
}



struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};
struct PS_IN_SOFT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_IN_Noise
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float2		texCoords1 : TEXCOORD1;
	float2		texCoords2 : TEXCOORD2;
	float2		texCoords3 : TEXCOORD3;
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
struct PS_OUT_Emissive
{
	vector		vColor : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
	vector		vSpecular : SV_TARGET3;
	vector		vEmissive : SV_TARGET4;
};

PS_OUT PS_MAIN_RECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);//vector(1.f, 0.f, 0.f, 1.f);rgba

	if (Out.vColor.a < 0.1f)
		discard; 

	Out.vColor *= g_vColor;

	return Out;
}

PS_OUT PS_MAIN_Default(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);//vector(1.f, 0.f, 0.f, 1.f);rgba



	return Out;
}
PS_OUT PS_MAIN_UI_CUTY(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (In.vPosition.y < g_CutY)
		discard;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);//vector(1.f, 0.f, 0.f, 1.f);rgba

	if (Out.vColor.a < 0.1f)
		discard;

	Out.vColor *= g_vColor;

	return Out;
}


PS_OUT PS_MAIN_WORLD(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);//vector(1.f, 0.f, 0.f, 1.f);rgba

	if (Out.vColor.a < 0.1f)
		discard;



	return Out;
}
PS_OUT PS_MAIN_PARTICLE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) * g_vColor;//vector(1.f, 0.f, 0.f, 1.f);rgba

	if (Out.vColor.a < g_fAlphaTestValue)
		discard;


	return Out;
}

PS_OUT PS_MAIN_PARTICLEEDGE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 Center = g_vUVPos + g_vUVSize *0.5f;

	float Rate = saturate(length(In.vTexUV.xy - Center.xy)/ (g_vUVSize.x * 0.5f) );

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);//vector(1.f, 0.f, 0.f, 1.f);rgba

	if (Out.vColor.a < g_fAlphaTestValue)
		discard;
	float Alpha = Out.vColor.a;
	Out.vColor *= (vector(0,0,0,1) * (Rate) +  (1-Rate )* g_vColor);


	Out.vColor.a = Alpha;
	return Out;
}
PS_OUT PS_MAIN_PARTICLEREMOVEALPHA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);//vector(1.f, 0.f, 0.f, 1.f);rgba
	float Alpha = Out.vColor.r;

	if (Alpha < g_fAlphaTestValue)
		discard;

	//Out.vColor += (Out.vColor*g_vColor*0.2 + g_vColor*0.8) ;
	Out.vColor *= g_vColor;
	Out.vColor.a = Alpha * 0.5f;
	return Out;
}

PS_OUT PS_MAIN_PARTICLEREMOVEALPHA2(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);//vector(1.f, 0.f, 0.f, 1.f);rgba
	float Alpha = Out.vColor.r;

	if (Alpha < g_fAlphaTestValue)
		discard;

	Out.vColor += (Out.vColor*g_vColor*0.2 + g_vColor*0.8) ;
	//Out.vColor *= g_vColor;
	Out.vColor.a = Alpha * 0.5f;
	return Out;
}
PS_OUT PS_MAIN_PARTICLEREMOVEALPHA3(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);//vector(1.f, 0.f, 0.f, 1.f);rgba
	float Alpha = max(max(Out.vColor.r, Out.vColor.g), Out.vColor.b);

	if (Alpha < g_fAlphaTestValue)
		discard;

	Out.vColor = (Out.vColor * 0.8 + g_vColor*0.2);
	//Out.vColor = (Out.vColor * 0.8 + g_vColor*0.2);
	//Out.vColor *= g_vColor;
	Out.vColor.a = min(Alpha, g_vColor.a);
	return Out;
}
PS_OUT PS_MAIN_PARTICLEREMOVEALPHA4(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);//vector(1.f, 0.f, 0.f, 1.f);rgba
	float Alpha = max(max(Out.vColor.r, Out.vColor.g), Out.vColor.b);

	if (Alpha < g_fAlphaTestValue || g_vColor.a < g_fAlphaTestValue)
		discard;

	Out.vColor = (Out.vColor * 0.8 + g_vColor*0.2);
	//Out.vColor = (Out.vColor * 0.8 + g_vColor*0.2);
	//Out.vColor *= g_vColor;
	Out.vColor.a = g_vColor.a;
	return Out;
}
PS_OUT PS_FADE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);//vector(1.f, 0.f, 0.f, 1.f);rgba

	if (Out.vColor.a < g_fAlphaTestValue)
		discard;

	Out.vColor *= g_vColor;

	return Out;
}
PS_OUT PS_FADE_Flicker(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);//vector(1.f, 0.f, 0.f, 1.f);rgba

	Out.vColor = g_vColor;

	float Centerlength = length(In.vTexUV - float2(0.5f, 0.5f));


	if (Centerlength > g_fAlphaTestValue)
	{
		Out.vColor.a = 1.f * g_vColor.w;
	}
	else
	{
		Out.vColor = g_vColor;
		Out.vColor.a = Centerlength / g_fAlphaTestValue * g_vColor.w;
	}
	
	return Out;
}




PS_OUT PS_MAIN_ALLMOSTDISCARD(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);//vector(1.f, 0.f, 0.f, 1.f);rgba

	if (Out.vColor.a < 0.5f)
		discard;



	return Out;
}

PS_OUT PS_illusion(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;


	vector BlurDesc = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//float Alpha = max(max(Out.vDiffuse.r, Out.vDiffuse.g), Out.vDiffuse.b);

	//if (Alpha < 0.3)
	//	discard;


	float2 PosToUv = float2(In.vPosition.x / 1280, In.vPosition.y / 720);


	float2 TargetUV = float2(PosToUv.x + (0.5f - (BlurDesc.x))*0.15625f, PosToUv.y + (0.5f - (BlurDesc.y))*0.25f);
	//float2 TargetUV = In.vTexUV + float2(PosToUv.x + (0.5f - (Noise.x))*0.0015625f, PosToUv.y + (0.5f - (Noise.y))*0.0025f);

	vector BackBuffer = g_BackBufferTexture.Sample(DefaultSampler, TargetUV);

	Out.vColor = BackBuffer;

	Out.vColor.a = g_vColor.a;
	//Out.vDiffuse.a = min(Alpha, In.vColor.a);

	return Out;
}

PS_OUT PS_Blur(PS_IN_BLUR In)
{
	PS_OUT		Out = (PS_OUT)0;

	float weight0, weight1, weight2, weight3, weight4;
	float normalization;
	float4 color;

	vector NoiseDesc = g_NoiseTexture.Sample(DefaultSampler, In.tex);
	float len = length(NoiseDesc) ;

	weight0 = 1.0f ;
	weight1 = 0.9f;
	weight2 = 0.55f;
	weight3 = 0.18f;
	weight4 = 0.1f;

	//weight0 = 	weight1 = 	weight2 =	weight3 = 	weight4 = 0.1f;

	normalization = (weight0 +2.5f* (weight1 + weight2 + weight3 + weight4));

	// ?????????? ????????????.
	weight0 = weight0 / normalization;
	weight1 = weight1 / normalization;
	weight2 = weight2 / normalization;
	weight3 = weight3 / normalization;
	weight4 = weight4 / normalization;

	// ?????? ?????????? ????????????.
	color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float2 TargetUV;

	// ?????????? ???? ?????????? ???????? ???? ????????.
	color += g_BlurTargetTexture.Sample(DefaultSampler,	In.texCrd1) * weight4;
	color += g_BlurTargetTexture.Sample(DefaultSampler,	In.texCrd2) * weight3;
	color += g_BlurTargetTexture.Sample(DefaultSampler,	In.texCrd3) * weight2;
	color += g_BlurTargetTexture.Sample(DefaultSampler,	In.texCrd4) * weight1;
	color += g_BlurTargetTexture.Sample(DefaultSampler,	In.texCrd5) * weight0;
	color += g_BlurTargetTexture.Sample(DefaultSampler,	In.texCrd6) * weight1;
	color += g_BlurTargetTexture.Sample(DefaultSampler,	In.texCrd7) * weight2;
	color += g_BlurTargetTexture.Sample(DefaultSampler,	In.texCrd8) * weight3;
	color += g_BlurTargetTexture.Sample(DefaultSampler,	In.texCrd9) * weight4;

	// ???? ?????? 1?? ??????????.

	color.a = 1.0f;


	Out.vColor = color;
	return Out;
}

PS_OUT PS_RadialBlur(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float weight0;
	float normalization;
	float4 color;

	float2 vDir = In.vTexUV - g_vCenter;

	float Texel = 0.00133334f;

	int Count = max((length(vDir) - g_fZoomSize) / Texel , 2);

	float fPower = Texel * max(g_fZoomPower,0.1f);

	vDir = normalize(vDir);



	// ?????? ?????????? ????????????.
	color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll(100)]for (int i = 0; i < Count; i++)
	{

		color += g_BlurTargetTexture.Sample(ClampSampler, saturate( In.vTexUV + (vDir * fPower * (float)i))    );
	}
	color /= min((Count),100);

	// ???? ?????? 1?? ??????????.

	color.a = 1.0f;


	Out.vColor = color;
	return Out;
}



PS_OUT PS_MAIN_SOFTRECT(PS_IN_SOFT In)
{
	PS_OUT		Out = (PS_OUT)0;



	float2		vUV = In.vProjPos.xy / In.vProjPos.w;

	vUV.x = vUV.x * 0.5f + 0.5f;
	vUV.y = vUV.y * -0.5f + 0.5f;

	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vUV);

	float		fViewZ = vDepthDesc.x * 300.f;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	float Alpha = Out.vColor.r;

	Out.vColor *= g_vColor;
	Out.vColor.a = Alpha * 0.5f;

	Out.vColor.a = Out.vColor.a * saturate((fViewZ - In.vProjPos.w) * 2.f);

	if (Out.vColor.a < g_fAlphaTestValue)
		discard;

	return Out;

}


PS_OUT PS_MAIN_NoiseFireEffect(PS_IN_Noise In)
{

	PS_OUT		Out = (PS_OUT)0;

	vector noise1 = g_NoiseTexture.Sample(DefaultSampler, In.texCoords1);
	vector noise2 = g_NoiseTexture.Sample(DefaultSampler, In.texCoords2);
	vector noise3 = g_NoiseTexture.Sample(DefaultSampler, In.texCoords3);

	noise1 = (noise1 - 0.5f) * 2.0f;
	noise2 = (noise2 - 0.5f) * 2.0f;
	noise3 = (noise3 - 0.5f) * 2.0f;

	// ???????? x?? y???? ?? ???? ???? ???? x?? y?????? ??????????.
	noise1.xy = noise1.xy * distortion1.xy;
	noise2.xy = noise2.xy * distortion2.xy;
	noise3.xy = noise3.xy * distortion3.xy;

	// ?????? ?? ?????? ?????? ?????? ???????? ??????????.
	vector finalNoise = noise1 + noise2 + noise3;

	// ???????? ?????? ???????? Y?????? ???? ?????? ???????? ?????? ????????????.
	// ?? ?????? ???????? ???????? ?????? ???????? ?? ???????? ???????? ?????? ????????????.
	float perturb = saturate(((1.0f - In.vTexUV.y) * distortionScale) + distortionBias);

	// ???? ???? ???????? ???????????? ?????? ???? ?? ?????? ?????? ?????? ????????.
	float2 noiseCoords= saturate((finalNoise.xy * perturb) + In.vTexUV.xy);

	// ???????? ?????? ?????? ?????? ???????? ???? ?????????? ?????? ????????????.
	// wrap?? ???????? ???????? ???? clamp?? ???????? ?????????? ???????? ???? ???????? ???????? ???? ??????????.
	vector fireColor = g_DiffuseTexture.Sample(ClampSampler, noiseCoords.xy);
	//ClampSampler
	// ???????? ?????? ?????? ?????? ???????? ???? ?????????? ???????? ????????????.
	// ?????? ???????? ???????? ?? ?????? ????????.
	// wrap?? ???????? ???????? ???? clamp?? ???????? ?????????? ???????? ???? ???????? ???????? ???? ??????????.
	vector alphaColor = g_SourTexture.Sample(ClampSampler, noiseCoords.xy);

	fireColor *= alphaColor;
	fireColor.a = alphaColor.r * g_vColor.a;
	Out.vColor = fireColor;


	if (Out.vColor.a < g_fAlphaTestValue)
		discard;

	return Out;

	//Out.vColor.a = (DiscardValue  - g_fAlphaTestValue )/(1- g_fAlphaTestValue)  * g_vColor.a;

}


PS_OUT_Emissive PS_MAIN_PARTICLE_Emissive(PS_IN In)
{
	PS_OUT_Emissive		Out = (PS_OUT_Emissive)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV) * g_vColor;//vector(1.f, 0.f, 0.f, 1.f);rgba

	if (Out.vColor.a < g_fAlphaTestValue)
		discard;

	Out.vEmissive = 1;

	return Out;
}


PS_OUT_Emissive PS_MAIN_PARTICLE_Emissive_REMOVEALPHA3(PS_IN In)
{
	PS_OUT_Emissive		Out = (PS_OUT_Emissive)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);//vector(1.f, 0.f, 0.f, 1.f);rgba
	float Alpha = max(max(Out.vColor.r, Out.vColor.g), Out.vColor.b);

	if (Alpha < g_fAlphaTestValue)
		discard;

	Out.vColor = (Out.vColor * 0.8 + g_vColor*0.2);
	//Out.vColor = (Out.vColor * 0.8 + g_vColor*0.2);
	//Out.vColor *= g_vColor;
	Out.vColor.a = min(Alpha, g_vColor.a);

	Out.vEmissive = 1;
	return Out;
}
PS_OUT_Emissive PS_MAIN_PARTICLE_Emissive_EDGE(PS_IN In)
{
	PS_OUT_Emissive		Out = (PS_OUT_Emissive)0;

	float2 Center = g_vUVPos + g_vUVSize *0.5f;

	float Rate = saturate(length(In.vTexUV.xy - Center.xy) / (g_vUVSize.x * 0.5f));

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);//vector(1.f, 0.f, 0.f, 1.f);rgba

	if (Out.vColor.a < g_fAlphaTestValue)
		discard;
	float Alpha = Out.vColor.a;
	Out.vColor *= (vector(0, 0, 0, 1) * (Rate)+(1 - Rate)* g_vColor);


	Out.vColor.a = Alpha;
	Out.vEmissive = 1;
	return Out;
}



PS_OUT PS_MAIN_PaperCurl(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector PaperCurl = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vColor = PaperCurl;


	if (PaperCurl.b > PaperCurl.r + PaperCurl.g) discard;

	if (PaperCurl.r > PaperCurl.b + PaperCurl.g)
	{
		Out.vColor = g_SourTexture.Sample(DefaultSampler, In.vTexUV);
	}

	Out.vColor.a = 1;

	return Out;
}


PS_OUT PS_MAIN_RECT_PaperCurl(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	//Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);//vector(1.f, 0.f, 0.f, 1.f);rgba
	//float Alpha = Out.vColor.a;

	Out.vColor = g_vColor;
	//Out.vColor = pow(Out.vColor,1/( 1 + g_vColor.a));

	Out.vColor.a = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV).a;
	return Out;
}

technique11		DefaultTechnique
{
	pass Rect			//0
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0	PS_MAIN_RECT();
	}
	pass UI			//1
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}
	pass WorldRect			//2
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_WORLD();
	}
	pass ParticleRect			//3
	{
		SetBlendState(ParticleBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_PARTICLE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PARTICLE();
	}
	pass ParticleRectRemoveAlpha		//4
	{
		SetBlendState(ParticleBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_PARTICLE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PARTICLEREMOVEALPHA();
	}
	pass ParticleRectRemoveAlpha2		//5
	{
		SetBlendState(ParticleBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_PARTICLE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PARTICLEREMOVEALPHA2();
	}
	pass UICUTY			//6
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_UI_CUTY();
	}

	pass FADEEFFECT		//7
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FADE();
	}

	pass UIALMOSTDISCARD			//8
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALLMOSTDISCARD();
	}
	pass ParticleRectRemoveAlphaCullModeNone		//9
	{
		SetBlendState(ParticleBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_MAIN_PARTICLE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PARTICLEREMOVEALPHA3();
	}
	pass ParticleEdge			//10
	{
		SetBlendState(ParticleBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_PARTICLE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PARTICLEEDGE();
	}

	pass ParticleRectRemoveAlphaCullModeNone2		//11
	{
		SetBlendState(ParticleBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_MAIN_PARTICLE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PARTICLEREMOVEALPHA4();
	}
	pass illusion //12
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_illusion();
	}
	pass VerticleBlur// 13
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_VerticleBlur();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Blur();
	}

	pass HorizonBlur// 14
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_HorizBlur();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Blur();
	}
	pass DefaultCopy//15
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0  VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0	PS_MAIN_Default();
	}
	pass RadialBlur// 16
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_RadialBlur();
	}
	pass SoftRect	//17
	{
		SetBlendState(ParticleBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_SOFTRECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SOFTRECT();
	}
	pass NoiseFireEffect	//18
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_Noise();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NoiseFireEffect();
	}
	pass ParticleRect_Emissive_Cut			//19
	{
		SetBlendState(ParticleBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_PARTICLE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PARTICLE_Emissive();
	}
	pass ParticleRect_Emissive_RemoveAlphaCullModeNone		//20
	{
		SetBlendState(ParticleBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_MAIN_PARTICLE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PARTICLE_Emissive_REMOVEALPHA3();
	}
	pass Particle_Emissive_Edge			//21
	{
		SetBlendState(ParticleBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_PARTICLE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PARTICLE_Emissive_EDGE();
	}
	pass FADEEFFECT_Flicker		//22
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FADE_Flicker();
	}


	pass PaperCurl			//23
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PaperCurl();
	}

	pass UIALMOSTDISCARD_CullNone			//24
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(NonZTestAndWriteState, 0);
		SetRasterizerState(CullMode_None);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALLMOSTDISCARD();
	}
	pass ForPaperCurl//25
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0	PS_MAIN_RECT_PaperCurl();
	}
}