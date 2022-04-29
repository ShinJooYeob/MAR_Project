#include "stdafx.h"
#include "..\public\GamePlayUI.h"
#include "Player.h"




CGamePlayUI::CGamePlayUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CGamePlayUI::CGamePlayUI(const CGamePlayUI & rhs)
	:CGameObject(rhs)
{
}

HRESULT CGamePlayUI::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));

	return S_OK;
}
HRESULT CGamePlayUI::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());



	return S_OK;
}

_int CGamePlayUI::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;



	FAILED_CHECK(Change_WeaponUI());

	for (auto& pUI : m_vecUIContainer)
	{
		NULL_CHECK_RETURN(pUI, E_FAIL);
		if (pUI->Update(fDeltaTime) < 0)
			return -1;
	}

	return _int();
}

_int CGamePlayUI::LateUpdate(_double fDeltaTime)
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


_int CGamePlayUI::Render()
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




	//FAILED_CHECK(Apply_Rect_To_Transform());
	//FAILED_CHECK(Bind_Transform_OnShader(m_pShaderCom, "g_WorldMatrix"));

	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	//FAILED_CHECK(m_pTextureCom->Bind_OnShader_AutoFrame(m_pShaderCom, "g_DiffuseTexture", g_fDeltaTime));
	////FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture"));

	//FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, 0));

	return _int();
}

_int CGamePlayUI::LateRender()
{
	return _int();
}

HRESULT CGamePlayUI::Add_Dmg_to_Player(_uint iNowHP, _uint iDmg)
{


	CUtilityMgr* pUtil = GetSingle(CUtilityMgr);

	for (_uint i = 9; i >= 2; i--)
	{

		_int TargetTextureIndex = max(_int(iNowHP - ((i - 2) * 4)) , 0);
		
		 _int NowIndex = m_vecUIContainer[i]->Get_NowTextureLayerIndex();


		 if (TargetTextureIndex > 3)
			 TargetTextureIndex = 0;
		 else
		 {
			 TargetTextureIndex = -2 * TargetTextureIndex + 8;
		 }
		 m_vecUIContainer[i]->Set_TextureLayerIndex_UntilTo(NowIndex, TargetTextureIndex, pUtil->RandomFloat(0.3f,0.5f));
	}






	return S_OK;
}

HRESULT CGamePlayUI::Change_WeaponUI()
{
	CGameInstance* pInstance = g_pGameInstance;

	BYTE BtnState = pInstance->Get_DIMouseButtonState(CInput_Device::MBS_WHEEL);
	if (BtnState & DIS_Press)
	{
		if (BtnState & DIS_Down)
		{
			m_vecUIContainer[10]->Set_IsDraw(true);
			m_vecUIContainer[11]->Set_IsDraw(true);

			POINT ptMouse;
			GetCursorPos(&ptMouse);
			ScreenToClient(g_hWnd, &ptMouse);

			m_vOldMousePos = _float2(_float(ptMouse.x), _float(ptMouse.y)).XMVector();

			UIDESC tUIDesc;

			tUIDesc.fX = m_vOldMousePos.x;
			tUIDesc.fY = m_vOldMousePos.y;
			tUIDesc.fCX = 355;
			tUIDesc.fCY = 355;


			m_vecUIContainer[10]->Apply_UI_To_MemberValue(tUIDesc);
			m_vecUIContainer[10]->Apply_UIDesc_To_Transform();

			m_vecUIContainer[11]->Apply_UI_To_MemberValue(tUIDesc);
			m_vecUIContainer[11]->Apply_UIDesc_To_Transform();

		}
		else if (BtnState & DIS_Up)
		{
			m_vecUIContainer[10]->Set_IsDraw(false);
			m_vecUIContainer[11]->Set_IsDraw(false);

			_float Angle = m_vecUIContainer[11]->Get_Angle();
			_uint iTargetWeapon = 0;

			for (_uint i = 1 ; i < 6; i++)
			{
				if (Angle > 30 + (i - 1) * 60 && Angle <= 30 + (i) * 60)
				{
					iTargetWeapon = i;
					break;
				}

			}

			m_pPlayer->Change_Weapon(iTargetWeapon);
		}
		else
		{
			POINT ptMouse;
			GetCursorPos(&ptMouse);
			ScreenToClient(g_hWnd, &ptMouse);
			

			_Vector vBasis = (XMVectorSet(0, 1, 0, 0));

			_Vector vTarget = XMVector3Normalize(XMVectorSet(_float(m_vOldMousePos.x), _float(m_vOldMousePos.y), 0, 0)
								- XMVectorSet(_float(ptMouse.x), _float(ptMouse.y), 0, 0));

			_float Angle = 0;

			if (!XMVectorGetX(XMVectorEqual(vBasis, vTarget)) )
			{
				
				_float fCosAngle = XMVectorGetX(XMVector3Dot(vBasis, vTarget));

				Angle = XMConvertToDegrees(acosf(fCosAngle));

				if (m_vOldMousePos.x > ptMouse.x)
					Angle = 360 - Angle;
			}

			m_vecUIContainer[11]->Set_Angle(Angle);
		}



	}
	return S_OK;
}

