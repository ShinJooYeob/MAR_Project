#include "stdafx.h"
#include "..\public\Weapon.h"
#include "Player.h"



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



	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	Safe_AddRef(m_pPlayer);


	if (pArg != nullptr)
	{
		memcpy(&m_tWeaponDesc, pArg, sizeof(WEAPONDESC));
		m_tATBMat = m_tWeaponDesc.pModel->Find_AttachMatrix_InHirarchyNode(m_tWeaponDesc.szHirarchyNodeName);

	}

	


	return S_OK;
}

HRESULT CWeapon::Initialize_Clone_Bullet(void * pArg)
{

	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	Safe_AddRef(m_pPlayer);


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
	Safe_Release(m_pPlayer);

}
