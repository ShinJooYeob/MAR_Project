#pragma once
#include "MonsterWeapon.h"

BEGIN(Client)

class CScrechedBlock final :public CMonsterWeapon
{
public:
	typedef struct tagBreakedGazboDesc
	{
		_float3 vPosition;
		_uint MeshKinds;
		_float3 MoveDir;

	}BREAKEDGAZBODESC;


private:
	CScrechedBlock(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CScrechedBlock(const CScrechedBlock& rhs);
	virtual ~CScrechedBlock() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;


	void Set_Spout();

	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;


private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;
	CCollider*			m_pColliderCom = nullptr;

private:
	_double				m_fStartTimer = 0;
	_double				m_fMaxTimer= 0;
	_bool				m_bIsSpout = false;

	_float				m_fRandPower = 0;
	_float				m_fJumpPower = 0;

	_float3				m_vRotAxis;

	BREAKEDGAZBODESC		m_tDesc;
private:
	HRESULT SetUp_Components();


public:
	static CScrechedBlock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END