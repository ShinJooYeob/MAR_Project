#include "stdafx.h"
#include "..\public\Npc.h"



CNpc::CNpc(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CNpc::CNpc(const CNpc & rhs)
	:CGameObject(rhs)
{
}

HRESULT CNpc::Initialize_Prototype(void * pArg)
{
	return S_OK;
}

HRESULT CNpc::Initialize_Clone(void * pArg)
{
	return S_OK;
}

_int CNpc::Update(_double fDeltaTime)
{
	return _int();
}

_int CNpc::LateUpdate(_double fDeltaTime)
{
	return _int();
}

_int CNpc::Render()
{
	return _int();
}

_int CNpc::LateRender()
{
	return _int();
}

void CNpc::Free()
{
	__super::Free();

}
