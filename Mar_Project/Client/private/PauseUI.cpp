#include "stdafx.h"
#include "..\public\PauseUI.h"
#include "Player.h"


#define WeaponUpgradeCount 5.f


CPauseUI::CPauseUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CPauseUI::CPauseUI(const CPauseUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPauseUI::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));

	return S_OK;
}
HRESULT CPauseUI::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());
	m_fRenderSortValue = -9999999999999.f;
	return S_OK;
}

_int CPauseUI::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	FAILED_CHECK(Update_PaperCurl(fDeltaTime));

	if (m_pPaperCurlUI->Get_NowTextureLayerIndex() != 0)
	{

		FAILED_CHECK(Update_GuideLine(fDeltaTime));

		FAILED_CHECK(Update_Vopal(fDeltaTime));
		FAILED_CHECK(Update_PapperGrinder(fDeltaTime));
		FAILED_CHECK(Update_Horse(fDeltaTime));
		FAILED_CHECK(Update_Teapot(fDeltaTime));


		FAILED_CHECK(Update_Button(fDeltaTime));
		FAILED_CHECK(Update_Fonts(fDeltaTime));




		for (auto& pUI : m_vecUIContainer)
		{
			NULL_CHECK_RETURN(pUI, E_FAIL);
			if (pUI->Update(fDeltaTime) < 0)
				return -1;
		}

	}
	return _int();
}

_int CPauseUI::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_pPaperCurlUI->Get_NowTextureLayerIndex() != 0)
	{
		for (auto& pUI : m_vecUIContainer)
		{
			NULL_CHECK_RETURN(pUI, E_FAIL);

			if (pUI->LateUpdate(fDeltaTime) < 0)
				return -1;
		}
	}
	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this));

	return _int();
}


_int CPauseUI::Render()
{
	if (__super::Render() < 0)
		return -1;
	if (m_pPaperCurlUI->Get_NowTextureLayerIndex() != 0)
	{
		FAILED_CHECK(g_pGameInstance->Begin_MTV(L"MRT_PaperCurl"));

		vector<CUI*> DeptOrderedVector = m_vecUIContainer;

		sort(DeptOrderedVector.begin(), DeptOrderedVector.end(), [](CUI* pSour, CUI* pDest)
		{return (pSour->Get_UIDepth() > pDest->Get_UIDepth()); });

		for (auto& pUI : DeptOrderedVector)
		{
			NULL_CHECK_RETURN(pUI, E_FAIL);

			if (pUI->Render() < 0)
				return -1;
		}

		FAILED_CHECK(Render_Fonts());
		FAILED_CHECK(g_pGameInstance->End_MTV(L"MRT_PaperCurl"));
	}
	FAILED_CHECK(m_pPaperCurlUI->Bind_Texture_OnShader("g_SourTexture", g_pGameInstance->Get_SRV(L"Target_PaperCurl")));
	m_pPaperCurlUI->Render();


	return _int();
}

_int CPauseUI::LightRender()
{
	return _int();
}

HRESULT CPauseUI::Render_Fonts()
{
	CGameInstance* pInstance = g_pGameInstance;

	for (auto& tDesc : m_vecFontDesc)
	{
		pInstance->Render_Font(L"VinerFonts", tDesc.szString.c_str(), tDesc.vPosition, tDesc.vColor, tDesc.fAngle, tDesc.vFontScale);
	}
	m_vecFontDesc.clear();


	return S_OK;
}

void CPauseUI::Start_RollUp(_bool bBool)
{
}

HRESULT CPauseUI::Update_PaperCurl(_double fDeltaTime)
{


	if (!m_PaperCurled && g_bShowMouse && (g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON) & DIS_Down))
	{
		RECT rt;
		rt.left = LONG(0);
		rt.top = LONG(0);
		rt.right = LONG(200);
		rt.bottom = LONG(200);


		POINT ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);


		if (PtInRect(&rt, ptMouse))
		{
			m_pPaperCurlUI->Set_TextureLayerIndex_UntilTo(0, 9, 0.5f);
			m_PaperCurled = true;

			{
				SOUNDDESC tSoundDesc;

				tSoundDesc.vPosition = g_pGameInstance->Get_TargetPostion_float4(PLV_PLAYER);
				tSoundDesc.vMinMax = _float2(1000, 1001);
				tSoundDesc.fTargetSound = 1.f;
				wstring SoundTrack = L"";

				SoundTrack = L"BookPageTurn.mp3";

				g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
			}


		}
	}

	
	return S_OK;

}

