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




	return S_OK;
}

_int CScene_Loby::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;



	// IMGUI / ¸Ê Åø Å×½ºÆ®¾ÀÀ¸·Î »ç¿ë
	if (GetKeyState(VK_F1) & 0x8000)
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
	CAMERADESC CameraDesc;

	//CameraDesc.bIsOrtho = true;
	//CameraDesc.vEye = _float3(0.f, 0.f, -10.f);
	//CameraDesc.vWorldRotAxis = _float3(0.f, 0.f, 0.f);
	//CameraDesc.vAxisY = _float3(0, 1, 0);
	//CameraDesc.fFovy = D3DXToRadian(60.0f);
	//CameraDesc.fAspect = _float(g_iWinCX) / g_iWinCY;
	//CameraDesc.fNear = 0.2f;
	//CameraDesc.fFar = 300.f;

	//CameraDesc.TransformDesc.fMovePerSec = 10.f;
	//CameraDesc.TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	//m_pMainCam = (CCamera_Main*)(GetSingle(CGameInstance)->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Camera_Main)));

	//if (m_pMainCam == nullptr)
	//{
	//	if (GetSingle(CGameInstance)->Add_GameObject_To_Layer(SCENEID::SCENE_STATIC, pLayerTag, TAG_OP(Prototype_Camera_Main), &CameraDesc))
	//		return E_FAIL;

	//	m_pMainCam = (CCamera_Main*)(GetSingle(CGameInstance)->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Camera_Main)));
	//	if (m_pMainCam == nullptr)
	//		return E_FAIL;
	//}
	//else 
	//{

	//	if (FAILED(m_pMainCam->Reset_LookAtAxis(&CameraDesc)))
	//		return E_FAIL;


	//	m_pMainCam->Set_NowSceneNum(SCENE_LOBY);


	//}
	
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
