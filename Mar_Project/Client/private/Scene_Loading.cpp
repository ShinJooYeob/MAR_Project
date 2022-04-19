#include "stdafx.h"
#include "..\Public\Scene_Loading.h"

#include "Loader.h"
#include "Scene_Loby.h"
#include "Scene_StageSelect.h"
#include "Scene_Stage1.h"


#ifdef USE_IMGUI
#include "Scene_Edit.h"
#endif // USE_IMGUI


#include "Camera_Main.h"






CScene_Loading::CScene_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CScene(pDevice, pDeviceContext)
{
}

HRESULT CScene_Loading::Initialize(SCENEID eSceneID)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	g_pGameInstance->Stop_AllChannel();

	m_eNextSceneIndex = eSceneID;
	m_pLoader = CLoader::Create(m_pDevice,m_pDeviceContext,eSceneID);

	if (FAILED(Ready_Layer_LoadingUI(TEXT("Layer_Loading"))))
		return E_FAIL;




	return S_OK;
}

_int CScene_Loading::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	//m_pLoadingBar->Set_Progress((_float)(m_pLoader->Get_ProgressCount()), (_float)(m_pLoader->Get_MaxCount()));

	return 0;
}

_int CScene_Loading::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;


	//로딩이 끝낫을 경우
	if ( m_pLoader->IsLoadFinished())
	{
		g_pGameInstance->Set_TargetSceneNum(m_eNextSceneIndex);
		switch (m_eNextSceneIndex)
		{
		case SCENEID::SCENE_LOBY:
			FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
			FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loby::Create(m_pDevice, m_pDeviceContext), m_eNextSceneIndex));
			break;

		case SCENEID::SCENE_STAGESELECT:
			FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
			FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_StageSelect::Create(m_pDevice, m_pDeviceContext), m_eNextSceneIndex));
			break;

		case SCENEID::SCENE_STAGE1:
			FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
			FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Stage1::Create(m_pDevice, m_pDeviceContext), m_eNextSceneIndex));
			break;

#ifdef USE_IMGUI
		case SCENEID::SCENE_EDIT:
			FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
			FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Edit::Create(m_pDevice, m_pDeviceContext), m_eNextSceneIndex));
			break;
#endif // USE_IMGUI



		default:
			MSGBOX("Failed to SceneChange");
			break;
		}


	}

	return 0;
}

_int CScene_Loading::Render()
{
	if (__super::Render() < 0)
		return -1;


#ifdef _DEBUG

	if (m_pLoader != nullptr) 
	{
		_int iPercent = int((m_pLoader->Get_ProgressCount() / (float)m_pLoader->Get_MaxCount()) * 100);
		_tchar  szTempString[MAX_PATH];
		_itow_s(iPercent, szTempString, MAX_PATH , 10);
		lstrcat(szTempString,L"% 로딩 중 (Loading Scene)");

		SetWindowText(g_hWnd, szTempString);
	}
#endif // _DEBUG


	return 0;
}

_int CScene_Loading::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

	return 0;
}

HRESULT CScene_Loading::Ready_Layer_LoadingUI(const _tchar * pLayerTag)
{

	FAILED_CHECK(GetSingle(CGameInstance)->Add_GameObject_To_Layer(SCENEID::SCENE_LOADING, pLayerTag, TAG_OP(Prototype_UILoading)));
	
	return S_OK;
}

CScene_Loading * CScene_Loading::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, SCENEID eSceneID)
{
	CScene_Loading* pTempLoadingScene = new CScene_Loading(pDevice, pDeviceContext);

	if (FAILED(pTempLoadingScene->Initialize((eSceneID))))
	{
		Safe_Release(pTempLoadingScene);
		MSGBOX("Failed to Creating CScene_Loading");
		return nullptr;
	}


	return pTempLoadingScene;
}

void CScene_Loading::Free()
{
	__super::Free();
	
	Safe_Release(m_pLoader);
}
