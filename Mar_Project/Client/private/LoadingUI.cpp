#include "stdafx.h"
#include "..\public\LoadingUI.h"



CLoadingUI::CLoadingUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CLoadingUI::CLoadingUI(const CLoadingUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLoadingUI::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));

	return S_OK;

}

HRESULT CLoadingUI::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());



	return S_OK;
}

_int CLoadingUI::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;


	FAILED_CHECK(Update_KeyAngle(fDeltaTime));


	for (auto& pUI : m_vecUIContainer)
	{
		NULL_CHECK_RETURN(pUI, E_FAIL);
		if (pUI->Update(fDeltaTime) < 0)
			return -1;
	}


	return _int();
}

_int CLoadingUI::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	for (auto& pUI : m_vecUIContainer)
	{
		NULL_CHECK_RETURN(pUI, E_FAIL);

		if (pUI->LateUpdate(fDeltaTime) < 0)
			return -1;
	}
	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this));

	return _int();
}

_int CLoadingUI::Render()
{

	if (__super::Render() < 0)
		return -1;

	//NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);


	for (auto& pUI : m_vecUIContainer)
	{
		NULL_CHECK_RETURN(pUI, E_FAIL);

		if (pUI->Render() < 0)
			return -1;
	}

	return _int();
}

_int CLoadingUI::LightRender()
{
	return _int();
}

HRESULT CLoadingUI::Update_KeyAngle(_double fDeltaTime)
{
	m_PassedTime += fDeltaTime;

	if (m_bIsRight)
	{
		m_fKeyAngle = g_pGameInstance->Easing_Return(TYPE_CubicOut, TYPE_CubicIn, 0, 30, _float(m_PassedTime));

		if (m_PassedTime >= 1)
		{
			m_bIsRight = !m_bIsRight;
			m_PassedTime = 0;
		}

	}
	else
	{
		m_fKeyAngle = g_pGameInstance->Easing_Return(TYPE_CubicOut, TYPE_CubicIn, 0, -30, _float(m_PassedTime));

		if (m_PassedTime >= 1)
		{
			m_bIsRight = !m_bIsRight;
			m_PassedTime = 0;
		}

	}



	m_vecUIContainer[1]->Set_Angle(m_fKeyAngle);

	return S_OK;
}

HRESULT CLoadingUI::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	CGameInstance* pInstance = g_pGameInstance;
	FAILED_CHECK(Ready_BackGround(pInstance));
	FAILED_CHECK(Ready_Key(pInstance));
	FAILED_CHECK(Ready_Text(pInstance));

	
	sort(m_vecUIContainer.begin(), m_vecUIContainer.end(), [](CUI* pSour, CUI* pDest)->_bool
	{
		return pSour->Get_UIDepth() > pDest->Get_UIDepth();
	});

	return S_OK;
}

HRESULT CLoadingUI::Ready_BackGround(CGameInstance * pInstance)
{
	CUI* pUI = nullptr;
	FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

	NULL_CHECK_RETURN(pUI, E_FAIL);

	FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_LoadingScene), TAG_COM(Com_Texture)));

	FAILED_CHECK(pUI->Change_TextureLayer(L"Loading"));
	pUI->Set_TextureLayerIndex(0);

	UIDESC tUIDesc;

	tUIDesc.fX = g_iWinCX*0.5f;
	tUIDesc.fY = g_iWinCY * 0.5f;
	tUIDesc.fCX = g_iWinCX;
	tUIDesc.fCY = g_iWinCY;

	pUI->Apply_UI_To_MemberValue(tUIDesc);
	FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

	pUI->Set_UIDepth(9999);

	m_vecUIContainer.push_back(pUI);

	return S_OK;
}

HRESULT CLoadingUI::Ready_Key(CGameInstance * pInstance)
{
	CUI* pUI = nullptr;
	FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

	NULL_CHECK_RETURN(pUI, E_FAIL);

	FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_LoadingScene), TAG_COM(Com_Texture)));

	FAILED_CHECK(pUI->Change_TextureLayer(L"Loading"));
	pUI->Set_TextureLayerIndex(1);

	UIDESC tUIDesc;

	tUIDesc.fX = g_iWinCX*0.5f;
	tUIDesc.fY = g_iWinCY * 0.3f;
	tUIDesc.fCY = g_iWinCY * 0.8f;
	tUIDesc.fCX = tUIDesc.fCY * 0.1f;

	pUI->Apply_UI_To_MemberValue(tUIDesc);
	FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

	pUI->Set_UIDepth(8888);

	m_vecUIContainer.push_back(pUI);

	return S_OK;
}

HRESULT CLoadingUI::Ready_Text(CGameInstance * pInstance)
{
	return S_OK;
}

CLoadingUI * CLoadingUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CLoadingUI*	pInstance = new CLoadingUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CLoadingUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLoadingUI::Clone(void * pArg)
{
	CGameObject*	pInstance = new CLoadingUI(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CLoadingUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoadingUI::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);


	for (auto& pUi : m_vecUIContainer)
	{
		Safe_Release(pUi);
	}
	m_vecUIContainer.clear();
}
