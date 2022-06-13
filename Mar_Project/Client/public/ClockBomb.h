#pragma once
#include "Weapon.h"

BEGIN(Client)

class CClockBomb final : public CWeapon
{
private:
	CClockBomb(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CClockBomb(const CClockBomb& rhs);
	CClockBomb(const CClockBomb& rhs, _uint Clone2CloneChecker);
	virtual ~CClockBomb() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;
	virtual HRESULT Initialize_Clone2Clone(void* pArg);

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;
	CModel*	Get_WeaponModel() override { return m_pModel; };
	void Set_IsOn() { m_bIsOn = true; };
	_bool Get_IsOn() { return m_bIsOn; };

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;

private:
	_double				m_SpwanPassedTime = 0;
	_double				m_ClockPassedTime = 0;

	_uint				m_iSoundChecker = 0;

	_float				m_TargetTimer = 0;

	_double				m_LevitationTime = 0;
	_bool				m_bIsOn = false;

	_bool				m_bHavetoMaking = true;
	_uint				m_iOldAnimIndex = INT_MAX;
	_uint				m_iAdjMovedIndex = 0;

	PARTICLEDESC		m_tParticleDesc;

	_bool				m_bDeadBomb = false;
	_double				m_DeadPassedTime = 0;
private:
	HRESULT SetUp_Components();
	HRESULT Ready_ParticleDesc();

	HRESULT Adjust_AnimMovedTransform(_double fDeltatime);

public:
	static CClockBomb* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Clone2Clone(void* pArg);
	virtual void Free() override;
};

END	