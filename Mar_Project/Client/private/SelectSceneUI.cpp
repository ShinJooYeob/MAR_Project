#include "stdafx.h"
#include "..\public\SelectSceneUI.h"
#include "Scene.h"



CSelectSceneUI::CSelectSceneUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CSelectSceneUI::CSelectSceneUI(const CSelectSceneUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSelectSceneUI::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));

	return S_OK;

}

HRESULT CSelectSceneUI::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());


	m_fRenderSortValue = -999.f;

	return S_OK;
}

_int CSelectSceneUI::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	CGameInstance* pInstance = g_pGameInstance;

	if (m_PassedTime < 5)
	{
		m_PassedTime += fDeltaTime;
		if (m_PassedTime > 5)m_PassedTime = 5;
	}

	FAILED_CHECK(Update_Font(fDeltaTime));
	




	return _int();
}

_int CSelectSceneUI::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;


	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this));


	return _int();
}

_int CSelectSceneUI::Render()
{

	if (__super::Render() < 0)
		return -1;

	//NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);


	CGameInstance* pInstance = g_pGameInstance;

	for (auto& tDesc : m_vecFontDesc)
	{
		pInstance->Render_Font(L"VinerFonts", tDesc.szString.c_str(), tDesc.vPosition, tDesc.vColor, tDesc.fAngle, tDesc.vFontScale);
	}
	m_vecFontDesc.clear();


	return _int();
}

_int CSelectSceneUI::LightRender()
{
	return _int();
}


HRESULT CSelectSceneUI::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	CGameInstance* pInstance = g_pGameInstance;
	FAILED_CHECK(Ready_Font(pInstance));

	
	
	return S_OK;
}

HRESULT CSelectSceneUI::Ready_Font(CGameInstance * pInstance)
{
	return S_OK;
}

HRESULT CSelectSceneUI::Update_Font(_double fDeltaTime)
{

	FONTSDESC tDesc;

	tDesc.vPosition = _float2(100, 56.25f);
	//tDesc.vPosition = _float2(395, 545);
	tDesc.szString = L"Find And Follow The Cat, \nTo Go WanderLand.";
	tDesc.szString = tDesc.szString.substr(0, _uint(tDesc.szString.length() * m_PassedTime / 5.f));

	tDesc.fAngle = 0;			
	tDesc.vColor = _float4(1);
	tDesc.vFontScale = _float2(1.f);
	m_vecFontDesc.push_back(tDesc);

	return S_OK;
}



CSelectSceneUI * CSelectSceneUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CSelectSceneUI*	pInstance = new CSelectSceneUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CSelectSceneUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSelectSceneUI::Clone(void * pArg)
{
	CGameObject*	pInstance = new CSelectSceneUI(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CSelectSceneUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSelectSceneUI::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);

}
