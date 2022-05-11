
#include "Shader_Define.hpp" 

cbuffer LightDesc
{
	float4		g_vLightVector;
	float4		g_vLightDiffuse;
	float4		g_vLightAmbient;
	float4		g_vLightSpecular;
};

texture2D		g_DiffuseTexture;


texture2D		g_SourDiffuseTexture;
texture2D		g_DestDiffuseTexture1;
texture2D		g_DestDiffuseTexture2;
texture2D		g_DestDiffuseTexture3;
texture2D		g_DestDiffuseTexture4;

texture2D		g_FilterTexture;
texture2D		g_BrushTexture;

texture2D		g_HeightMapTexture;

cbuffer BrushDesc
{
	float4		g_vBrushPos = float4(10.0f, 0.0f, 10.f, 1.f);
	float		g_fRadius = 3.f;
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
	float		g_fMimMapSize = 20;
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


PS_OUT PS_MAIN_TERRAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (In.bIsNotDraw)
	{
		discard;
	}
	else {
		vector	vSourMtrlDiffuse = g_SourDiffuseTexture.Sample(DefaultSampler, In.vTexUV * g_fMimMapSize);
		vector	vDestMtrlDiffuse1 = g_DestDiffuseTexture1.Sample(DefaultSampler, In.vTexUV * g_fMimMapSize);
		vector	vDestMtrlDiffuse2 = g_DestDiffuseTexture2.Sample(DefaultSampler, In.vTexUV * g_fMimMapSize);
		vector	vDestMtrlDiffuse3 = g_DestDiffuseTexture3.Sample(DefaultSampler, In.vTexUV * g_fMimMapSize);
		vector	vDestMtrlDiffuse4 = g_DestDiffuseTexture4.Sample(DefaultSampler, In.vTexUV * g_fMimMapSize);


		vector	vFilterColor = g_FilterTexture.Sample(DefaultSampler, In.vTexUV);


		vector	vMtrlDiffuse = vSourMtrlDiffuse * (1.f - vFilterColor.a) + vDestMtrlDiffuse1 * (vFilterColor.a);
		vMtrlDiffuse = vMtrlDiffuse * (1.f - vFilterColor.r) + vDestMtrlDiffuse2 * (vFilterColor.r);
		vMtrlDiffuse = vMtrlDiffuse * (1.f - vFilterColor.g) + vDestMtrlDiffuse3 * (vFilterColor.g);
		vMtrlDiffuse = vMtrlDiffuse * (1.f - vFilterColor.b) + vDestMtrlDiffuse4 * (vFilterColor.b) ;



		float	fShade = max(dot(normalize(g_vLightVector) * -1.f, In.vWorldNormal), 0.f);

		vector	vReflect = reflect(normalize(g_vLightVector), In.vWorldNormal);
		vector	vLook = normalize(In.vWorldPos - g_CamPosition);

		float	fSpecular = pow(max(dot(normalize(vReflect) * -1.f, vLook), 0.f), 30.f);


		Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient))
			+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;


		float  FogShaderRate = 1 - saturate(  max((3.f - In.vWorldPos.y), 0) / 3.f);
		
		Out.vColor.a *= FogShaderRate;
	}

	return Out;
}
//(L.Diffus *  M.Diffuse) * (Shade (0 ~ 1) + (L.Ambient * M.Ambient)) + 
//(L.Specular * M.Specular) * 스펙큘러의 세기(0 ~ 1 사이의 실수)


PS_OUT PS_MAIN_TERRAIN_POINT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (In.bIsNotDraw)
	{
		discard;
	}
	else {

		vector	vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV * 30.f);


		vector	vLightDir = In.vWorldPos - g_vLightVector;
		float  fLength = length(vLightDir);

		float	fShade = max(dot(normalize(vLightDir) * -1.f, In.vWorldNormal), 0.f);

		vector	vReflect = reflect(normalize(vLightDir), In.vWorldNormal);
		vector	vLook = normalize(In.vWorldPos - g_CamPosition);

		float	fSpecular = pow(max(dot(normalize(vReflect) * -1.f, vLook), 0.f), 30.f);

		Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient))
			+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

		Out.vColor *= saturate((10.f - fLength) / 10.f);
		
	}

	return Out;
}

