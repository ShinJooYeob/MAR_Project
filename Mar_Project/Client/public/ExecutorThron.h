#pragma once
#include "MonsterWeapon.h"

BEGIN(Client)

class CExecutorThron final :public CMonsterWeapon
{
public:
	typedef struct tagThronDesc
	{
		_float3 vPosition;
		_uint MeshKinds;
		_float3 MoveDir;

	}THRONDESC;


private:
	CExecutorThron(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CExecutorThron(const CExecutorThron& rhs);
	virtual ~CExecutorThron() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;


private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;
	CCollider*			m_pColliderCom = nullptr;

private:
	_double				m_fStartTimer = 0;

	_double				m_fTurningTime = 0;


	_bool				m_SummonOther = false;

	THRONDESC		m_tDesc;
private:
	HRESULT SetUp_Components();
	void Add_JumpPower(_float power);


public:
	static CExecutorThron* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END