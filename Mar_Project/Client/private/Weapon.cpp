#include "stdafx.h"
#include "..\public\Weapon.h"



CWeapon::CWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CWeapon::CWeapon(const CWeapon & rhs)
	:CGameObject(rhs)
{
}

HRESULT CWeapon::Initialize_Prototype(void * pArg)
{
	return S_OK;
}

HRESULT CWeapon::Initialize_Clone(void * pArg)
{
	return S_OK;
}

_int CWeapon::Update(_double fDeltaTime)
{
	return _int();
}

_int CWeapon::LateUpdate(_double fDeltaTime)
{
	return _int();
}

_int CWeapon::Render()
{
	return _int();
}

_int CWeapon::LateRender()
{
	return _int();
}

void CWeapon::Free()
{
	__super::Free();

}
