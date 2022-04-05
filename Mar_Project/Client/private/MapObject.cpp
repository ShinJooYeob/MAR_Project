#include "stdafx.h"
#include "..\public\MapObject.h"



CMapObject::CMapObject(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CMapObject::CMapObject(const CMapObject & rhs)
	:CGameObject(rhs)
{
}

HRESULT CMapObject::Initialize_Prototype(void * pArg)
{
	return S_OK;
}

HRESULT CMapObject::Initialize_Clone(void * pArg)
{
	return S_OK;
}

_int CMapObject::Update(_double fDeltaTime)
{
	return _int();
}

_int CMapObject::LateUpdate(_double fDeltaTime)
{
	return _int();
}

_int CMapObject::Render()
{
	return _int();
}

_int CMapObject::LateRender()
{
	return _int();
}

void CMapObject::Free()
{
	__super::Free();

}
