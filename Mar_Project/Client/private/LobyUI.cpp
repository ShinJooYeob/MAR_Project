#include "stdafx.h"
#include "..\public\LobyUI.h"
#include "Scene.h"



CLobyUI::CLobyUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CLobyUI::CLobyUI(const CLobyUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLobyUI::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));

	return S_OK;

}

HRESULT CLobyUI::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());


	m_iIndex = 0;
	UIDESC tDesc = m_vecUIContainer[3 + m_iIndex]->Get_UIDesc();

	tDesc.fX -= 84;
	tDesc.fY -= 2;
	tDesc.fCX = 264;
	tDesc.fCY = 114;
	m_vecUIContainer[1]->Apply_UI_To_MemberValue(tDesc);

	if (m_iIndex == 2) m_vecUIContainer[1]->Set_TextureLayerIndex(1);
	else m_vecUIContainer[1]->Set_TextureLayerIndex(0);

	return S_OK;
}

_int CLobyUI::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	CGameInstance* pInstance = g_pGameInstance;


	if (!m_vecUIContainer[0]->Get_NowTextureLayerIndex())
	{
		if (pInstance->Get_DIKeyState(DIK_RETURN)&DIS_Down)
		{
			m_vecUIContainer[0]->Set_TextureLayerIndex(1);

		}
	}
	else
	{
		if (pInstance->Get_DIKeyState(DIK_UP) & DIS_Down) 
		{
			m_iIndex--;
			if (m_iIndex < 0) m_iIndex = 0;

			//for (_uint i = 0; i < 3; i++)
			//{
			//	m_vecUIContainer[3 + i]->Set_TextureLayerIndex(i * 2);
			//}
			//m_vecUIContainer[3 + m_iIndex]->Set_TextureLayerIndex(m_iIndex * 2 + 1);


			UIDESC tDesc =  m_vecUIContainer[3 + m_iIndex]->Get_UIDesc();

			tDesc.fX -= 84;
			tDesc.fY -= 2;
			tDesc.fCX = 264;
			tDesc.fCY = 114;
			m_vecUIContainer[1]->Apply_UI_To_MemberValue(tDesc);

			if (m_iIndex == 2) m_vecUIContainer[1]->Set_TextureLayerIndex(1);
			else m_vecUIContainer[1]->Set_TextureLayerIndex(0);

		}
		if (pInstance->Get_DIKeyState(DIK_DOWN) & DIS_Down)
		{
			m_iIndex++;
			if (m_iIndex > 2) m_iIndex = 2;

			//for (_uint i = 0; i < 3; i++)
			//{
			//	m_vecUIContainer[3 + i]->Set_TextureLayerIndex(i * 2);
			//}
			//m_vecUIContainer[3 + m_iIndex]->Set_TextureLayerIndex(m_iIndex * 2 + 1);

			UIDESC tDesc = m_vecUIContainer[3 + m_iIndex]->Get_UIDesc();

			tDesc.fX -= 84;
			tDesc.fY -= 2;
			tDesc.fCX = 264;
			tDesc.fCY = 114;
			m_vecUIContainer[1]->Apply_UI_To_MemberValue(tDesc);

			if (m_iIndex == 2) m_vecUIContainer[1]->Set_TextureLayerIndex(1);
			else m_vecUIContainer[1]->Set_TextureLayerIndex(0);
		}

		if (pInstance->Get_DIKeyState(DIK_RETURN) & DIS_Down)
		{
			switch (m_iIndex)
			{
			case 0:
				pInstance->Get_NowScene()->Set_SceneChanging(SCENE_STAGE1);

				break;
			case 1:
				pInstance->Get_NowScene()->Set_SceneChanging(SCENE_EDIT);
				break;
			case 2:

				PostQuitMessage(0);

				break;
			default:
				break;
			}

		}

	}





	for (auto& pUI : m_vecUIContainer)
	{
		NULL_CHECK_RETURN(pUI, E_FAIL);
		if (pUI->Update(fDeltaTime) < 0)
			return -1;
	}


	return _int();
}

_int CLobyUI::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	for (auto& pUI : m_vecUIContainer)
	{
		NULL_CHECK_RETURN(pUI, E_FAIL);

		if (pUI->LateUpdate(fDeltaTime) < 0)
			return -1;
	}

	if (!m_vecUIContainer[0]->Get_NowTextureLayerIndex())
	{
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this));

	}
	else
	{
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this));

	}


	return _int();
}

_int CLobyUI::Render()
{

	if (__super::Render() < 0)
		return -1;

	//NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);

	if (!m_vecUIContainer[0]->Get_NowTextureLayerIndex())
	{
		if(m_vecUIContainer[0]->Render() < 0)
			return -1;
	}
	else
	{
		for (auto& pUI : m_vecUIContainer)
		{
			NULL_CHECK_RETURN(pUI, E_FAIL);

			if (pUI->Render() < 0)
				return -1;
		}

	}
	

	return _int();
}

