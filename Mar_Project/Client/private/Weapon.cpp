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
	__super::Initialize_Prototype(pArg);
	return S_OK;
}

HRESULT CWeapon::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	return S_OK;
}

_int CWeapon::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	return _int();
}

_int CWeapon::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;
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
