#pragma once




BEGIN(Client)

enum ParticleID
{
	Particle_Fixed,
	Particle_Ball,
	Particle_Suck,
	Particle_Straight,
	//Particle_Cone,
	//Particle_Fountain,
	//Particle_Spread,

	Particle_End
};

typedef struct tagParticleDesc
{
	ParticleID eParticleTypeID = Particle_Fixed;

	CTransform* FollowingTarget = nullptr;
	_float3 FixedTarget = _float3(0, 0, 0);

	const _tchar* szTextureProtoTypeTag = nullptr;
	const _tchar* szTextureLayerTag = nullptr;
	_uint		iSimilarLayerNum = 1;


	_uint	 TextureChageFrequency = 0;
	_float2	 vTextureXYNum = _float2(1, 1);

	_float	TotalParticleTime = 0;
	_float	EachParticleLifeTime = 0;
	_uint	MaxParticleCount = 0;

	_uint	SizeChageFrequency = 0;
	_float3 ParticleSize = _float3(1, 1, 1);
	_float3 ParticleSize2 = _float3(0, 0, 0);

	_uint  ColorChageFrequency = 0;
	_float3 TargetColor = _float3(255.f, 255.f, 255.f);
	_float3 TargetColor2 = _float3(255.f, 255.f, 255.f);


	_float Particle_Power = 0;
	_float2 PowerRandomRange = _float2(0.5f, 1.5f);

	_float3 vUp = _float3(0, 1, 0);

	_float MaxBoundaryRadius = 10;

	_bool   m_bIsUI = false;
	_float  m_bUIDepth = 0;

	_float3 ParticleStartRandomPosMin = _float3(-5.0f, -5.0f, -5.0f);
	_float3 ParticleStartRandomPosMax= _float3(5.f, 5.f, 5.f);

	_bool	DepthTestON = true;
	_bool	AlphaBlendON = true;

	_float	m_fAlphaTestValue = 0.1f;

}PARTICLEDESC;




typedef struct tagUIDesc
{
	_float fX = 0, fY = 0, fCX = 0, fCY = 0;

}UIDESC;



typedef struct tagFloatRect
{
	_float left = 0;
	_float top = 0;
	_float right = 0;
	_float bottom = 0;

}FLOATRECT;

typedef struct tagEditSceneUI {

	UIDESC			UIDesc ;
	FLOATRECT		UIRect ;
	_bool			bIsRect = false;
	_float			fAngle = 0;
	_float			fDepth = 0;
	ID3D11ShaderResourceView* SRV = nullptr;
}EDITUI;


// 전역화함

static bool GetRandomBool()
{
	int random = (rand() % 10000) + 1;
	return 10000 * 0.5 < random;
}
END