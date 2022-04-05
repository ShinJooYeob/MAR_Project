#include "stdafx.h"
#include "..\public\Monster.h"



CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CMonster::CMonster(const CMonster & rhs)
	:CGameObject(rhs)
{
}

HRESULT CMonster::Initialize_Prototype(void * pArg)
{
	return S_OK;
}

HRESULT CMonster::Initialize_Clone(void * pArg)
{
	return S_OK;
}

_int CMonster::Update(_double fDeltaTime)
{
	return _int();
}

_int CMonster::LateUpdate(_double fDeltaTime)
{
	return _int();
}

_int CMonster::Render()
{
	return _int();
}

_int CMonster::LateRender()
{
	return _int();
}

void CMonster::Free()
{
	__super::Free();

}
