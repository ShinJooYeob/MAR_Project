#pragma once

#include "Monster.h"

BEGIN(Client)

class CWaspInk final : public CMonster
{
private:
	CWaspInk(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CWaspInk(const CWaspInk& rhs);
	virtual ~CWaspInk() = default;


public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;


	_int		Update_DmgCalculate(_double fDeltaTime);
	virtual _int Update_Pattern(_double fDeltaTime)override;
	virtual void Add_Dmg_to_Monster(_float iDmgAmount)override;

	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;


private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModel = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

	_bool				m_bSpwanAnimFinished = false;
	_double				m_SpwanAnimPassedTime = 0;
	_uint				m_SpwanAnimCount = 0;
	_uint				m_SpwanKinds = INT_MAX;
	_float3				m_SpwanMovingPos[5];



	_bool				m_bIsDmgAnimUpdated[3];

	_bool				m_bDeadAnimStart = false;

	_bool				m_bDeathDissolveStart = false;
	_float				m_fDissolveTime = 0;

	_uint				m_iOldAnimIndex = INT_MAX;
	_uint				m_iAdjMovedIndex = 0;

	_float				m_fTargetSound = 0.55f;
	SOUNDDESC*			m_pBuzzSoundDesc = nullptr;
private:
	HRESULT SetUp_Components();

	HRESULT NearPatternWander(_double fDeltaTime);
	HRESULT Adjust_AnimMovedTransform(_double fDeltatime);



public:
	static CWaspInk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};


END