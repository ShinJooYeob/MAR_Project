#pragma once
#include "MonsterWeapon.h"

BEGIN(Client)

class CThrowOilBullet final :public CMonsterWeapon
{
public:
	typedef struct tagBreakedGazboDesc
	{
		_float3 vPosition;
		_uint MeshKinds;
		_float3 MoveDir;

	}BREAKEDGAZBODESC;


private:
	CThrowOilBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CThrowOilBullet(const CThrowOilBullet& rhs);
	virtual ~CThrowOilBullet() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;


	void Set_Position(_float3 vPosition);
	void Set_MovingStart(_float3 vDir);

	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;


private:

	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	PARTICLEDESC		m_tParticleDesc;

private:
	CTransform*			m_pPlayerTransfrom = nullptr;
	_float				m_fStartTimer = 0;
	_bool				m_bIsHavetoMoving = false;

	BREAKEDGAZBODESC		m_tDesc;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ParticleDesc();


public:
	static CThrowOilBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END