_int CLobyUI::LightRender()
{
	return _int();
}


HRESULT CLobyUI::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	CGameInstance* pInstance = g_pGameInstance;
	FAILED_CHECK(Ready_BackGround(pInstance));
	FAILED_CHECK(Ready_LobyFontGuide(pInstance));
	FAILED_CHECK(Ready_BloodBG(pInstance));
	FAILED_CHECK(Ready_LobyFont(pInstance, 0));
	FAILED_CHECK(Ready_LobyFont(pInstance, 2));
	FAILED_CHECK(Ready_LobyFont(pInstance, 4));

	
	
	sort(m_vecUIContainer.begin(), m_vecUIContainer.end(), [](CUI* pSour, CUI* pDest)->_bool
	{
		return pSour->Get_UIDepth() > pDest->Get_UIDepth();
	});

	return S_OK;
}

HRESULT CLobyUI::Ready_BackGround(CGameInstance * pInstance)
{
	CUI* pUI = nullptr;
	FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

	NULL_CHECK_RETURN(pUI, E_FAIL);

	FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_LobyScene), TAG_COM(Com_Texture)));

	FAILED_CHECK(pUI->Change_TextureLayer(L"BackGround"));
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

HRESULT CLobyUI::Ready_LobyFontGuide(CGameInstance * pInstance)
{
	CUI* pUI = nullptr;
	FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

	NULL_CHECK_RETURN(pUI, E_FAIL);

	FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_LobyScene), TAG_COM(Com_Texture)));

	FAILED_CHECK(pUI->Change_TextureLayer(L"FontGuide"));
	pUI->Set_TextureLayerIndex(0);

	UIDESC tUIDesc;

	tUIDesc.fX = 735;
	tUIDesc.fY = 414;
	tUIDesc.fCX = 672;
	tUIDesc.fCY = 775;

	pUI->Apply_UI_To_MemberValue(tUIDesc);
	FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

	pUI->Set_UIDepth(998);

	m_vecUIContainer.push_back(pUI);

	return S_OK;
}

HRESULT CLobyUI::Ready_LobyFont(CGameInstance * pInstance,_uint iLayerIndex)
{
	CUI* pUI = nullptr;
	FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

	NULL_CHECK_RETURN(pUI, E_FAIL);

	FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_LobyScene), TAG_COM(Com_Texture)));

	FAILED_CHECK(pUI->Change_TextureLayer(L"LobyFont"));
	pUI->Set_TextureLayerIndex(iLayerIndex);

	UIDESC tUIDesc;

	switch (iLayerIndex)
	{
	case 0:
		tUIDesc.fX = 1118;
		tUIDesc.fY = 198;
		break;

	case 2:
		tUIDesc.fX = 1171;
		tUIDesc.fY = 333;
		break;

	case 4:
		tUIDesc.fX = 1162 ;
		tUIDesc.fY = 492;
		break;


	default:
		break;
	}



	tUIDesc.fCX = 394;
	tUIDesc.fCY = 123;

	pUI->Apply_UI_To_MemberValue(tUIDesc);
	FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

	pUI->Set_UIDepth(99);

	m_vecUIContainer.push_back(pUI);



	return S_OK;
}

HRESULT CLobyUI::Ready_BloodBG(CGameInstance * pInstance)
{
	CUI* pUI = nullptr;
	FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

	NULL_CHECK_RETURN(pUI, E_FAIL);

	FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_LobyScene), TAG_COM(Com_Texture)));

	FAILED_CHECK(pUI->Change_TextureLayer(L"FontBloodBG"));
	pUI->Set_TextureLayerIndex(0);

	UIDESC tUIDesc;

	UIDESC TargetDesc = m_vecUIContainer[m_vecUIContainer.size() - 1]->Get_UIDesc();

	tUIDesc.fX = TargetDesc.fX -84;
	tUIDesc.fY = TargetDesc.fY - 2;
	tUIDesc.fCX = 264;
	tUIDesc.fCY = 114;

	pUI->Apply_UI_To_MemberValue(tUIDesc);
	FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

	pUI->Set_UIDepth(999);

	m_vecUIContainer.push_back(pUI);

	return S_OK;
}



CLobyUI * CLobyUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CLobyUI*	pInstance = new CLobyUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CLobyUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLobyUI::Clone(void * pArg)
{
	CGameObject*	pInstance = new CLobyUI(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CLobyUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLobyUI::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);


	for (auto& pUi : m_vecUIContainer)
	{
		Safe_Release(pUi);
	}
	m_vecUIContainer.clear();
}
