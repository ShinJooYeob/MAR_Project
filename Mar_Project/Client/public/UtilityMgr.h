#pragma once

#include "Base.h"

BEGIN(Client)

class CUtilityMgr final :public CBase
{
	DECLARE_SINGLETON(CUtilityMgr)

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

	/*For Particle*/
	HRESULT Create_ParticleObject(_uint eSceneID, PARTICLEDESC tParticleDesc);


public:
	HRESULT Clear_RenderGroup_forSceneChange();
	void Set_Renderer(CRenderer* pRenderer);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	class CMainApp*			m_pMainApp = nullptr;
	CRenderer*				m_pRenderer = nullptr;

	_uint CountDigit(_uint iNum);
public:
	virtual void Free()override;

};

END