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
	m_bDrawClockBomb = false;


	return S_OK;
}

_int CGamePlayUI::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;



	FAILED_CHECK(Change_WeaponUI());
	FAILED_CHECK(Update_CrossHead());
	FAILED_CHECK(Update_GrinderHUD());
	FAILED_CHECK(Update_TeapotHUD(fDeltaTime));
	FAILED_CHECK(Update_ClockBombHUD(fDeltaTime));
	FAILED_CHECK(Update_FightHUD(fDeltaTime));



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

_int CGamePlayUI::LightRender()
{
	return _int();
}

HRESULT CGamePlayUI::Add_Dmg_to_Player(_uint iNowHP, _int iDmg)
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

void CGamePlayUI::Set_DrawClockBombUI(_bool bBool)
{
	if (m_bDrawClockBomb == bBool) return;

	m_bDrawClockBomb = bBool;
	m_PassedClockBombTime = 0;

	m_vecUIContainer[20]->Set_IsDraw(bBool);
	m_vecUIContainer[21]->Set_IsDraw(bBool);
}

void CGamePlayUI::Set_DrawFightUI(_bool bBool)
{
	m_bFightUIDraw = bBool;
	if (m_bFightUIDraw)
	{
		m_PassedFightUITime = 0;
		m_vecUIContainer[22]->Set_IsDraw(true);
		m_vecUIContainer[23]->Set_IsDraw(true);


		UIDESC tUIDesc;

		tUIDesc.fX = 640;
		tUIDesc.fY = -64;
		tUIDesc.fCX = 1280;
		tUIDesc.fCY = 128;

		m_vecUIContainer[22]->Apply_UI_To_MemberValue(tUIDesc);
		(m_vecUIContainer[22]->Apply_UIDesc_To_Transform());


		tUIDesc.fX = 640;
		tUIDesc.fY = 784;
		tUIDesc.fCX = 1280;
		tUIDesc.fCY = 128;

		m_vecUIContainer[23]->Apply_UI_To_MemberValue(tUIDesc);
		(m_vecUIContainer[23]->Apply_UIDesc_To_Transform());

	}
	else
	{

		m_vecUIContainer[22]->Set_IsDraw(true);
		m_vecUIContainer[23]->Set_IsDraw(true);
		m_PassedFightUITime = 0;


		UIDESC tUIDesc;

		tUIDesc.fX = 640;
		tUIDesc.fY = 64;
		tUIDesc.fCX = 1280;
		tUIDesc.fCY = 128;

		m_vecUIContainer[22]->Apply_UI_To_MemberValue(tUIDesc);
		(m_vecUIContainer[22]->Apply_UIDesc_To_Transform());


		tUIDesc.fX = 640;
		tUIDesc.fY = 656;
		tUIDesc.fCX = 1280;
		tUIDesc.fCY = 128;

		m_vecUIContainer[23]->Apply_UI_To_MemberValue(tUIDesc);
		(m_vecUIContainer[23]->Apply_UIDesc_To_Transform());
	}


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

HRESULT CGamePlayUI::Update_CrossHead()
{

	m_vecUIContainer[12]->Set_Color({1,1,1,m_pPlayer->Get_CharedGauge()});

	return S_OK;
}
HRESULT CGamePlayUI::Update_GrinderHUD()
{

	if (m_pPlayer->Get_NowWeaponIndex() == 1)
	{
		for (_uint i = 0; i < 2; i++)
			m_vecUIContainer[13 + i]->Set_IsDraw(true);
		
		_float CutY = _float(138 - (m_pPlayer->Get_GrinderCoolGauge() / PlayerGrinderCoolTime) * 70);
		m_vecUIContainer[13]->Set_UICutY(CutY);


		if (m_pPlayer->Get_IsNeedToGrinderCooling())
		{
			m_vecUIContainer[15]->Set_IsDraw(true);

			m_vecUIContainer[15]->Set_Color({ 1,1,1,_float(m_pPlayer->Get_GrinderCoolGauge() / PlayerGrinderCoolTime) });

		}
		else
		{
			m_vecUIContainer[15]->Set_IsDraw(false);

		}

	}
	else
	{
		for (_uint i =0 ; i < 3; i++)
		{
			m_vecUIContainer[13 + i]->Set_IsDraw(false);
		}
	}






	return S_OK;
}

HRESULT CGamePlayUI::Update_TeapotHUD(_double fDeltaTime)
{


	if (m_pPlayer->Get_NowWeaponIndex() == 3)
	{
		for (_uint i = 0; i < 3; i++)
			m_vecUIContainer[16 + i]->Set_IsDraw(true);

		_float Gauge = m_pPlayer->Get_CharedGauge();
		_float CutY = _float(133.5f - Gauge * 53.f);
		m_vecUIContainer[17]->Set_UICutY(CutY);


		if (m_pPlayer->Get_IsNeedToTeapotCooling())
		{
			m_vecUIContainer[16]->Set_IsDraw(false);
			m_vecUIContainer[19]->Set_IsDraw(true);

			m_vecUIContainer[19]->Set_Color({ 1,1,1,Gauge });

		}
		else
		{
			m_vecUIContainer[19]->Set_IsDraw(false);

			static _float PassedTime = 0;
			PassedTime += _float(fDeltaTime);
			_float AlphaValue = GetSingle(CGameInstance)->Easing_Return(TYPE_Linear, TYPE_Linear, 0, Gauge, PassedTime, 1);

			if (PassedTime > 1)
			{
				AlphaValue = 0;
				PassedTime = 0;
			}

			m_vecUIContainer[16]->Set_Color({ 1,1,1,AlphaValue });

		}


	}
	else
	{
		for (_uint i = 0; i < 4; i++)
		{
			m_vecUIContainer[16 + i]->Set_IsDraw(false);
		}
	}






	return S_OK;
}


HRESULT CGamePlayUI::Update_ClockBombHUD(_double fDeltaTime)
{
	if (!m_bDrawClockBomb) return S_FALSE;


	m_vecUIContainer[20]->Set_IsDraw(true);
	m_vecUIContainer[21]->Set_IsDraw(true);

	m_PassedClockBombTime += fDeltaTime;
	_float EasedTime = g_pGameInstance->Easing(TYPE_Linear, 0, 360, _float(m_PassedClockBombTime), 5);


	if (m_PassedClockBombTime > 5)
	{
		m_PassedClockBombTime = 0;
		m_bDrawClockBomb = false;
		m_vecUIContainer[20]->Set_IsDraw(false);
		m_vecUIContainer[21]->Set_IsDraw(false);
	}

	m_vecUIContainer[21]->Set_Angle(EasedTime);


	return S_OK;
}

HRESULT CGamePlayUI::Update_FightHUD(_double fDeltaTime)
{
	if (m_PassedFightUITime > 2) return S_FALSE;

	m_PassedFightUITime += fDeltaTime;

	_float EasedHeight = 0;

	if (m_bFightUIDraw)
		EasedHeight = g_pGameInstance->Easing(TYPE_Linear, 0, 128, _float(m_PassedFightUITime), 2);
	else
		EasedHeight = g_pGameInstance->Easing(TYPE_Linear, 128, 0, _float(m_PassedFightUITime), 2);



	if (m_PassedFightUITime > 2)
	{
		EasedHeight = 128;

		if (!m_bFightUIDraw)
		{
			m_vecUIContainer[22]->Set_IsDraw(false);
			m_vecUIContainer[23]->Set_IsDraw(false);
		}
	}

	UIDESC tUIDesc;

	tUIDesc.fX = 640;
	tUIDesc.fY = -64 + EasedHeight;
	tUIDesc.fCX = 1280;
	tUIDesc.fCY = 128;

	m_vecUIContainer[22]->Apply_UI_To_MemberValue(tUIDesc);
	FAILED_CHECK(m_vecUIContainer[22]->Apply_UIDesc_To_Transform());


	tUIDesc.fX = 640;
	tUIDesc.fY = 784 - EasedHeight;
	tUIDesc.fCX = 1280;
	tUIDesc.fCY = 128;

	m_vecUIContainer[23]->Apply_UI_To_MemberValue(tUIDesc);
	FAILED_CHECK(m_vecUIContainer[23]->Apply_UIDesc_To_Transform());



	return S_OK;
}




HRESULT CGamePlayUI::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));


	CGameInstance* pInstance = g_pGameInstance;


	FAILED_CHECK(Ready_Player(pInstance));
	FAILED_CHECK(Ready_HpBarBackGround(pInstance));//0
	FAILED_CHECK(Ready_TeethUI(pInstance));//1
	FAILED_CHECK(Ready_HpFlowerPetal(pInstance));//2~9
	
	FAILED_CHECK(Ready_WeaponChageRing(pInstance));//10
	FAILED_CHECK(Ready_WeaponChageCursor(pInstance));//11
	FAILED_CHECK(Ready_CrossHead(pInstance));//12
	FAILED_CHECK(Ready_WeaponGrinderHUD(pInstance));//13 - 15
	FAILED_CHECK(Ready_WeaponTeapotHUD(pInstance));//16 - 19
	FAILED_CHECK(Ready_WeaponClockBombHUD(pInstance));//20 - 21
	FAILED_CHECK(Ready_FightHUD(pInstance));//22 - 23


	
	
		
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

	tUIDesc.fX	=215;
	tUIDesc.fY	=92	;
	tUIDesc.fCX	=256;
	tUIDesc.fCY	=64;

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

		tUIDesc.fCX = 32;
		tUIDesc.fCY = 32;

		switch (i)
		{
		case 0:

			tUIDesc.fX = 327;
			tUIDesc.fY = 74;
			pUI->Set_Angle(231);
			break;
		case 1:
			tUIDesc.fX = 313;
			tUIDesc.fY = 119;
			pUI->Set_Angle(339);
			break;
		case 2:
			tUIDesc.fX = 294;
			tUIDesc.fY = 73;
			pUI->Set_Angle(220);
			break;
		case 3:
			tUIDesc.fX = 277;
			tUIDesc.fY = 118;
			pUI->Set_Angle(351);
			break;
		case 4:
			tUIDesc.fX = 260;
			tUIDesc.fY = 75;
			pUI->Set_Angle(230);
			break;
		case 5:
			tUIDesc.fX = 244;
			tUIDesc.fY = 118;
			pUI->Set_Angle(358);
			break;
		case 6:
			tUIDesc.fX = 227;
			tUIDesc.fY = 72;
			pUI->Set_Angle(216);
			break;
		case 7:
			tUIDesc.fX = 208;
			tUIDesc.fY = 117;
			pUI->Set_Angle(354);
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

HRESULT CGamePlayUI::Ready_WeaponGrinderHUD(CGameInstance * pInstance)
{

	for (_uint i = 0; i < 3; i++)
	{
		CUI* pUI = nullptr;
		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_GamePlayScene), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"Skill_Grinder"));
		pUI->Set_TextureLayerIndex(i+1);
		if (i == 0)
			pUI->Set_PassIndex(6);
		pUI->Set_IsDraw(false);
		UIDESC tUIDesc;

		tUIDesc.fX = 1175;
		tUIDesc.fY = 103;
		tUIDesc.fCX = 70;
		tUIDesc.fCY = 100;

		pUI->Apply_UI_To_MemberValue(tUIDesc);
		pUI->Set_DrawingValueIsUIDesc(false);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

		pUI->Set_UIDepth(_float(-2 -_int(i)));

		m_vecUIContainer.push_back(pUI);

	}

	return S_OK;
}

