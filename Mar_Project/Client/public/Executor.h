#pragma once

#include "Monster.h"

BEGIN(Client)

class CExecutor final : public CMonster
{
private:
	CExecutor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CExecutor(const CExecutor& rhs);
	virtual ~CExecutor() = default;


public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;

	_int		Update_DmgCalculate(_double fDeltaTime);
	virtual _int Update_Pattern(_double fDeltaTime)override;
	virtual void Add_Dmg_to_Monster(_float iDmgAmount)override;
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModel = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

	_bool				m_bIsBerseked = false;
	_bool				m_bIsJumping = false;

	_uint				m_iOldAnimIndex = INT_MAX;
	_uint				m_iAdjMovedIndex = 0;



	_bool				m_bSpwanAnimFinished = false;
	_double				m_SpwanPassedTime = 0;

	_bool				 m_bDeathAnimStart = false;
	_double				m_DeathAnimPassedTime = 0;
	_bool				m_bIsDmgAnimUpdated[3];


	ATTACHBONEMATRIX_PTR m_tCollisionAttachPtr;

	vector<class CMonsterWeapon*>			m_vecWeapon;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_Weapon();

	virtual HRESULT Set_Monster_On_Terrain(CTransform* pTransform, _double fDeltaTime)override;
	HRESULT Adjust_AnimMovedTransform(_double fDeltatime);


public:
	static CExecutor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};


END