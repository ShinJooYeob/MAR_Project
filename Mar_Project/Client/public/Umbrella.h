#pragma once
#include "Weapon.h"

BEGIN(Client)

class CUmbrella final : public CWeapon
{
private:
	CUmbrella(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUmbrella(const CUmbrella& rhs);
	virtual ~CUmbrella() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj,CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;
	CModel*	Get_WeaponModel() override { return m_pModel; };
	_float3 Get_ColliderPosition();
	virtual void Set_AttackAble(_bool bBool = true);
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;
	CCollider*			m_pColliderCom = nullptr;


	_bool				m_TempAttackAble = false;
	_double				m_ATABPassedTime = 0;
private:
	HRESULT SetUp_Components();


public:
	static CUmbrella* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END	