HRESULT CGamePlayUI::Ready_WeaponTeapotHUD(CGameInstance * pInstance)
{

	{

		CUI* pUI = nullptr;
		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_GamePlayScene), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"Skill_Teapot"));
		pUI->Set_TextureLayerIndex(0);

		pUI->Set_IsDraw(false);
		UIDESC tUIDesc;

		tUIDesc.fX = 1180;
		tUIDesc.fY = 103;
		tUIDesc.fCX = 120;
		tUIDesc.fCY = 130;

		pUI->Apply_UI_To_MemberValue(tUIDesc);
		pUI->Set_DrawingValueIsUIDesc(false);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

		pUI->Set_UIDepth(_float(-10));

		m_vecUIContainer.push_back(pUI);
	}
	{

		CUI* pUI = nullptr;
		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_GamePlayScene), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"Skill_Teapot"));
		pUI->Set_TextureLayerIndex(1);

		pUI->Set_IsDraw(false);
		pUI->Set_PassIndex(6);
		UIDESC tUIDesc;

		tUIDesc.fX = 1179;
		tUIDesc.fY = 107;
		tUIDesc.fCX = 25;
		tUIDesc.fCY = 53;

		pUI->Apply_UI_To_MemberValue(tUIDesc);
		pUI->Set_DrawingValueIsUIDesc(false);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

		pUI->Set_UIDepth(_float(-10));

		m_vecUIContainer.push_back(pUI);
	}

	for (_uint i = 0; i < 2; i++)
	{
		CUI* pUI = nullptr;
		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_GamePlayScene), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"Skill_Teapot"));
		pUI->Set_TextureLayerIndex(i + 2);
		if (i == 0)
			pUI->Set_PassIndex(6);
		pUI->Set_IsDraw(false);
		UIDESC tUIDesc;

		tUIDesc.fX = 1175;
		tUIDesc.fY = 103;
		tUIDesc.fCX = 70;
		tUIDesc.fCY = 100;

		pUI->Apply_UI_To_MemberValue(tUIDesc);
		pUI->Set_DrawingValueIsUIDesc(false);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

		pUI->Set_UIDepth(_float(-11 - _int(i)));

		m_vecUIContainer.push_back(pUI);

	}

	return S_OK;
}

