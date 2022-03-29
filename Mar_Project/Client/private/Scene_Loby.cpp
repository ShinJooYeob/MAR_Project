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



	FAILED_CHECK(Ready_Layer_MainCamera(TAG_LAY(Layer_Camera_Main)));
	FAILED_CHECK(Ready_Layer_SkyBox(TAG_LAY(Layer_SkyBox)));
	FAILED_CHECK(Ready_Layer_Terrain(TAG_LAY(Layer_Terrain)));
	
	
	FAILED_CHECK(Ready_Layer_TestObj(TAG_LAY(Layer_Player)));
	FAILED_CHECK(Ready_Layer_UIImage(TAG_LAY(Layer_UI_IMG)));
	
	


	return S_OK;
}

_int CScene_Loby::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;



#ifdef USE_IMGUI
	if (GetKeyState(VK_F1) & 0x8000)
	{
		FAILED_CHECK(GetSingle(CGameInstance)->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENEID::SCENE_EDIT), SCENEID::SCENE_LOADING));
	}
#endif // USE_IMGUI



	if (GetKeyState(VK_F2) & 0x8000)
	{
		FAILED_CHECK(GetSingle(CGameInstance)->Scene_Change(CScene_Loading::Create(m_pDevice,m_pDeviceContext, SCENEID::SCENE_STAGESELECT), SCENEID::SCENE_LOADING));
	}
	
	

	return 0;
}

_int CScene_Loby::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

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


	m_pMainCam = (CCamera_Main*)(GetSingle(CGameInstance)->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Camera_Main)));

	if (m_pMainCam == nullptr)
	{
		FAILED_CHECK(GetSingle(CGameInstance)->Add_GameObject_To_Layer(SCENEID::SCENE_STATIC, pLayerTag, TAG_OP(Prototype_Camera_Main), &CameraDesc));

		m_pMainCam = (CCamera_Main*)(GetSingle(CGameInstance)->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Camera_Main)));

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
	FAILED_CHECK(GetSingle(CGameInstance)->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_SkyBox)));

	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_Terrain(const _tchar * pLayerTag)
{
	FAILED_CHECK(GetSingle(CGameInstance)->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_Terrain)));

	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_TestObj(const _tchar * pLayerTag)
{
	FAILED_CHECK(GetSingle(CGameInstance)->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_Player)));

	return S_OK;
}

HRESULT CScene_Loby::Ready_Layer_UIImage(const _tchar * pLayerTag)
{
	FAILED_CHECK(GetSingle(CGameInstance)->Add_GameObject_To_Layer(SCENE_LOBY, pLayerTag, TAG_OP(Prototype_UIImage)));
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
