#pragma once
#include "MonsterWeapon.h"

BEGIN(Client)

class CChainExplosion final :public CMonsterWeapon
{
public:
	typedef struct tagThronDesc
	{
		_float3 vPosition;
		_uint MeshKinds;
		_float3 MoveDir;

	}THRONDESC;


private:
	CChainExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CChainExplosion(const CChainExplosion& rhs);
	virtual ~CChainExplosion() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;

	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;


private:
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

#ifdef _DEBUG
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
#endif // _DEBUG

private:
	_double				m_fStartTimer = 0;
	_double				m_fTurningTime = 0;

	PARTICLEDESC		m_tParticleDesc;
	_bool				m_SummonOther = false;

	THRONDESC		m_tDesc;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ParticleDesc();


public:
	static CChainExplosion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END