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

CWeapon::CWeapon(const CWeapon & rhs, _uint C2CChecker)
	: CGameObject(rhs),
	m_tWeaponDesc(rhs.m_tWeaponDesc),
	m_tATBMat(rhs.m_tATBMat),
	m_BoneMatrix(rhs.m_BoneMatrix),
	m_pPlayer(rhs.m_pPlayer)
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
