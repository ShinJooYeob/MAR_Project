#pragma once
#include "Weapon.h"

BEGIN(Client)

class CHorse final : public CWeapon
{
private:
	CHorse(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CHorse(const CHorse& rhs);
	virtual ~CHorse() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;


	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;
	CModel*	Get_WeaponModel() override { return m_pModel; };
	void Set_TrailOn(_bool bBool);

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CSwordTrail*		m_pSwordTrailCom = nullptr;
	CSwordTrail*		m_pSubSwordTrailCom = nullptr;

	class CTransform*			m_pPlayerTransform = nullptr;


	_float3				m_TargetColor;
	_float3				m_SourColor;
	_double				m_TrailColorChageTime = 0;
	PARTICLEDESC		m_tParticleDesc;
private:
	HRESULT SetUp_Components();


public:
	static CHorse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END	