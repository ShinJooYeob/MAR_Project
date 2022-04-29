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
	virtual _int LateRender()override;



public:
	static CGrenadeBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END	