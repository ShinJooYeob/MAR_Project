#pragma once
#include "Bullet.h"

BEGIN(Client)

class CGrenadeBullet final : public CBullet
{
private:
	CGrenadeBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGrenadeBullet(const CGrenadeBullet& rhs);
	virtual ~CGrenadeBullet() = default;

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
	class CTerrain*		m_pTerrain = nullptr;

	_bool				m_bIsHit = false;
	_bool				m_bDeadAnimStart = false;
	_double				m_DeadAnimPassedTime = 0;

	_uint				m_iChecker = 0;

	vector<PARTICLEDESC>		m_vecParticleDesc;
private:
	HRESULT Set_Player_On_Terrain();
	HRESULT Ready_ParticleDesc();

public:
	static CGrenadeBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END	