HRESULT CPauseUI::Update_Vopal(_double fDeltaTime)
{

	TargetUpgradeID = Upgrade_Vopal;
	StartX = 501.f;
	StartY = 4.f;
	DestX = (StartX - 47.f);
	DestY = (StartY + 359.f);




	if (m_bUpgrading[TargetUpgradeID])
	{

		m_fUpgradingPassedTime[TargetUpgradeID] += (_float)fDeltaTime;

		_int iUpgradeCount = m_pPlayer->Get_UpgradeCount(TargetUpgradeID);

		_float3 EasedPos = g_pGameInstance->Easing_Vector(TYPE_SinInOut,
			_float3(StartX + (WeaponUpgradeCount - _float(iUpgradeCount)) * -6, StartY + _float(WeaponUpgradeCount - _float(iUpgradeCount)) * -8, 0),

			_float3(DestX + _float(_float(iUpgradeCount) - 1.f) * -6, DestY
				+ (_float(iUpgradeCount) - 1)* -8, 180), m_fUpgradingPassedTime[TargetUpgradeID], 1);


		m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_UIDepth(-200 - _float(iUpgradeCount));

		if (m_fUpgradingPassedTime[TargetUpgradeID] > 1)
		{
			m_bUpgrading[TargetUpgradeID] = false;
			EasedPos = _float3(DestX + _float(_float(iUpgradeCount) - 1) * -6, DestY + (_float(iUpgradeCount) - 1)* -8, 180);
		}

		m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_UIDescX(EasedPos.x);
		m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_UIDescY(EasedPos.y);
		m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_YAngle(EasedPos.z);

		if (EasedPos.z > 90 && EasedPos.z < 270) m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_TextureLayerIndex(1);
		else m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_TextureLayerIndex(0);
	}
	else
	{
		_int iUpGrade = m_pPlayer->Get_UpgradeCount(TargetUpgradeID);
		_float3 vPos;
		for (_int i = 1; i <= iUpGrade; i++)
		{
			vPos = _float3(DestX + _float(_float(i) - 1) * -6, DestY + (_float(i) - 1)* -8, 180);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDescX(vPos.x);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDescY(vPos.y);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_YAngle(vPos.z);

			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_TextureLayerIndex(1);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDepth(-200 - _float(i));
		}
		for (_int i = iUpGrade + 1; i <= WeaponUpgradeCount; i++)
		{
			vPos = _float3(StartX + (WeaponUpgradeCount - _float(i)) * -6, StartY + _float(WeaponUpgradeCount - _float(i)) * -8, 0);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDescX(vPos.x);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDescY(vPos.y);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_YAngle(vPos.z);

			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_TextureLayerIndex(0);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDepth(-100 + _float(i));
		}


	}

	return S_OK;
}

HRESULT CPauseUI::Update_PapperGrinder(_double fDeltaTime)
{
	TargetUpgradeID = Upgrade_PaperGrinder;
	StartX = 654.f;
	StartY = 22.f;
	DestX = (StartX - 47.f);
	DestY = (StartY + 359.f);


	if (m_bUpgrading[TargetUpgradeID])
	{

		m_fUpgradingPassedTime[TargetUpgradeID] += (_float)fDeltaTime;

		_int iUpgradeCount = m_pPlayer->Get_UpgradeCount(TargetUpgradeID);

		_float3 EasedPos = g_pGameInstance->Easing_Vector(TYPE_SinInOut,
			_float3(StartX + (WeaponUpgradeCount - _float(iUpgradeCount)) * -6, StartY + _float(WeaponUpgradeCount - _float(iUpgradeCount)) * -8, 0),

			_float3(DestX + _float(_float(iUpgradeCount) - 1.f) * -6, DestY
				+ (_float(iUpgradeCount) - 1)* -8, 180), m_fUpgradingPassedTime[TargetUpgradeID], 1);


		m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_UIDepth(-200 - _float(iUpgradeCount));

		if (m_fUpgradingPassedTime[TargetUpgradeID] > 1)
		{
			m_bUpgrading[TargetUpgradeID] = false;
			EasedPos = _float3(DestX + _float(_float(iUpgradeCount) - 1) * -6, DestY + (_float(iUpgradeCount) - 1)* -8, 180);
		}

		m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_UIDescX(EasedPos.x);
		m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_UIDescY(EasedPos.y);
		m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_YAngle(EasedPos.z);

		if (EasedPos.z > 90 && EasedPos.z < 270) m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_TextureLayerIndex(1);
		else m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_TextureLayerIndex(0);
	}
	else
	{
		_int iUpGrade = m_pPlayer->Get_UpgradeCount(TargetUpgradeID);
		_float3 vPos;
		for (_int i = 1; i <= iUpGrade; i++)
		{
			vPos = _float3(DestX + _float(_float(i) - 1) * -6, DestY + (_float(i) - 1)* -8, 180);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDescX(vPos.x);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDescY(vPos.y);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_YAngle(vPos.z);

			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_TextureLayerIndex(1);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDepth(-200 - _float(i));
		}
		for (_int i = iUpGrade + 1; i <= WeaponUpgradeCount; i++)
		{
			vPos = _float3(StartX + (WeaponUpgradeCount - _float(i)) * -6, StartY + _float(WeaponUpgradeCount - _float(i)) * -8, 0);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDescX(vPos.x);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDescY(vPos.y);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_YAngle(vPos.z);

			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_TextureLayerIndex(0);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDepth(-100 + _float(i));
		}


	}

	return S_OK;
}

HRESULT CPauseUI::Update_Horse(_double fDeltaTime)
{
	TargetUpgradeID = Upgrade_Horse;
	StartX = 807.f;
	StartY = 40.f;
	DestX = (StartX - 47.f);
	DestY = (StartY + 359.f);



	if (m_bUpgrading[TargetUpgradeID])
	{

		m_fUpgradingPassedTime[TargetUpgradeID] += (_float)fDeltaTime;

		_int iUpgradeCount = m_pPlayer->Get_UpgradeCount(TargetUpgradeID);

		_float3 EasedPos = g_pGameInstance->Easing_Vector(TYPE_SinInOut,
			_float3(StartX + (WeaponUpgradeCount - _float(iUpgradeCount)) * -6, StartY + _float(WeaponUpgradeCount - _float(iUpgradeCount)) * -8, 0),

			_float3(DestX + _float(_float(iUpgradeCount) - 1.f) * -6, DestY
				+ (_float(iUpgradeCount) - 1)* -8, 180), m_fUpgradingPassedTime[TargetUpgradeID], 1);


		m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_UIDepth(-200 - _float(iUpgradeCount));

		if (m_fUpgradingPassedTime[TargetUpgradeID] > 1)
		{
			m_bUpgrading[TargetUpgradeID] = false;
			EasedPos = _float3(DestX + _float(_float(iUpgradeCount) - 1) * -6, DestY + (_float(iUpgradeCount) - 1)* -8, 180);
		}

		m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_UIDescX(EasedPos.x);
		m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_UIDescY(EasedPos.y);
		m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_YAngle(EasedPos.z);

		if (EasedPos.z > 90 && EasedPos.z < 270) m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_TextureLayerIndex(1);
		else m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_TextureLayerIndex(0);
	}
	else
	{
		_int iUpGrade = m_pPlayer->Get_UpgradeCount(TargetUpgradeID);
		_float3 vPos;
		for (_int i = 1; i <= iUpGrade; i++)
		{
			vPos = _float3(DestX + _float(_float(i) - 1) * -6, DestY + (_float(i) - 1)* -8, 180);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDescX(vPos.x);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDescY(vPos.y);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_YAngle(vPos.z);

			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_TextureLayerIndex(1);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDepth(-200 - _float(i));
		}
		for (_int i = iUpGrade + 1; i <= WeaponUpgradeCount; i++)
		{
			vPos = _float3(StartX + (WeaponUpgradeCount - _float(i)) * -6, StartY + _float(WeaponUpgradeCount - _float(i)) * -8, 0);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDescX(vPos.x);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDescY(vPos.y);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_YAngle(vPos.z);

			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_TextureLayerIndex(0);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDepth(-100 + _float(i));
		}


	}

	return S_OK;
}

