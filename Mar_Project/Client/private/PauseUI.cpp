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


	FAILED_CHECK(Update_Vopal(fDeltaTime));
	FAILED_CHECK(Update_PapperGrinder(fDeltaTime));
	FAILED_CHECK(Update_Horse(fDeltaTime));
	FAILED_CHECK(Update_Teapot(fDeltaTime));


	for (auto& pUI : m_vecUIContainer)
	{
		NULL_CHECK_RETURN(pUI, E_FAIL);
		if (pUI->Update(fDeltaTime) < 0)
			return -1;
	}

	return _int();
}

_int CPauseUI::LateUpdate(_double fDeltaTime)
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


_int CPauseUI::Render()
{
	if (__super::Render() < 0)
		return -1;

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

	FAILED_CHECK(g_pGameInstance->End_MTV(L"MRT_PaperCurl"));

	FAILED_CHECK(m_pPaperCurlUI->Bind_Texture_OnShader("g_SourTexture", g_pGameInstance->Get_SRV(L"Target_PaperCurl")));

	m_pPaperCurlUI->Render();


	return _int();
}

_int CPauseUI::LightRender()
{
	return _int();
}

void CPauseUI::Start_RollUp(_bool bBool)
{
}

HRESULT CPauseUI::Update_PaperCurl(_double fDeltaTime)
{
	static _bool TurnOn = false;
	if (g_pGameInstance->Get_DIKeyState(DIK_B) & DIS_Down)
	{
		TurnOn = !TurnOn;
		m_pPaperCurlUI->Set_IsDraw(true);
		if (TurnOn)
			m_pPaperCurlUI->Set_TextureLayerIndex_UntilTo(0, 9, 0.5f);
		else
			m_pPaperCurlUI->Set_TextureLayerIndex_UntilTo(9, 0, 0.5f);
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

	if (g_pGameInstance->Get_DIKeyState(DIK_J) & DIS_Down)
	{
		m_bUpgrading[TargetUpgradeID] = true;
		m_fUpgradingPassedTime[TargetUpgradeID] = 0;

		m_pPlayer->Plus_UpgradeCount(TargetUpgradeID);
	}


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

	if (g_pGameInstance->Get_DIKeyState(DIK_J) & DIS_Down)
	{
		m_bUpgrading[TargetUpgradeID] = true;
		m_fUpgradingPassedTime[TargetUpgradeID] = 0;

		m_pPlayer->Plus_UpgradeCount(TargetUpgradeID);
	}

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

	if (g_pGameInstance->Get_DIKeyState(DIK_J) & DIS_Down)
	{
		m_bUpgrading[TargetUpgradeID] = true;
		m_fUpgradingPassedTime[TargetUpgradeID] = 0;

		m_pPlayer->Plus_UpgradeCount(TargetUpgradeID);
	}


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

	if (g_pGameInstance->Get_DIKeyState(DIK_J) & DIS_Down)
	{
		m_bUpgrading[TargetUpgradeID] = true;
		m_fUpgradingPassedTime[TargetUpgradeID] = 0;

		m_pPlayer->Plus_UpgradeCount(TargetUpgradeID);
	}

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
	m_pPaperCurlUI->Set_IsDraw(false);

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