HRESULT CGamePlayUI::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));


	CGameInstance* pInstance = g_pGameInstance;


	FAILED_CHECK(Ready_Player(pInstance));
	FAILED_CHECK(Ready_HpBarBackGround(pInstance));
	FAILED_CHECK(Ready_TeethUI(pInstance));
	FAILED_CHECK(Ready_HpFlowerPetal(pInstance));
	
	FAILED_CHECK(Ready_WeaponChageRing(pInstance));
	FAILED_CHECK(Ready_WeaponChageCursor(pInstance));
	FAILED_CHECK(Ready_CrossHead(pInstance));

	
		
	sort(m_vecUIContainer.begin(), m_vecUIContainer.end(), [](CUI* pSour, CUI* pDest)->_bool
	{
		return pSour->Get_UIDepth() > pDest->Get_UIDepth();
	});
	return S_OK;
}

HRESULT CGamePlayUI::Ready_Player(CGameInstance * pInstance)
{
	m_pPlayer = (CPlayer*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);
	Safe_AddRef(m_pPlayer);

	return S_OK;
}

HRESULT CGamePlayUI::Ready_HpBarBackGround(CGameInstance* pInstance)
{
	CUI* pUI = nullptr;
	FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

	NULL_CHECK_RETURN(pUI, E_FAIL);

	FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_GamePlayScene), TAG_COM(Com_Texture)));

	FAILED_CHECK(pUI->Change_TextureLayer(L"HPBar"));

	UIDESC tUIDesc;

	tUIDesc.fX	=178		;
	tUIDesc.fY	=66		;
	tUIDesc.fCX	=	320	;
	tUIDesc.fCY	=		92;

	pUI->Apply_UI_To_MemberValue(tUIDesc);
	FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

	pUI->Set_UIDepth(9999);

	m_vecUIContainer.push_back(pUI);

	return S_OK;
}

HRESULT CGamePlayUI::Ready_HpFlowerPetal(CGameInstance * pInstance)
{

	for (_uint i = 0 ; i < 8 ; i ++)
	{

		CUI* pUI = nullptr;

		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_GamePlayScene), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"HPPatal"));

		UIDESC tUIDesc;

		tUIDesc.fCX = 34;
		tUIDesc.fCY = 33;

		switch (i)
		{
		case 0:

			tUIDesc.fX = 319;
			tUIDesc.fY = 47;
			pUI->Set_Angle(180);
			break;
		case 1:
			tUIDesc.fX = 298;
			tUIDesc.fY = 94;
			break;
		case 2:
			tUIDesc.fX = 277;
			tUIDesc.fY = 45;
			pUI->Set_Angle(180);
			break;
		case 3:
			tUIDesc.fX = 251;
			tUIDesc.fY = 98;
			break;
		case 4:
			tUIDesc.fX = 235;
			tUIDesc.fY = 48;
			pUI->Set_Angle(180);
			break;
		case 5:
			tUIDesc.fX = 212;
			tUIDesc.fY = 96;
			break;
		case 6:
			tUIDesc.fX = 194;
			tUIDesc.fY = 41;
			pUI->Set_Angle(180);
			break;
		case 7:
			tUIDesc.fX = 171;
			tUIDesc.fY = 98;
			break;
		default:
			break;
		}



		pUI->Apply_UI_To_MemberValue(tUIDesc);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());
		pUI->Set_UIDepth(_float(9000 + i));
		m_vecUIContainer.push_back(pUI);

	}

	


	return S_OK;
}