HRESULT CPauseUI::Update_Teapot(_double fDeltaTime)
{
	TargetUpgradeID = Upgrade_Teapot;
	StartX = 960.f;
	StartY = 58.f;
	DestX = (StartX - 47.f);
	DestY = (StartY + 359.f);

	


	if (m_bUpgrading[TargetUpgradeID])
	{

		m_fUpgradingPassedTime[TargetUpgradeID] += (_float)fDeltaTime;

		_int iUpgradeCount = m_pPlayer->Get_UpgradeCount(TargetUpgradeID);

		_float3 EasedPos = g_pGameInstance->Easing_Vector(TYPE_SinInOut,
			_float3(StartX + (WeaponUpgradeCount - _float(iUpgradeCount)) * -6, StartY + _float(WeaponUpgradeCount - _float(iUpgradeCount)) * -8, 0),

			_float3(DestX + _float(_float(iUpgradeCount) - 1.f) * -6, DestY
				+ (_float(iUpgradeCount) - 1)* -8, 180), m_fUpgradingPassedTime[TargetUpgradeID], 1);


		m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_UIDepth(-200 - _float(iUpgradeCount));

		if (m_fUpgradingPassedTime[TargetUpgradeID] > 1)
		{
			m_bUpgrading[TargetUpgradeID] = false;
			EasedPos = _float3(DestX + _float(_float(iUpgradeCount) - 1) * -6, DestY + (_float(iUpgradeCount) - 1)* -8, 180);
		}

		m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_UIDescX(EasedPos.x);
		m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_UIDescY(EasedPos.y);
		m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_YAngle(EasedPos.z);

		if (EasedPos.z > 90 && EasedPos.z < 270) m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_TextureLayerIndex(1);
		else m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(iUpgradeCount))]->Set_TextureLayerIndex(0);
	}
	else
	{
		_int iUpGrade = m_pPlayer->Get_UpgradeCount(TargetUpgradeID);
		_float3 vPos;
		for (_int i = 1; i <= iUpGrade; i++)
		{
			vPos = _float3(DestX + _float(_float(i) - 1) * -6, DestY + (_float(i) - 1)* -8, 180);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDescX(vPos.x);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDescY(vPos.y);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_YAngle(vPos.z);

			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_TextureLayerIndex(1);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDepth(-200 - _float(i));
		}
		for (_int i = iUpGrade + 1; i <= WeaponUpgradeCount; i++)
		{
			vPos = _float3(StartX + (WeaponUpgradeCount - _float(i)) * -6, StartY + _float(WeaponUpgradeCount - _float(i)) * -8, 0);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDescX(vPos.x);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDescY(vPos.y);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_YAngle(vPos.z);

			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_TextureLayerIndex(0);
			m_vecUIContainer[_uint(WeaponUpgradeCount*(TargetUpgradeID + 1) + 1 - _int(i))]->Set_UIDepth(-100 + _float(i));
		}


	}


	return S_OK;
}