PS_OUT PS_MAIN_TERRAIN_WIRE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	if (In.bIsNotDraw)
	{
		discard;
	}
	else {
		vector	vBrushColor = (vector)0.f;

		if (g_vBrushPos.x - g_fRadius < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fRadius &&
			g_vBrushPos.z - g_fRadius < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fRadius)
		{
			float2		vBrushUV;

			vBrushUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fRadius)) / (2.f * g_fRadius);
			vBrushUV.y = ((g_vBrushPos.z + g_fRadius) - In.vWorldPos.z) / (2.f * g_fRadius);


			vBrushColor = g_BrushTexture.Sample(DefaultSampler, vBrushUV);
		}

		vector	vTileKinds = g_HeightMapTexture.Sample(PointSampler, In.vTexUV);

		//Out.vColor = vector(0, 0.5f, 0, 1) + vBrushColor;

		//vector	vTileKinds = g_HeightMapTexture.Sample(DefaultSampler, HeightUV);

		Out.vColor = vTileKinds;

		if (vTileKinds.r <= 0.f)// 1 못가는곳
		{
			Out.vColor = vector(1, 0, 0, 1) + vBrushColor;
		}
		else if (vTileKinds.r <= 0.00785f) // 2 이하 특수타일
		{
			Out.vColor = vector(0, 0, 1, 1) + vBrushColor;
		}
		else if(vTileKinds.r <= 0.01177f) // 움직일수 있는곳
		{
			Out.vColor = vector(0, 0.5f, 0, 1) + vBrushColor;
		}

	}

	return Out;
}

PS_OUT PS_MAIN_TERRAIN_EDIT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	if (In.bIsNotDraw)
	{
		discard;
	}
	else {

		vector	vSourMtrlDiffuse = g_SourDiffuseTexture.Sample(DefaultSampler, In.vTexUV * g_fMimMapSize);
		vector	vDestMtrlDiffuse1 = g_DestDiffuseTexture1.Sample(DefaultSampler, In.vTexUV * g_fMimMapSize);
		vector	vDestMtrlDiffuse2 = g_DestDiffuseTexture2.Sample(DefaultSampler, In.vTexUV * g_fMimMapSize);
		vector	vDestMtrlDiffuse3 = g_DestDiffuseTexture3.Sample(DefaultSampler, In.vTexUV * g_fMimMapSize);
		vector	vDestMtrlDiffuse4 = g_DestDiffuseTexture4.Sample(DefaultSampler, In.vTexUV * g_fMimMapSize);


		//vector	vBrushColor = g_BrushTexture.Sample(DefaultSampler, In.vTexUV);	
		vector	vBrushColor = (vector)0.f;
		vector	vFilterColor = g_FilterTexture.Sample(DefaultSampler, In.vTexUV);

		if (g_vBrushPos.x - g_fRadius < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fRadius &&
			g_vBrushPos.z - g_fRadius < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fRadius)
		{
			float2		vBrushUV;

			vBrushUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fRadius)) / (2.f * g_fRadius);
			vBrushUV.y = ((g_vBrushPos.z + g_fRadius) - In.vWorldPos.z) / (2.f * g_fRadius);


			vBrushColor = g_BrushTexture.Sample(DefaultSampler, vBrushUV);
		}
		
	
		vector	vMtrlDiffuse = vSourMtrlDiffuse * (1.f - vFilterColor.a) + vDestMtrlDiffuse1 * (vFilterColor.a);
		vMtrlDiffuse = vMtrlDiffuse * (1.f - vFilterColor.r) + vDestMtrlDiffuse2 * (vFilterColor.r);
		vMtrlDiffuse = vMtrlDiffuse * (1.f - vFilterColor.g) + vDestMtrlDiffuse3 * (vFilterColor.g);
		vMtrlDiffuse = vMtrlDiffuse * (1.f - vFilterColor.b) + vDestMtrlDiffuse4 * (vFilterColor.b) + vBrushColor;



	/*	float	fShade = max(dot(normalize(g_vLightVector) * -1.f, In.vWorldNormal), 0.f);

		vector	vReflect = reflect(normalize(g_vLightVector), In.vWorldNormal);
		vector	vLook = normalize(In.vWorldPos - g_CamPosition);

		float	fSpecular = pow(max(dot(normalize(vReflect) * -1.f, vLook), 0.f), 30.f);

		Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient))
			+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;*/

		Out.vColor = vMtrlDiffuse;
	}
	return Out;
}



technique11		DefaultTechnique
{
	pass Terrain_DirectionalLight // 0
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_TERRAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TERRAIN_DIRECTIONAL();
	}	

	pass Terrain_PointLight // 1
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_TERRAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TERRAIN_POINT();
	}
	pass Terrain_Wire // 2
	{
		SetBlendState(NonBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(FillMode_Wireframe);

		VertexShader = compile vs_5_0 VS_MAIN_TERRAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TERRAIN_WIRE();
	}
	pass Terrain_EditTerrain // 3
	{
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(ZTestAndWriteState, 0);
		SetRasterizerState(CullMode_ccw);

		VertexShader = compile vs_5_0 VS_MAIN_TERRAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TERRAIN_EDIT();
	}
}