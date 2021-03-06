#pragma once
#include "MapObject.h"

BEGIN(Client)

class CShrinkFlower final : public CMapObject
{
private:
	CShrinkFlower(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CShrinkFlower(const CShrinkFlower& rhs);
	virtual ~CShrinkFlower() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj,CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;
	CCollider*			m_pColliderCom = nullptr;


private:
	class CPlayer*		m_pPlayer = nullptr;
	CTransform*			m_pPlayerTransform = nullptr;

private:
	_bool				m_bCanTrapped = true;
	_double				m_TrappedCoolTime = FLT_MAX;

private:
	HRESULT SetUp_Components();


public:
	static CShrinkFlower* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END	