HRESULT CGamePlayUI::Ready_WeaponClockBombHUD(CGameInstance * pInstance)
{

	{

		CUI* pUI = nullptr;
		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_GamePlayScene), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"Skill_Bomb"));
		pUI->Set_TextureLayerIndex(0);
		pUI->Set_IsDraw(false);

		UIDESC tUIDesc;

		tUIDesc.fX = 1100;
		tUIDesc.fY = 102;
		tUIDesc.fCX = 68;
		tUIDesc.fCY = 130;

		pUI->Apply_UI_To_MemberValue(tUIDesc);
		pUI->Set_DrawingValueIsUIDesc(false);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

		pUI->Set_UIDepth(_float(-20));

		m_vecUIContainer.push_back(pUI);
	}
	{

		CUI* pUI = nullptr;
		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_GamePlayScene), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"Skill_Bomb"));
		pUI->Set_TextureLayerIndex(1);
		pUI->Set_IsDraw(false);
		pUI->Set_PassIndex(6);
		UIDESC tUIDesc;

		tUIDesc.fX = 1100;
		tUIDesc.fY = 131;
		tUIDesc.fCX = 100;
		tUIDesc.fCY = 45;

		pUI->Apply_UI_To_MemberValue(tUIDesc);
		pUI->Set_DrawingValueIsUIDesc(false);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

		pUI->Set_UIDepth(_float(-21));

		m_vecUIContainer.push_back(pUI);
	}


	return S_OK;
}

