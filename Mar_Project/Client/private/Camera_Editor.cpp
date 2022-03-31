#include "stdafx.h"
#include "..\Public\Camera_Editor.h"



CCamera_Editor::CCamera_Editor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CCamera(pDevice,pDeviceContext)
{
}

CCamera_Editor::CCamera_Editor(const CCamera_Editor & rhs)
	:CCamera(rhs)
{
}
//1
HRESULT CCamera_Editor::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));

	return S_OK;
}

HRESULT CCamera_Editor::Initialize_Clone(void * pArg)
{

	FAILED_CHECK(__super::Initialize_Clone(pArg));


	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

_int CCamera_Editor::Update(_double fDeltaTime)
{
	__super::Update(fDeltaTime);

	if (g_pGameInstance->Get_DIKeyState(DIK_W) & DIS_Press)
	{
		m_pTransform->Move_Forward(fDeltaTime);
	}
	if (g_pGameInstance->Get_DIKeyState(DIK_A) & DIS_Press)
	{
		m_pTransform->Move_Left(fDeltaTime);
	}
	if (g_pGameInstance->Get_DIKeyState(DIK_S) & DIS_Press)
	{
		m_pTransform->Move_Backward(fDeltaTime);
	}
	if (g_pGameInstance->Get_DIKeyState(DIK_D) & DIS_Press)
	{
		m_pTransform->Move_Right(fDeltaTime);
	}


	return _int();
}

_int CCamera_Editor::LateUpdate(_double fDeltaTime)
{	

	__super::LateUpdate(fDeltaTime);


	return _int();
}

_int CCamera_Editor::Render()
{



	return _int();
}

_int CCamera_Editor::LateRender()
{


	return _int();
}


HRESULT CCamera_Editor::SetUp_Components()
{

	//if (FAILED(__super::Add_Component(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_ComRenderer)))
	//	return E_FAIL;

	//if (FAILED(__super::Add_Component(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Texture_Blank), TAG_COM(Com_Texture), (CComponent**)&m_ComTexture)))
	//	return E_FAIL;

	//if (FAILED(__super::Add_Component(SCENEID::SCENE_STATIC, TAG_CP(Prototype_VIBuffer_Rect), TAG_COM(Com_VIBuffer), (CComponent**)&m_ComVIBuffer)))
	//	return E_FAIL;



	return S_OK;
}



CCamera_Editor * CCamera_Editor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CCamera_Editor* pInstance = new CCamera_Editor(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg))) {
		MSGBOX("Failed to Create CCamera_Editor");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CCamera_Editor::Clone(void * pArg)
{
	CCamera_Editor* pInstance = new CCamera_Editor(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg))) {
		MSGBOX("Failed to Create CCamera_Editor");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CCamera_Editor::Free()
{
	__super::Free();


}
