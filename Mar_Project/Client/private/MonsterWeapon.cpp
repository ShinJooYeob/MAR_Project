#include "stdafx.h"
#include "..\public\MonsterWeapon.h"



CMonsterWeapon::CMonsterWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CMonsterWeapon::CMonsterWeapon(const CMonsterWeapon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonsterWeapon::Initialize_Prototype(void * pArg)
{
	__super::Initialize_Prototype(pArg);
	return S_OK;
}

HRESULT CMonsterWeapon::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));



	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);



	if (pArg != nullptr)
	{
		memcpy(&m_tWeaponDesc, pArg, sizeof(MONWEAPONDESC));
		m_tATBMat = m_tWeaponDesc.pModel->Find_AttachMatrix_InHirarchyNode(m_tWeaponDesc.szHirarchyNodeName);

	}

	return S_OK;
}

HRESULT CMonsterWeapon::Initialize_Clone_Bullet(void * pArg)
{
	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	return S_OK;
}

_int CMonsterWeapon::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	return _int();
}

_int CMonsterWeapon::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
	return -1;
return _int();
}

_int CMonsterWeapon::Render()
{
	return _int();
}

_int CMonsterWeapon::LateRender()
{
	return _int();
}

void CMonsterWeapon::Free()
{
	__super::Free();
}
