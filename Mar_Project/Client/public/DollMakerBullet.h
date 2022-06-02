#pragma once
#include "MonsterWeapon.h"

BEGIN(Client)

class CDollMakerBullet final :public CMonsterWeapon
{
public:
	typedef struct tagHandyGirlBulletDesc
	{
		_float3 vPosition;
		_float3 vMoveDir;

	}HANDYGRILBULLETDESC;


private:
	CDollMakerBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CDollMakerBullet(const CDollMakerBullet& rhs);
	virtual ~CDollMakerBullet() = default;

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


	vector<PARTICLEDESC>		m_vecParticleDesc;


private:
	CTransform*			m_pPlayerTransfrom = nullptr;

	_double				m_TargetTime = 0;
	_double				m_fStartTimer = 0;
	_double				m_DeathTimer = 0;
	_bool				m_bGoingtoDeath = false;
	_float				m_RandScale = 3;


	HANDYGRILBULLETDESC		m_tDesc;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ParticleDesc();


public:
	static CDollMakerBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END