HRESULT CGamePlayUI::Ready_WeaponChageRing(CGameInstance * pInstance)
{

	CUI* pUI = nullptr;
	FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

	NULL_CHECK_RETURN(pUI, E_FAIL);

	FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_GamePlayScene), TAG_COM(Com_Texture)));

	FAILED_CHECK(pUI->Change_TextureLayer(L"QuickSlot"));
	pUI->Set_TextureLayerIndex(0);
	pUI->Set_IsDraw(false);
	UIDESC tUIDesc;

	tUIDesc.fX = 600;
	tUIDesc.fY = 300;
	tUIDesc.fCX = 355;
	tUIDesc.fCY = 355;

	pUI->Apply_UI_To_MemberValue(tUIDesc);
	FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

	pUI->Set_UIDepth(11);

	m_vecUIContainer.push_back(pUI);

	return S_OK;
}

HRESULT CGamePlayUI::Ready_WeaponChageCursor(CGameInstance * pInstance)
{
	CUI* pUI = nullptr;
	FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

	NULL_CHECK_RETURN(pUI, E_FAIL);

	FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_GamePlayScene), TAG_COM(Com_Texture)));

	FAILED_CHECK(pUI->Change_TextureLayer(L"QuickSlot"));
	pUI->Set_TextureLayerIndex(1);
	pUI->Set_IsDraw(false);
	UIDESC tUIDesc;

	tUIDesc.fX = 600;
	tUIDesc.fY = 300;
	tUIDesc.fCX = 355;
	tUIDesc.fCY = 355;

	pUI->Apply_UI_To_MemberValue(tUIDesc);
	FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

	pUI->Set_UIDepth(10);

	m_vecUIContainer.push_back(pUI);

	return S_OK;
}

HRESULT CGamePlayUI::Ready_CrossHead(CGameInstance * pInstance)
{

	CUI* pUI = nullptr;
	FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

	NULL_CHECK_RETURN(pUI, E_FAIL);

	FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_GamePlayScene), TAG_COM(Com_Texture)));

	FAILED_CHECK(pUI->Change_TextureLayer(L"CrossHead"));
	pUI->Set_TextureLayerIndex(0);
	pUI->Set_IsDraw(false);
	UIDESC tUIDesc;

	tUIDesc.fX = g_iWinCX*0.5f;
	tUIDesc.fY = g_iWinCY*0.5f;
	tUIDesc.fCX = 50;
	tUIDesc.fCY = 50;

	pUI->Apply_UI_To_MemberValue(tUIDesc);
	pUI->Set_DrawingValueIsUIDesc(true);

	FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

	pUI->Set_UIDepth(-1);

	m_vecUIContainer.push_back(pUI);
	return S_OK;
}

HRESULT CGamePlayUI::Ready_TeethUI(CGameInstance * pInstance)
{

	CUI* pUI = nullptr;
	FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

	NULL_CHECK_RETURN(pUI, E_FAIL);

	FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_GamePlayScene), TAG_COM(Com_Texture)));

	FAILED_CHECK(pUI->Change_TextureLayer(L"Teeth"));

	UIDESC tUIDesc;

	tUIDesc.fX = 74;
	tUIDesc.fY = 166;
	tUIDesc.fCX = 36;
	tUIDesc.fCY = 53;

	pUI->Apply_UI_To_MemberValue(tUIDesc);
	FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

	pUI->Set_UIDepth(9999);

	m_vecUIContainer.push_back(pUI);

	return S_OK;
}

CGamePlayUI * CGamePlayUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CGamePlayUI*	pInstance = new CGamePlayUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CGamePlayUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGamePlayUI::Clone(void * pArg)
{
	CGameObject*	pInstance = new CGamePlayUI(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CGameObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGamePlayUI::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pPlayer);


	for (auto& pUi : m_vecUIContainer)
	{
		Safe_Release(pUi);
	}
	m_vecUIContainer.clear();

}
