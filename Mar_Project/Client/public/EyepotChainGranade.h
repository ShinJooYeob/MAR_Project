#pragma once
#include "MonsterWeapon.h"

BEGIN(Client)

class CEyepotChainGranade final :public CMonsterWeapon
{
public:
	typedef struct tagEyepotChainGranadeDesc
	{
		_float3 vPosition;
		_float3 MoveDir;
		_uint MeshKinds;

	}ECGDESC;


private:
	CEyepotChainGranade(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEyepotChainGranade(const CEyepotChainGranade& rhs);
	virtual ~CEyepotChainGranade() = default;

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
	CTexture*			m_pTextureCom = nullptr;

private:
	_double				m_fStartTimer = 0;
	_double				m_fParticleTimer = 0;

	_float				m_RandHeight = 0;
	_bool				m_bGonnabeDie = false;
	_double				m_DyingTime = 0;

	PARTICLEDESC		m_tParticleDesc;


	list<_float3>				m_PassedPositionList;

	ECGDESC		m_tDesc;
private:
	HRESULT SetUp_Components();
	HRESULT Set_Bullet_On_Terrain();
	HRESULT Ready_ParticleDesc();


public:
	static CEyepotChainGranade* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END