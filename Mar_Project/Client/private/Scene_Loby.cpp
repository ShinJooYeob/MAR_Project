#include "stdafx.h"
#include "..\Public\Scene_Loby.h"
#include "Scene_Loading.h"

#include "Camera_Main.h"



CScene_Loby::CScene_Loby(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CScene(pDevice,pDeviceContext)
{
}





HRESULT CScene_Loby::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;


	GetSingle(CUtilityMgr)->DistFogRendOn(false);

	FAILED_CHECK(Ready_Light());
	FAILED_CHECK(Ready_Camera(TAG_LAY(Layer_Camera_Main)));
	FAILED_CHECK(Ready_Alice(TAG_LAY(Layer_Player)));

	FAILED_CHECK(Ready_Layer_UI(TAG_LAY(Layer_UI_IMG)));

	
		

	GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeIn, 0.5, { 0,0,0,1 });
	FAILED_CHECK(g_pGameInstance->PlayBGM(L"BGM_STAGE_0.ogg", 0.1f));


	return S_OK;
}

_int CScene_Loby::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;



	

	return 0;
}

_int CScene_Loby::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsNeedToSceneChange)	return Change_to_NextScene();


	static _uint iTrackIndex = 0;

	if (g_pGameInstance->Get_DIKeyState(DIK_1)&DIS_Down)
	{
		iTrackIndex++;
		if (iTrackIndex > 8)iTrackIndex = 0;

		//wstring TrackName = L"BGM_STAGE_3.mp3";
		wstring TrackName = L"BGM_STAGE_" + to_wstring(iTrackIndex) + L".ogg";

		g_pGameInstance->PlayBGM(TrackName.c_str(), 0.1f);



	}
	if (g_pGameInstance->Get_DIKeyState(DIK_2)&DIS_Down)
	{
		iTrackIndex++;
		if (iTrackIndex > 4)iTrackIndex = 0;

		//wstring TrackName = L"BGM_STAGE_3.mp3";
		wstring TrackName = L"BGM_FIGHT_" + to_wstring(iTrackIndex) + L".ogg";

		g_pGameInstance->PlayBGM(TrackName.c_str(), 0.1f);

	}


//#ifdef USE_IMGUI
//	if (GetKeyState(VK_F1) & 0x8000)
//	{
//		FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
//		FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENEID::SCENE_EDIT), SCENEID::SCENE_LOADING));
//	}
//#endif // USE_IMGUI
//
//
//
//	if (GetKeyState(VK_F2) & 0x8000)
//	{
//		FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
//		FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENEID::SCENE_STAGE1), SCENEID::SCENE_LOADING));
//	}
//
	return 0;
}

_int CScene_Loby::Render()
{
	if (__super::Render() < 0)
		return -1;

#ifdef _DEBUG
	if (!g_bIsShowFPS)
		SetWindowText(g_hWnd, TEXT("LobyScene"));
#endif // _DEBUG

	return 0;
}

_int CScene_Loby::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	return 0;
}

_int CScene_Loby::Change_to_NextScene()
{


	switch (m_eNextScene)
	{
	case  SCENE_STAGE1:
		FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
		FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENEID::SCENE_STAGESELECT), SCENEID::SCENE_LOADING));
		break;


	case  SCENE_EDIT:
#ifdef USE_IMGUI
			FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
			FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENEID::SCENE_EDIT), SCENEID::SCENE_LOADING));
		
#endif // USE_IMGUI
		break;

	default:
		break;
	}

	return _int();
}



HRESULT CScene_Loby::Ready_Light()
{
	const LIGHTDESC* pLightDesc = g_pGameInstance->Get_LightDesc(tagLightDesc::TYPE_DIRECTIONAL, 0);

	if (pLightDesc == nullptr)
	{

		LIGHTDESC LightDesc;

		LightDesc.eLightType = tagLightDesc::TYPE_DIRECTIONAL;
		LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vAmbient = _float4(1.0f);
		LightDesc.vSpecular = _float4(1);
		LightDesc.vVector = _float4(0, 256, -128, 0);

		g_pGameInstance->Add_Light(LightDesc);
	}
	else
	{
		g_pGameInstance->Relocate_LightDesc(tagLightDesc::TYPE_DIRECTIONAL, 0, _float4(0, 256, -128, 0).XMVector());
	}




	//LightDesc.eLightType = tagLightDesc::TYPE_POINT;
	//LightDesc.vDiffuse = _float4(1);
	//LightDesc.vAmbient = _float4(1);
	//LightDesc.vSpecular = _float4(1);
	//LightDesc.vVector = _float4(5, 5, 5, 1);
	//LightDesc.fRange = 10;
	//g_pGameInstance->Add_Light(LightDesc);

	return S_OK;
}

HRESULT CScene_Loby::Ready_Camera(const _tchar* pLayerTag)
{
	CCamera::CAMERADESC CameraDesc;
	CameraDesc.vWorldRotAxis = _float3(0, 0, 0);
	CameraDesc.vEye = _float3(0.125f, 1.3f, -0.8f);
	CameraDesc.vAt = _float3(0.225f, 1.3f, 0);
	CameraDesc.vAxisY = _float3(0, 1, 0);

	CameraDesc.fFovy = XMConvertToRadians(60.f);
	CameraDesc.fAspect = _float(g_iWinCX) / g_iWinCY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.f;

	CameraDesc.iWinCX = g_iWinCX;
	CameraDesc.iWinCY = g_iWinCY;

	CameraDesc.TransformDesc.fMovePerSec = 5.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(60.0f);
	CameraDesc.TransformDesc.fScalingPerSec = 1.f;


	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENEID::SCENE_LOBY, pLayerTag, TAG_OP(Prototype_Camera_Main), &CameraDesc));

	m_pMainCam = (CCamera_Main*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_LOBY, TAG_LAY(Layer_Camera_Main)));

	NULL_CHECK_RETURN(m_pMainCam, E_FAIL);

	


	return S_OK;
}

HRESULT CScene_Loby::Ready_Alice(const _tchar* pLayerTag)
{

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENEID::SCENE_LOBY, pLayerTag, TAG_OP(Prototype_PlayerLoby)));

	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_UI(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENEID::SCENE_LOBY, pLayerTag, TAG_OP(Prototype_UILoby)));

	return S_OK	;
}


CScene_Loby * CScene_Loby::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CScene_Loby* pTempLoby = new CScene_Loby(pDevice,pDeviceContext);

	if (FAILED(pTempLoby->Initialize())) 
	{
		Safe_Release(pTempLoby); 
		MSGBOX("Failed to Creating CScene_Loby");
		return nullptr;
	}
	return pTempLoby;

}

void CScene_Loby::Free()
{
	__super::Free();
}
