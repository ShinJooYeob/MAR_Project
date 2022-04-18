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



	FAILED_CHECK(Ready_Light());

	FAILED_CHECK(Ready_Layer_MainCamera(TAG_LAY(Layer_Camera_Main)));
	FAILED_CHECK(Ready_Layer_SkyBox(TAG_LAY(Layer_SkyBox)));
	FAILED_CHECK(Ready_Layer_Terrain(TAG_LAY(Layer_Terrain)));
	
	
	FAILED_CHECK(Ready_Layer_Player(TAG_LAY(Layer_Player)));
	FAILED_CHECK(Ready_Layer_UIImage(TAG_LAY(Layer_UI_IMG)));
	

	FAILED_CHECK(Ready_Layer_JumpPad(TAG_LAY(Layer_JumpPad)));
	FAILED_CHECK(Ready_Layer_SteamPad(TAG_LAY(Layer_SteamPad)));
	FAILED_CHECK(Ready_Layer_HiddenPad(TAG_LAY(Layer_HiddenPad)));
	FAILED_CHECK(Ready_Layer_TeethObj(TAG_LAY(Layer_TeethObj)));
	FAILED_CHECK(Ready_Layer_RoseObj(TAG_LAY(Layer_RoseObj)));
	
	
	FAILED_CHECK(Ready_Layer_StaticMapObj(TAG_LAY(Layer_StaticMapObj)));



	FAILED_CHECK(Ready_Layer_Executor(TAG_LAY(Layer_Monster)));
	FAILED_CHECK(Ready_Layer_Eyepot(TAG_LAY(Layer_Monster)));
	FAILED_CHECK(Ready_Layer_Grunt(TAG_LAY(Layer_Monster)));
	FAILED_CHECK(Ready_Layer_WaspArrow(TAG_LAY(Layer_Monster)));
	FAILED_CHECK(Ready_Layer_WaspInk(TAG_LAY(Layer_Monster)));


		
			
			
		
			

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

#ifdef USE_IMGUI
	if (GetKeyState(VK_F1) & 0x8000)
	{
		FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
		FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENEID::SCENE_EDIT), SCENEID::SCENE_LOADING));
	}
#endif // USE_IMGUI



	if (GetKeyState(VK_F2) & 0x8000)
	{
		FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
		FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENEID::SCENE_STAGESELECT), SCENEID::SCENE_LOADING));
	}

	return 0;
}

_int CScene_Loby::Render()
{
	if (__super::Render() < 0)
		return -1;

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("LobyScene"));
#endif // _DEBUG

	return 0;
}

_int CScene_Loby::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

	return 0;
}



HRESULT CScene_Loby::Ready_Light()
{
	LIGHTDESC LightDesc;



	LightDesc.eLightType = tagLightDesc::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(1.f, 1.f,1.f,1.f);
	LightDesc.vAmbient = _float4(1);
	LightDesc.vSpecular = _float4(1);
	LightDesc.vVector = _float4(1, -1, 1, 0);

	g_pGameInstance->Add_Light(LightDesc);



	LightDesc.eLightType = tagLightDesc::TYPE_POINT;
	LightDesc.vDiffuse = _float4(1);
	LightDesc.vAmbient = _float4(1);
	LightDesc.vSpecular = _float4(1);
	LightDesc.vVector = _float4(5, 5, 5, 1);

	g_pGameInstance->Add_Light(LightDesc);

	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_MainCamera(const _tchar * pLayerTag)
{
	CCamera::CAMERADESC CameraDesc;
	CameraDesc.vWorldRotAxis = _float3(0, 0, 0);
	CameraDesc.vEye = _float3(0, 5.f, -10.f);
	CameraDesc.vAt = _float3(0, 0.f, 0);
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


	m_pMainCam = (CCamera_Main*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Camera_Main)));

	if (m_pMainCam == nullptr)
	{
		FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENEID::SCENE_STATIC, pLayerTag, TAG_OP(Prototype_Camera_Main), &CameraDesc));

		m_pMainCam = (CCamera_Main*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Camera_Main)));

		NULL_CHECK_RETURN(m_pMainCam, E_FAIL);

	}
	else 
	{
		m_pMainCam->Set_NowSceneNum(SCENE_LOBY);
	}
	
	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_SkyBox(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_SkyBox)));

	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_Terrain(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_Terrain)));

	CTransform* TerrainTransform = (CTransform*)( g_pGameInstance->Get_Commponent_By_LayerIndex(SCENE_LOBY, pLayerTag, TAG_COM(Com_Transform)));


	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_Player(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STATIC, pLayerTag, TAG_OP(Prototype_Player),&_float3(4,20,4)));

	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_UIImage(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_UIImage)));
	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_JumpPad(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_JumpPad),&_float3(7, 22, 7) ));

	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_SteamPad(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_SteamPad), &_float3(9, 22, 7)));

	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_HiddenPad(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_HiddenPad), &_float3(11, 22, 7)));

	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_TeethObj(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_TeethObj), &_float3(13, 22, 7)));

	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_RoseObj(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_RoseObj), &_float3(14, 22, 7)));

	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_StaticMapObj(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_StaticMapObject)));

	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_Executor(const _tchar * pLayerTag)
{

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_Executor), &_float3( 2, 21, 4)));
	
	

	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_Eyepot(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_Eyepot), &_float3(4, 21, 4)));

	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_Grunt(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_Grunt), &_float3(6, 21, 4)));

	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_WaspArrow(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_WaspArrow), &_float3(8, 21, 4)));

	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_WaspInk(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_WaspInk), &_float3(10, 21, 4)));

	return S_OK;
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
