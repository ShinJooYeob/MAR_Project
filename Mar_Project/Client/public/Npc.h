#pragma once

#include "GameObject.h"

BEGIN(Client)
class CNpc abstract : public CGameObject
{

private:
	CNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNpc(const CNpc& rhs);
	virtual ~CNpc() = default;

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
