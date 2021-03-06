#pragma once

#include "Base.h"

BEGIN(Client)

class CUtilityMgr final :public CBase
{
	DECLARE_SINGLETON(CUtilityMgr)

	enum ScreenEffectID
	{
		ScreenEffect_FadeIn,
		ScreenEffect_FadeOut,
		ScreenEffect_FadeInOut,
		ScreenEffect_FadeOutIn,
		ScreenEffect_FadeFlickeringIn,
		ScreenEffect_CamShaking,
		ScreenEffect_HitEffect,
		ScreenEffect_RadialBlur,
		ScreenEffect_END
	};

private:
	explicit CUtilityMgr();
	virtual ~CUtilityMgr() = default;

public:
	HRESULT Initialize_UtilityMgr(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, class CMainApp*	pMainApp);


	/*For Random*/
	_float	RandomFloat(_float Min, _float Max);
	_float3	RandomFloat3(_float Min, _float Max);
	_float3	RandomFloat3(_float3 Min, _float3 Max);


	/*For SlowMotion*/
	void SlowMotionStart(_float fTargetTime = 0.4f, _float TargetSpeed = 0.2f);
	void DistFogRendOn(_bool bBool);

	/*For Particle*/
	HRESULT Create_ParticleObject(_uint eSceneID, PARTICLEDESC tParticleDesc);

	/*For ReflectVector*/

	_Vector ReflectVector(_Vector vDir, _Vector vVerticleVector);
	_Vector SlideVector(_Vector vDir, _Vector vVerticleVector);


	/*For Effect*/
	HRESULT Start_ScreenEffect(ScreenEffectID eEffectType, _double EffectDuration, _float4 AdditionalParameter = _float4(1, 1, 1, 1));


	HRESULT Start_InstanceParticle(_uint eNowSceneNum, _float3 vPosition, _uint iParticleDescIndex = 0);

	HRESULT BindOnShader_DissolveTexture(CShader* pShader, const char* szEadgeRampTexTag, const char* szNoiseTextureTag);


	HRESULT Set_MonsterBlurPos(_float3 vPos);
	HRESULT Set_MonsterBlurOn(_bool	 bBool);

public:
	HRESULT Clear_RenderGroup_forSceneChange();
	void Set_Renderer(CRenderer* pRenderer);




private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	class CMainApp*			m_pMainApp = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CTexture*				m_pTextureCom= nullptr;

	class CFadeEffect*			m_pFadeEffect = nullptr;
	class CRadialBlurMonster*		m_pMonsterRadial = nullptr;
	

	vector<INSTPARTICLEDESC> m_vecInstanceParticleDesc;
private:
	_uint CountDigit(_uint iNum);
	HRESULT Ready_InstanceParticleDesc();
public:
	virtual void Free()override;

};

END