HRESULT CPauseUI::Update_GuideLine(_double fDeltaTime)
{
	static _double PassedTime = 0;

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (g_bShowMouse && (g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON) & DIS_Down))
	{



		for (_int i = 0; i< Upgrade_End;i++)
		{

			_int iUpGrade = m_pPlayer->Get_UpgradeCount(UPGRADEID(i));
			_float2 TargetPos = _float2(m_vecUIContainer[_uint(WeaponUpgradeCount*(i + 1) + 1 - _int(iUpGrade))]->Get_UIDesc().fX,
				m_vecUIContainer[_uint(WeaponUpgradeCount*(i + 1) + 1 - _int(iUpGrade))]->Get_UIDesc().fY);

			RECT rt;
			rt.left = LONG(TargetPos.x - 75);
			rt.top = LONG(TargetPos.y - 100);
			rt.right = LONG(TargetPos.x + 75);
			rt.bottom = LONG(TargetPos.y + 100);

			if (PtInRect(&rt, ptMouse))
			{
				m_iUpgradeIndex = i;
				IndexChageTime = 0;

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = g_pGameInstance->Get_TargetPostion_float4(PLV_PLAYER);
					tSoundDesc.vMinMax = _float2(1000, 1001);
					tSoundDesc.fTargetSound = 1.f;
					wstring SoundTrack = L"";

					SoundTrack = L"beep.mp3";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}

				break;
			}
		}

	}


	{
		_float2 TargetPos = _float2(m_vecUIContainer[25]->Get_UIDesc().fX, m_vecUIContainer[25]->Get_UIDesc().fY);

		RECT rt;
		rt.left = LONG(TargetPos.x - 96);
		rt.top = LONG(TargetPos.y - 24);
		rt.right = LONG(TargetPos.x + 96);
		rt.bottom = LONG(TargetPos.y + 24);

		if (PtInRect(&rt, ptMouse))
		{
			m_vecUIContainer[25]->Set_TextureLayerIndex(1);
			if (g_bShowMouse && (g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON) & DIS_Down))
			{
				if (m_pPlayer->Get_UpgradeCount((UPGRADEID)m_iUpgradeIndex) < WeaponUpgradeCount && !m_bUpgrading[m_iUpgradeIndex] && 
					((m_pPlayer->Get_UpgradeCount((UPGRADEID)m_iUpgradeIndex) * 37) <=m_pPlayer->Get_TeethCount())	)
				{
					m_bUpgrading[m_iUpgradeIndex] = true;
					m_fUpgradingPassedTime[m_iUpgradeIndex] = 0;

					m_pPlayer->Plus_iTeethCount(-(m_pPlayer->Get_UpgradeCount((UPGRADEID)m_iUpgradeIndex) * 37));
					m_pPlayer->Plus_UpgradeCount((UPGRADEID)m_iUpgradeIndex);

					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = g_pGameInstance->Get_TargetPostion_float4(PLV_PLAYER);
						tSoundDesc.vMinMax = _float2(1000, 1001);
						tSoundDesc.fTargetSound = 1.f;
						wstring SoundTrack = L"";

						SoundTrack = L"MapObject_pickups_teeth_magic0" + to_wstring(rand() % 3 + 1) + L".ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}

				}
				else
				{
					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = g_pGameInstance->Get_TargetPostion_float4(PLV_PLAYER);
						tSoundDesc.vMinMax = _float2(1000, 1001);
						tSoundDesc.fTargetSound = 1.f;
						wstring SoundTrack = L"";

						SoundTrack = L"MapObject_pickups_teeth0"+to_wstring(rand()%2 + 1)+L".ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}



				}
			}
		}
		else
		{
			m_vecUIContainer[25]->Set_TextureLayerIndex(0);
		}

		TargetPos = _float2(m_vecUIContainer[26]->Get_UIDesc().fX, m_vecUIContainer[26]->Get_UIDesc().fY);

		rt.left = LONG(TargetPos.x - 96);
		rt.top = LONG(TargetPos.y - 24);
		rt.right = LONG(TargetPos.x + 96);
		rt.bottom = LONG(TargetPos.y + 24);

		if (PtInRect(&rt, ptMouse))
		{
			m_vecUIContainer[26]->Set_TextureLayerIndex(1);
			if (g_bShowMouse && (g_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON) & DIS_Down))
			{

				m_vecUIContainer[26]->Set_TextureLayerIndex(0);
					m_pPaperCurlUI->Set_TextureLayerIndex_UntilTo(9, 0, 0.5f);
					m_PaperCurled = false;

					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = g_pGameInstance->Get_TargetPostion_float4(PLV_PLAYER);
						tSoundDesc.vMinMax = _float2(1000, 1001);
						tSoundDesc.fTargetSound = 1.f;
						wstring SoundTrack = L"";

						SoundTrack = L"BookPageTurn.mp3";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}
			}
		}
		else
		{

			m_vecUIContainer[26]->Set_TextureLayerIndex(0);

		}

	}





	if (m_bUpgrading[Upgrade_Vopal] || m_bUpgrading[Upgrade_PaperGrinder] || m_bUpgrading[Upgrade_Horse] || m_bUpgrading[Upgrade_Teapot])
	{
		for (_uint i = 0; i< 4; i++)
		{
			m_vecUIContainer[21 + i]->Set_IsDraw(false);
		}
	}
	else
	{

		PassedTime += fDeltaTime;
		if (PassedTime > 1)		PassedTime = 0;

		_float EasedFloat = g_pGameInstance->Easing_Return(TYPE_QuadInOut, TYPE_QuadInOut, -5, 5, (_float)PassedTime, 1);

		//_float3 EasedColor = g_pGameInstance->Easing_Return_Vector(TYPE_QuadInOut, TYPE_QuadInOut, _float3(0.99609375f, 0.10546875f, 0.08203125f),
		//	_float3(0.99609375f, 0.83203125f, 0.0625f), (_float)PassedTime, 1);
		_float3 EasedColor = g_pGameInstance->Easing_Return_Vector(TYPE_QuadInOut, TYPE_QuadInOut, _float3(0.99609375f, 0.10546875f, 0.08203125f),
			_float3(0.12890625f, 0.1875f, 0.19140625f), (_float)PassedTime, 1);

		_int iUpGrade = m_pPlayer->Get_UpgradeCount((UPGRADEID)m_iUpgradeIndex);

		_float2 TargetPos = _float2(m_vecUIContainer[_uint(WeaponUpgradeCount*(m_iUpgradeIndex + 1) + 1 - _int(iUpGrade))]->Get_UIDesc().fX,
			m_vecUIContainer[_uint(WeaponUpgradeCount*(m_iUpgradeIndex + 1) + 1 - _int(iUpGrade))]->Get_UIDesc().fY);


		for (_uint i = 0; i < 2; i++)
		{

			UIDESC tDesc;

			tDesc.fX = TargetPos.x + ((i % 2) ? 70.f : -65.f);
			tDesc.fY = TargetPos.y + ((i % 2) ? 4.f : -4.f);
			tDesc.fCX = 12;
			tDesc.fCY = 190 + EasedFloat;
			m_vecUIContainer[21 + i]->Apply_UI_To_MemberValue(tDesc);
			m_vecUIContainer[21 + i]->Set_Color(_float4(EasedColor, EasedFloat));
			m_vecUIContainer[21 + i]->Set_IsDraw(true);


			tDesc.fX = TargetPos.x + ((i % 2) ? -10.f : 10.f);
			tDesc.fY = TargetPos.y + ((i % 2) ? 100.f : -103.f);
			tDesc.fCY = 115 + EasedFloat;
			m_vecUIContainer[23 + i]->Apply_UI_To_MemberValue(tDesc);
			m_vecUIContainer[23 + i]->Set_Color(_float4(EasedColor, EasedFloat));
			m_vecUIContainer[23 + i]->Set_IsDraw(true);

		}
	}





	return S_OK;
}

