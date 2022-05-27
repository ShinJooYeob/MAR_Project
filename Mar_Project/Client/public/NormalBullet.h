#pragma once
#include "Bullet.h"

BEGIN(Client)

class CNormalBullet final : public CBullet
{
private:
	CNormalBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNormalBullet(const CNormalBullet& rhs);
	virtual ~CNormalBullet() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;


	virtual void Set_IsDead()override;
public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;
private:
	PARTICLEDESC		m_tParticleDesc;

private:
	HRESULT Ready_ParticleDesc();

public:
	static CNormalBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END	