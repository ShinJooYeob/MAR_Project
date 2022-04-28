
#include "Shader_Define.hpp"

texture2D			g_DiffuseTexture;
texture2D			g_SourTexture;

cbuffer Particle {

	float  g_fAlphaTestValue = 0.1f;
	float2 g_vUVPos = float2(0,0);
	float2 g_vUVSize = float2(1, 1);
	float4 g_vColor = float4(1.f, 1.f, 1.f, 1.f);

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


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_RECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);//vector(1.f, 0.f, 0.f, 1.f);rgba

	if (Out.vColor.a < 0.1f)
		discard; 

	//if (Out.vColor.a < 0.1f)
	//{
	//	Out.vColor = vector(1,0,1,0);
	//}
	//else {
	//	Out.vColor = vector(0, 0, 1, 1) * (In.vTexUV.x * In.vTexUV.y);
	//}


	


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


technique11		DefaultTechnique
{
	pass Rect			//0
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
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

}