HRESULT CPauseUI::Update_Button(_double fDeltaTime)
{
#define  FontsSize 1.f
	//_float3(0.99609375f, 0.10546875f, 0.08203125f)
	//_float3(0.12890625f, 0.1875f, 0.19140625f)

	{

		FONTSDESC tDesc;
		if (m_pPlayer->Get_UpgradeCount(Upgrade_Vopal) < WeaponUpgradeCount)
		{

			tDesc.vPosition = _float2(447, 157);
			tDesc.szString = to_wstring(m_pPlayer->Get_UpgradeCount(Upgrade_Vopal) * 37);
			tDesc.fAngle = 6;
			tDesc.vColor = ((m_pPlayer->Get_UpgradeCount(Upgrade_Vopal) * 37) <= m_pPlayer->Get_TeethCount()) ? _float4(1) : _float4(0.99609375f, 0.10546875f, 0.08203125f, 1);
			tDesc.vFontScale = _float2(FontsSize);

			m_vecFontDesc.push_back(tDesc);
		}


		tDesc.vPosition = _float2(395, 480);
		if (m_pPlayer->Get_UpgradeCount(Upgrade_Vopal) == WeaponUpgradeCount)
		{
			tDesc.szString = L"LV: Max";
			tDesc.vColor = _float4(0.12890625f, 0.1875f, 0.19140625f, 1);
			tDesc.vFontScale = _float2(FontsSize * 0.8f);
		}
		else
		{
			tDesc.szString = L"LV: " + to_wstring(m_pPlayer->Get_UpgradeCount(Upgrade_Vopal));
			tDesc.vColor = _float4(1);
		}
		tDesc.fAngle = 5;
		tDesc.vFontScale = _float2(FontsSize);

		m_vecFontDesc.push_back(tDesc);
	}
	{

		FONTSDESC tDesc;

		if (m_pPlayer->Get_UpgradeCount(Upgrade_PaperGrinder) < WeaponUpgradeCount)
		{
			tDesc.vPosition = _float2(599, 170);
			tDesc.szString = to_wstring(m_pPlayer->Get_UpgradeCount(Upgrade_PaperGrinder) * 37);
			tDesc.fAngle = 6;
			tDesc.vColor = ((m_pPlayer->Get_UpgradeCount(Upgrade_PaperGrinder) * 37) <= m_pPlayer->Get_TeethCount()) ? _float4(1) : _float4(0.99609375f, 0.10546875f, 0.08203125f, 1);

			tDesc.vFontScale = _float2(FontsSize);

			m_vecFontDesc.push_back(tDesc);
		}


		tDesc.vPosition = _float2(543 ,493);
		if (m_pPlayer->Get_UpgradeCount(Upgrade_PaperGrinder) == WeaponUpgradeCount)
		{
			tDesc.szString = L"LV: Max";
			tDesc.vColor = _float4(0.12890625f, 0.1875f, 0.19140625f, 1);
			tDesc.vFontScale = _float2(FontsSize * 0.8f);
		}
		else
		{
		tDesc.szString = L"LV: " + to_wstring(m_pPlayer->Get_UpgradeCount(Upgrade_PaperGrinder));
		tDesc.vColor = _float4(1);

		}
		tDesc.fAngle = 5;
		tDesc.vFontScale = _float2(FontsSize);

		m_vecFontDesc.push_back(tDesc);
	}
	{

		FONTSDESC tDesc;
		if (m_pPlayer->Get_UpgradeCount(Upgrade_Horse) < WeaponUpgradeCount)
		{
			tDesc.vPosition = _float2(752, 188);
			tDesc.szString = to_wstring(m_pPlayer->Get_UpgradeCount(Upgrade_Horse) * 37);
			tDesc.fAngle = 6;
			tDesc.vColor = ((m_pPlayer->Get_UpgradeCount(Upgrade_Horse) * 37) <= m_pPlayer->Get_TeethCount()) ? _float4(1) : _float4(0.99609375f, 0.10546875f, 0.08203125f, 1);

			tDesc.vFontScale = _float2(FontsSize);

			m_vecFontDesc.push_back(tDesc);
		}

		tDesc.vPosition = _float2(691 ,506);
		if (m_pPlayer->Get_UpgradeCount(Upgrade_Horse) == WeaponUpgradeCount)
		{
			tDesc.szString = L"LV: Max";
			tDesc.vColor = _float4(0.12890625f, 0.1875f, 0.19140625f, 1);
			tDesc.vFontScale = _float2(FontsSize * 0.8f);
		}
		else
		{
			tDesc.szString = L"LV: " + to_wstring(m_pPlayer->Get_UpgradeCount(Upgrade_Horse));
			tDesc.vColor = _float4(1);
		}
		tDesc.fAngle = 5;
		tDesc.vFontScale = _float2(FontsSize);

		m_vecFontDesc.push_back(tDesc);
	}

	{

		FONTSDESC tDesc;
		if (m_pPlayer->Get_UpgradeCount(Upgrade_Teapot) < WeaponUpgradeCount)
		{
			tDesc.vPosition = _float2(907, 203);
			tDesc.szString = to_wstring(m_pPlayer->Get_UpgradeCount(Upgrade_Teapot) * 37);
			tDesc.fAngle = 6;
			tDesc.vColor = ((m_pPlayer->Get_UpgradeCount(Upgrade_Teapot) * 37) <= m_pPlayer->Get_TeethCount()) ? _float4(1) : _float4(0.99609375f, 0.10546875f, 0.08203125f, 1);

			tDesc.vFontScale = _float2(FontsSize);

			m_vecFontDesc.push_back(tDesc);
		}


		tDesc.vPosition = _float2(847, 519);
		if (m_pPlayer->Get_UpgradeCount(Upgrade_Teapot) == WeaponUpgradeCount)
		{
			tDesc.szString = L"LV: Max";
			tDesc.vColor = _float4(0.12890625f, 0.1875f, 0.19140625f, 1);
			tDesc.vFontScale = _float2(FontsSize * 0.8f);
		}
		else
		{
			tDesc.szString = L"LV: " + to_wstring(m_pPlayer->Get_UpgradeCount(Upgrade_Teapot));
			tDesc.vColor = _float4(1);
		}
		tDesc.fAngle = 5;
		tDesc.vFontScale = _float2(FontsSize);

		m_vecFontDesc.push_back(tDesc);
	}



	{

		FONTSDESC tDesc;

		tDesc.vPosition = _float2(286, 120);
		tDesc.szString = to_wstring(m_pPlayer->Get_TeethCount());
		tDesc.fAngle = 6;
		tDesc.vColor = _float4(1);
		tDesc.vFontScale = _float2(1);

		m_vecFontDesc.push_back(tDesc);
	}



	{

		FONTSDESC tDesc;

		//tDesc.vPosition = _float2(395, 545);
		//tDesc.szString = L"Not Enough Tooths to Upgrade";
		//tDesc.fAngle = 5;
		//tDesc.vColor = _float4(0.99609375f, 0.10546875f, 0.08203125f,1);
		//tDesc.vFontScale = _float2(1.2f);
		if (IndexChageTime < 5)
		{
			IndexChageTime += fDeltaTime;
			if (IndexChageTime > 5) IndexChageTime = 5;
		}

		if (m_pPlayer->Get_UpgradeCount((UPGRADEID)m_iUpgradeIndex) == WeaponUpgradeCount)
		{
			tDesc.vPosition = _float2(395, 545);
			tDesc.szString = L"No Way To Upgrade, \nAlready reached the maximum level of enhancement.";
			tDesc.szString = tDesc.szString.substr(0, _uint(tDesc.szString.length() * IndexChageTime / 5.f));

			tDesc.fAngle = 5;
			tDesc.vColor = _float4(0.12890625f, 0.1875f, 0.19140625f, 1);
			tDesc.vFontScale = _float2(0.65f);
			m_vecFontDesc.push_back(tDesc);
		}
		else if ((m_pPlayer->Get_UpgradeCount((UPGRADEID)m_iUpgradeIndex) * 37) > m_pPlayer->Get_TeethCount())
		{
			tDesc.vPosition = _float2(395, 545);
			tDesc.szString = L"Not Enough Teeth to Upgrade";
			tDesc.szString = tDesc.szString.substr(0, _uint(tDesc.szString.length() * IndexChageTime / 5.f));

			

			tDesc.fAngle = 5;
			tDesc.vColor = _float4(0.99609375f, 0.10546875f, 0.08203125f,1);
			tDesc.vFontScale = _float2(1.2f);
			m_vecFontDesc.push_back(tDesc);
		}
		else
		{


			switch (m_iUpgradeIndex)
			{
			case Upgrade_Vopal:
			{
				tDesc.szString = L"A fast attack with focused damage \nthe Vorpal Blade Is reliable in Most situations.";
				tDesc.szString = tDesc.szString.substr(0, _uint(tDesc.szString.length() * IndexChageTime / 5.f));
				tDesc.vPosition = _float2(395, 545);
				tDesc.fAngle = 5;
				tDesc.vColor = _float4(0.12890625f, 0.1875f, 0.19140625f, 1);
				tDesc.vFontScale = _float2(0.65f);

				m_vecFontDesc.push_back(tDesc);
			}
				break;
			case Upgrade_PaperGrinder:
			{
				tDesc.vPosition = _float2(395, 545);
				tDesc.szString = L"the Pepper Grinder can take out enemies from a long range,\n making Alice safer from damage at a distance.";		
				tDesc.szString = tDesc.szString.substr(0, _uint(tDesc.szString.length() * IndexChageTime / 5.f));

				tDesc.fAngle = 5;
				tDesc.vColor = _float4(0.12890625f, 0.1875f, 0.19140625f, 1);
				tDesc.vFontScale = _float2(0.65f);

				m_vecFontDesc.push_back(tDesc);
				}
				break;
			case Upgrade_Horse:
			{
				tDesc.vPosition = _float2(395, 545);
				tDesc.szString = L"Being very heavy, Horse has a lot of power behind it\nwhen it falls down on foes or the ground";
				tDesc.szString = tDesc.szString.substr(0, _uint(tDesc.szString.length() * IndexChageTime / 5.f));

				tDesc.fAngle = 5;
				tDesc.vColor = _float4(0.12890625f, 0.1875f, 0.19140625f, 1);
				tDesc.vFontScale = _float2(0.65f);

				m_vecFontDesc.push_back(tDesc);
			}
				break;
			case Upgrade_Teapot:
			{
				tDesc.vPosition = _float2(395, 545);
				tDesc.szString = L"The Cannon has a lot of power behind it, \nand can damage and kill most enemies with ease in battle.";
				tDesc.szString = tDesc.szString.substr(0, _uint(tDesc.szString.length() * IndexChageTime / 5.f));

				tDesc.fAngle = 5;
				tDesc.vColor = _float4(0.12890625f, 0.1875f, 0.19140625f, 1);
				tDesc.vFontScale = _float2(0.65f);

				m_vecFontDesc.push_back(tDesc);
			}
				break;
			default:
				break;
			}

		}

	}


	return S_OK;
}

