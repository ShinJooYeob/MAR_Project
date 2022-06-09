#pragma once

#include "Monster.h"

BEGIN(Client)

class CEyepot final : public CMonster
{
private:
	CEyepot(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEyepot(const CEyepot& rhs);
	virtual ~CEyepot() = default;


public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;


	virtual HRESULT Set_Monster_On_Terrain(CTransform* pTransform, _double fDeltaTime)override;
	virtual _int Update_Pattern(_double fDeltaTime)override;
	virtual void Add_Dmg_to_Monster(_float iDmgAmount)override;
	_int		Update_DmgCalculate(_double fDeltaTime);

	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;

	void Set_StartSprout() { m_bStartSprout = true; };

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModel = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CTransform*			m_pSubTransformCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;


	_bool				m_bIsDmgAnimUpdated[3];
	_bool				m_bStartPos = false;

				
	_uint				m_iPatternCount = 0;
	_bool				m_bIsJumping = false;
	_bool				m_bIsFarPattern = false;

	_bool				m_bDeathAnimStart = false;
	_bool				m_bDeathDissolveStart = false;
	_float				m_fDissolveTime = 0;
	_bool				m_bStartSprout = false;

	PARTICLEDESC		m_tParticleDesc;


	ATTACHBONEMATRIX_PTR  m_ArrCollisionAttach[2];
private:
	HRESULT SetUp_Components();
	HRESULT FarPatternWander(_double fDeltaTime);
	HRESULT DashPatternWander(_double fDeltaTime);
	HRESULT Ready_ParticleDesc();

public:
	static CEyepot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};


END