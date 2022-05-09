#pragma once

#include "Monster.h"

BEGIN(Client)

class CHandyBoy final : public CMonster
{
private:
	CHandyBoy(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CHandyBoy(const CHandyBoy& rhs);
	virtual ~CHandyBoy() = default;


public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;
	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;

	_int		Update_DmgCalculate(_double fDeltaTime);
	virtual _int Update_Pattern(_double fDeltaTime)override;
	virtual void Add_Dmg_to_Monster(_float iDmgAmount)override;


	void Make_Hand_PoleGrab();
	void Make_Hand_Free();
	void Enter_Hand();
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModel = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

	_bool				m_bIsDmgAnimUpdated[3];

	_uint				m_iOldAnimIndex = INT_MAX;
	_uint				m_iAdjMovedIndex = 0;

	ATTACHBONEMATRIX_PTR m_ArrCollisionAttach[8];

	_bool				m_bIsGrabed = false;

	_float3				m_PoleGrabPos;
	_float3				m_PoleGrabLook;

	_bool				m_bIsDealTime = false;
private:
	HRESULT SetUp_Components();
	HRESULT Adjust_MovedTransform_byAnim(_double fDeltatime);
	HRESULT Update_WanderAround_PatterDelay(_double fDeltatime,_float RandRange = 1.f,_float TurningMixRate = 0.15f);


public:
	static CHandyBoy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};


END