HRESULT CPauseUI::Update_Fonts(_double fDeltaTime)
{
	return S_OK;
}


HRESULT CPauseUI::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	CGameInstance* pInstance = g_pGameInstance;

	m_pPlayer = (CPlayer*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Player)));
	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);
	Safe_AddRef(m_pPlayer);

	FAILED_CHECK(Ready_PaperCurl(pInstance));


	FAILED_CHECK(Ready_BarBackGround(pInstance)); //0


	FAILED_CHECK(Ready_Vopal(pInstance)); //1 - 5
	FAILED_CHECK(Ready_PapperGrinder(pInstance)); //6 - 10
	FAILED_CHECK(Ready_Horse(pInstance)); //11 - 15
	FAILED_CHECK(Ready_Teapot(pInstance)); //16 - 20
	FAILED_CHECK(Ready_GuideLine(pInstance)); //21 - 24
	FAILED_CHECK(Ready_Button(pInstance)); //25 - 26



	return S_OK;
}

HRESULT CPauseUI::Ready_PaperCurl(CGameInstance * pInstance)
{



	m_pPaperCurlUI = nullptr;
	FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&m_pPaperCurlUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

	NULL_CHECK_RETURN(m_pPaperCurlUI, E_FAIL);

	FAILED_CHECK(m_pPaperCurlUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_PauseUI), TAG_COM(Com_Texture)));

	FAILED_CHECK(m_pPaperCurlUI->Change_TextureLayer(L"PaperCurl"));
	m_pPaperCurlUI->Set_TextureLayerIndex(0);
	m_pPaperCurlUI->Set_PassIndex(23);
	m_pPaperCurlUI->Set_IsDraw(true);

	UIDESC tUIDesc;

	tUIDesc.fCX = (_float)g_iWinCX;
	tUIDesc.fCY = (_float)g_iWinCY;
	tUIDesc.fX = tUIDesc.fCX*0.5f;
	tUIDesc.fY = tUIDesc.fCY*0.5f;

	m_pPaperCurlUI->Apply_UI_To_MemberValue(tUIDesc);
	m_pPaperCurlUI->Set_DrawingValueIsUIDesc(false);
	FAILED_CHECK(m_pPaperCurlUI->Apply_UIDesc_To_Transform());

	m_pPaperCurlUI->Set_UIDepth(_float(9999999999));



	return S_OK;
}