HRESULT CGamePlayUI::Ready_FightHUD(CGameInstance * pInstance)
{
	{

		CUI* pUI = nullptr;
		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_GamePlayScene), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"FightUI"));
		pUI->Set_TextureLayerIndex(0);
		pUI->Set_PassIndex(8);
		pUI->Set_IsDraw(false);

		UIDESC tUIDesc;

		tUIDesc.fX = 640;
		tUIDesc.fY = 64;
		tUIDesc.fCX = 1280;
		tUIDesc.fCY = 128;

		pUI->Apply_UI_To_MemberValue(tUIDesc);
		pUI->Set_DrawingValueIsUIDesc(false);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

		pUI->Set_UIDepth(_float(-9999999));

		m_vecUIContainer.push_back(pUI);
	}
	{

		CUI* pUI = nullptr;
		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_GamePlayScene), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"FightUI"));
		pUI->Set_TextureLayerIndex(1);
		pUI->Set_IsDraw(false);
		pUI->Set_PassIndex(8);
		UIDESC tUIDesc;

		tUIDesc.fX = 640;
		tUIDesc.fY = 656;
		tUIDesc.fCX = 1280;
		tUIDesc.fCY = 128;

		pUI->Apply_UI_To_MemberValue(tUIDesc);
		pUI->Set_DrawingValueIsUIDesc(false);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

		pUI->Set_UIDepth(_float(-9999999));

		m_vecUIContainer.push_back(pUI);
	}


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
	pUI->Set_IsDraw(true);
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

	tUIDesc.fX = 134;
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
