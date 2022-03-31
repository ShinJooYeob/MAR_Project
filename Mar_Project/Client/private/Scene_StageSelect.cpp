#include "stdafx.h"
#include "..\Public\Scene_StageSelect.h"
#include "Scene_Loading.h"
#include "Camera_Main.h"


CScene_StageSelect::CScene_StageSelect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CScene(pDevice,pDeviceContext)
{
}

HRESULT CScene_StageSelect::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;


	FAILED_CHECK(Ready_Layer_MainCamera(TAG_LAY(Layer_Camera_Main)));




	return S_OK;
}

_int CScene_StageSelect::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	if (m_bIsNeedToSceneChange)
		return Change_to_NextScene();
	



	return 0;
}

_int CScene_StageSelect::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsNeedToSceneChange)
		return Change_to_NextScene();

	return 0;
}

_int CScene_StageSelect::Render()
{
	if (__super::Render() < 0)
		return -1;




#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("GameScene"));
#endif // _DEBUG


	return 0;
}

_int CScene_StageSelect::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

	return 0;
}

_int CScene_StageSelect::Change_to_NextScene()
{

	switch (m_eNextScene)
	{

	case SCENEID::SCENE_LOBY:
	{
		FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENEID::SCENE_LOBY), SCENEID::SCENE_LOADING));
		break;
	}


	}
	return _int();
}


HRESULT CScene_StageSelect::Ready_Layer_MainCamera(const _tchar * pLayerTag)
{
	//CAMERADESC CameraDesc;

	//CameraDesc.bIsOrtho = true;

	//if (m_StageReturnBlock != NOT_EXIST_BLOCK)
	//	CameraDesc.vWorldRotAxis = m_StageReturnBlock + _float3(0.f, 3.f, 0.f);
	//else
	//	CameraDesc.vWorldRotAxis = _float3(0.f, 3.f, 0.f);

	//CameraDesc.vAxisY = _float3(0, 1, 0);
	//CameraDesc.fFovy = XMConvertToRadians(60.0f);
	//CameraDesc.fAspect = _float(g_iWinCX) / g_iWinCY;
	//CameraDesc.fNear = 0.2f;
	//CameraDesc.fFar = 300.f;

	//CameraDesc.TransformDesc.fMovePerSec = 10.f;
	//CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	//CCamera_Main* pMainCam = (CCamera_Main*)(GetSingle(CGameInstance)->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Camera_Main)));

	//if (pMainCam == nullptr)
	//	return E_FAIL;

	//if (m_StageReturnBlock == _float3(6, 8, 14))
	//{
	//	if (FAILED(pMainCam->Reset_LookAtAxis(&CameraDesc, CCamera_Main::Look_Back_Axis)))
	//		return E_FAIL;
	//}
	//else if (m_StageReturnBlock == _float3(13, 15, 13))
	//{
	//	if (FAILED(pMainCam->Reset_LookAtAxis(&CameraDesc, CCamera_Main::Look_Left_Axis)))
	//		return E_FAIL;
	//}
	//else
	//{
	//	if (FAILED(pMainCam->Reset_LookAtAxis(&CameraDesc)))
	//		return E_FAIL;
	//}


	//_float3 ActionPos[1] = { _float3(30,-24,0) };

	//FAILED_CHECK(pMainCam->ReInitialize(ActionPos,1))

	//	pMainCam->Set_NowSceneNum(SCENE_STAGESELECT);
	//pMainCam->CameraEffect(CCamera_Main::CAM_EFT_FADE_OUT, g_fDeltaTime, 2.5f);


	
	return S_OK;
}


CScene_StageSelect * CScene_StageSelect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CScene_StageSelect* pTempGame = new CScene_StageSelect(pDevice, pDeviceContext);

	if (FAILED(pTempGame->Initialize()))
	{
		Safe_Release(pTempGame);
		MSGBOX("Failed to Creating Scene_Stage1");
		return nullptr;
	}

	return pTempGame;
}

void CScene_StageSelect::Free()
{

	__super::Free();
}