HRESULT CPauseUI::Ready_BarBackGround(CGameInstance * pInstance)
{//
	{

		CUI* pUI = nullptr;
		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_PauseUI), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"BackGround"));
		pUI->Set_TextureLayerIndex(0);
		pUI->Set_PassIndex(8);
		pUI->Set_IsDraw(true);

		UIDESC tUIDesc;


		tUIDesc.fCX = (_float)g_iWinCX;
		tUIDesc.fCY = (_float)g_iWinCY;
		tUIDesc.fX = tUIDesc.fCX*0.5f;
		tUIDesc.fY = tUIDesc.fCY*0.5f;

		pUI->Apply_UI_To_MemberValue(tUIDesc);
		pUI->Set_DrawingValueIsUIDesc(false);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

		pUI->Set_UIDepth(_float(999999999));

		m_vecUIContainer.push_back(pUI);
	}
	return S_OK;
}

HRESULT CPauseUI::Ready_Vopal(CGameInstance * pInstance)
{
	for (_uint i = 0; i < WeaponUpgradeCount; i++)
	{
		CUI* pUI = nullptr;
		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_PauseUI), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"Vopal"));
		pUI->Set_TextureLayerIndex(0);
		pUI->Set_PassIndex(24);
		pUI->Set_IsDraw(true);

		UIDESC tUIDesc;


		tUIDesc.fX = 571 + _float(i) * -6;
		tUIDesc.fY = 4 + _float(i) * -8;
		tUIDesc.fCX = 130;
		tUIDesc.fCY = 200;

		pUI->Apply_UI_To_MemberValue(tUIDesc);
		pUI->Set_DrawingValueIsUIDesc(true);
		pUI->Set_YTurningOn(true);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

		pUI->Set_Angle(6);
		pUI->Set_UIDepth(-100 - _float(i));

		m_vecUIContainer.push_back(pUI);

	}

	return S_OK;
}

HRESULT CPauseUI::Ready_PapperGrinder(CGameInstance * pInstance)
{
	for (_uint i = 0; i < WeaponUpgradeCount; i++)
	{
		CUI* pUI = nullptr;
		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_PauseUI), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"PapperGrinder"));
		pUI->Set_TextureLayerIndex(0);
		pUI->Set_PassIndex(24);
		pUI->Set_IsDraw(true);

		UIDESC tUIDesc;


		tUIDesc.fX = 724 + _float(i) * -6;
		tUIDesc.fY = 22 + _float(i) * -8;
		tUIDesc.fCX = 130;
		tUIDesc.fCY = 200;

		pUI->Apply_UI_To_MemberValue(tUIDesc);
		pUI->Set_DrawingValueIsUIDesc(true);
		pUI->Set_YTurningOn(true);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

		pUI->Set_Angle(6);
		pUI->Set_UIDepth(_float(-100 - _float(i)));

		m_vecUIContainer.push_back(pUI);

	}
	return S_OK;
}

