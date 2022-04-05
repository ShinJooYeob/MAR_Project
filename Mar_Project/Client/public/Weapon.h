#pragma once

#include "GameObject.h"

BEGIN(Client)
class CWeapon abstract : public CGameObject
{

private:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;


public:
	virtual void Free() override;
};


END
