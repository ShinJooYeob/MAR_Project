#pragma once

#include "GameObject.h"
#include "Player.h"

BEGIN(Client)
class CMonster abstract : public CGameObject
{

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;


	void Add_Force(CTransform* pTransfrom, _float3 vDir, _float Power);
	void Add_Force_Smooth(CTransform* pTransfrom, _float3 vDir, _float Power,_float TargetTime = 0.2f);
	HRESULT Calculate_Force(_bool* _IsClientQuit, CRITICAL_SECTION* _CriSec);
	HRESULT Calculate_SmoothForce(_bool* _IsClientQuit, CRITICAL_SECTION* _CriSec);

	virtual void Add_Dmg_to_Monster(_float iDmgAmount) {};

protected:
	virtual _int Update_Pattern(_double fDeltaTime);

	_bool Check_Movable_Terrain(CTransform* pTransform, _fVector TargetDir,_float fMovalbeHeight = 1.f);
	virtual HRESULT Set_Monster_On_Terrain(CTransform* pTransform,_double fDeltaTime);

	HRESULT Update_WanderAround(CTransform* pTransform, _double fDeltaTime, _float TurnMixingRate = 0.25f);
	void SetUp_WanderLook(_float3 vLook) {	m_vLookDir = vLook;	};

	_float	Distance_BetweenPlayer(CTransform* pTransform);

protected:
	CPlayer*		m_pPlayer = nullptr;
	CTransform*			m_pPlayerTransfrom = nullptr;
	CGameInstance*		m_pInstance = nullptr;
	_bool				m_bIsPatternFinished = false;
	_uint				m_ePattern = 0;
	_double				m_PatternPassedTime = 0;
	_double				m_PatternDelayTime = 0;

	_float				m_fMaxHP = 0;
	_float				m_fHP = 0;
	_double				m_DmgPassedTime = 0;
	_float				m_fDmgAmount = 0;

	_float3				m_vLookDir;
	_double				m_LevitationTime = 0;
	_bool				m_bIsAddForceActived = false;
	_bool				m_bIsSmoothAddForceActived = false;
private:
	_uint				iWanderCount = 0;
	_float3 m_vAddedForce;
	_float3 m_vSmoothAddedForce;
	_float				m_fSmoothDecrease = 0;
	CTransform* pAFTransfrom = nullptr;
	CTransform* pSAFTransfrom = nullptr;

public:
	virtual void Free() override;
};


END