HRESULT CPauseUI::Ready_Horse(CGameInstance * pInstance)
{
	for (_uint i = 0; i < WeaponUpgradeCount; i++)
	{
		CUI* pUI = nullptr;
		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_PauseUI), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"Horse"));
		pUI->Set_TextureLayerIndex(0);
		pUI->Set_PassIndex(24);
		pUI->Set_IsDraw(true);

		UIDESC tUIDesc;


		tUIDesc.fX = 877 + _float(i) * -6;
		tUIDesc.fY = 40 + _float(i) * -8;
		tUIDesc.fCX = 130;
		tUIDesc.fCY = 200;

		pUI->Apply_UI_To_MemberValue(tUIDesc);
		pUI->Set_DrawingValueIsUIDesc(true);
		pUI->Set_YTurningOn(true);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

		pUI->Set_Angle(6);
		pUI->Set_UIDepth(_float(-100 - _float(i)));

		m_vecUIContainer.push_back(pUI);

	}
	return S_OK;
}

HRESULT CPauseUI::Ready_Teapot(CGameInstance * pInstance)
{
	for (_uint i = 0; i < WeaponUpgradeCount; i++)
	{
		CUI* pUI = nullptr;
		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_PauseUI), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"Teapot"));
		pUI->Set_TextureLayerIndex(0);
		pUI->Set_PassIndex(24);
		pUI->Set_IsDraw(true);

		UIDESC tUIDesc;


		tUIDesc.fX = 1030 + _float(i) * -6;
		tUIDesc.fY = 58 + _float(i) * -8;
		tUIDesc.fCX = 130;
		tUIDesc.fCY = 200;

		pUI->Apply_UI_To_MemberValue(tUIDesc);
		pUI->Set_DrawingValueIsUIDesc(true);
		pUI->Set_YTurningOn(true);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

		pUI->Set_Angle(6);
		pUI->Set_UIDepth(_float(-100 - _float(i)));

		m_vecUIContainer.push_back(pUI);

	}
	return S_OK;
}

HRESULT CPauseUI::Ready_GuideLine(CGameInstance * pInstance)
{


	for (_uint i = 0; i < 2; i++)
	{
		CUI* pUI = nullptr;
		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_PauseUI), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"GuideLine"));
		pUI->Set_TextureLayerIndex(0);
		pUI->Set_PassIndex(25);
		pUI->Set_IsDraw(true);

		UIDESC tUIDesc;


		tUIDesc.fX = 640.f + ((i%2)? 70.f: -70.f);
		tUIDesc.fY = 360.f + ((i % 2) ? 4.f : -4.f);
		tUIDesc.fCX = 12;
		tUIDesc.fCY = 200;

		pUI->Apply_UI_To_MemberValue(tUIDesc);
		pUI->Set_DrawingValueIsUIDesc(true);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

		pUI->Set_Angle(6);
		pUI->Set_UIDepth(_float(-300));

		m_vecUIContainer.push_back(pUI);

	}
	for (_uint i = 0; i < 2; i++)
	{
		CUI* pUI = nullptr;
		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_PauseUI), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"GuideLine"));
		pUI->Set_TextureLayerIndex(0);
		pUI->Set_PassIndex(25);
		pUI->Set_IsDraw(true);

		UIDESC tUIDesc;


		tUIDesc.fX = 640.f + ((i % 2) ? -6.f : 6.f);
		tUIDesc.fY = 360.f + ((i % 2) ? 106.f : -106.f);
		tUIDesc.fCX = 12.f;
		tUIDesc.fCY = 140.f;

		pUI->Apply_UI_To_MemberValue(tUIDesc);
		pUI->Set_DrawingValueIsUIDesc(true);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

		pUI->Set_Angle(96.5f);
		pUI->Set_UIDepth(_float(-300));

		m_vecUIContainer.push_back(pUI);

	}
	return S_OK;
}

HRESULT CPauseUI::Ready_Button(CGameInstance * pInstance)
{
	{
		CUI* pUI = nullptr;
		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_PauseUI), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"Button_UG"));
		pUI->Set_TextureLayerIndex(0);
		pUI->Set_PassIndex(8);
		pUI->Set_IsDraw(true);

		UIDESC tUIDesc;


		tUIDesc.fX = 1100;
		tUIDesc.fY = 600;
		tUIDesc.fCX = 192.f;
		tUIDesc.fCY = 48.f;

		pUI->Apply_UI_To_MemberValue(tUIDesc);
		pUI->Set_DrawingValueIsUIDesc(true);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

		pUI->Set_UIDepth(_float(-300));

		m_vecUIContainer.push_back(pUI);
	}

	{
		CUI* pUI = nullptr;
		FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)(&pUI), m_eNowSceneNum, TAG_OP(Prototype_UIImage)));

		NULL_CHECK_RETURN(pUI, E_FAIL);

		FAILED_CHECK(pUI->Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_Texture_PauseUI), TAG_COM(Com_Texture)));

		FAILED_CHECK(pUI->Change_TextureLayer(L"Button_EX"));
		pUI->Set_TextureLayerIndex(0);
		pUI->Set_PassIndex(8);
		pUI->Set_IsDraw(true);

		UIDESC tUIDesc;


		tUIDesc.fX = 1100;
		tUIDesc.fY = 650;
		tUIDesc.fCX = 192.f;
		tUIDesc.fCY = 48.f;

		pUI->Apply_UI_To_MemberValue(tUIDesc);
		pUI->Set_DrawingValueIsUIDesc(true);
		FAILED_CHECK(pUI->Apply_UIDesc_To_Transform());

		pUI->Set_UIDepth(_float(-300));

		m_vecUIContainer.push_back(pUI);
	}

	return S_OK;
}


CPauseUI * CPauseUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CPauseUI*	pInstance = new CPauseUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CPauseUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPauseUI::Clone(void * pArg)
{
	CGameObject*	pInstance = new CPauseUI(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CGameObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPauseUI::Free()
{
	__super::Free();
	Safe_Release(m_pPaperCurlUI);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pPlayer);



	for (auto& pUi : m_vecUIContainer)
	{
		Safe_Release(pUi);
	}
	m_vecUIContainer.clear();

}
