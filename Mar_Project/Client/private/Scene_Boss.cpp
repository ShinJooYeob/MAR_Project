#include "stdafx.h"
#include "..\Public\Scene_Boss.h"
#include "Scene_Loading.h"
#include "Player.h"
#include "StaticMapObject.h"
#include "Camera_Main.h"
#include "StageBoss_SpwanBoss.h"



CScene_Boss::CScene_Boss(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CScene(pDevice,pDeviceContext)
{
}





HRESULT CScene_Boss::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	GetSingle(CUtilityMgr)->DistFogRendOn(true);


	FAILED_CHECK(Ready_Light());

	FAILED_CHECK(Ready_Layer_MainCamera(TAG_LAY(Layer_Camera_Main)));
	FAILED_CHECK(Ready_Layer_SkyBox(TAG_LAY(Layer_SkyBox)));
	FAILED_CHECK(Ready_Layer_Terrain(TAG_LAY(Layer_Terrain)));
	
	
	FAILED_CHECK(Ready_Layer_Player(TAG_LAY(Layer_Player)));
	FAILED_CHECK(Ready_Layer_UI(TAG_LAY(Layer_UI_GamePlay)));
	

	FAILED_CHECK(Ready_Layer_StaticMapObj(TAG_LAY(Layer_StaticMapObj)));


	//FAILED_CHECK(Ready_Layer_DollMaker(TAG_LAY(Layer_Monster)));


	FAILED_CHECK(Ready_Layer_TriggerCollider(TAG_LAY(Layer_TriggerCollider)));
	

	

	return S_OK;
}

_int CScene_Boss::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	if (g_pGameInstance->Get_DIKeyState(DIK_J)&DIS_Down)
		FAILED_CHECK(Ready_Layer_DollMaker(TAG_LAY(Layer_Monster)));
	if (GetSingle(CGameInstance)->Get_DIKeyState(DIK_HOME)&DIS_Press)
	{

		LIGHTDESC* pLightDesc = g_pGameInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
		pLightDesc->vVector.z += 1.f;

		wstring ttDebugLog = L"LightPosLuminece : X: " + to_wstring(pLightDesc->vVector.x) + L",  Y: " + to_wstring(pLightDesc->vVector.y) + L",  Z: " + to_wstring(pLightDesc->vVector.z) + L"\n";
		OutputDebugStringW(ttDebugLog.c_str());
	}

	if (GetSingle(CGameInstance)->Get_DIKeyState(DIK_END)&DIS_Press)
	{

		LIGHTDESC* pLightDesc = g_pGameInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
		pLightDesc->vVector.z -= 1.f;

		wstring ttDebugLog = L"LightPosLuminece : X: " + to_wstring(pLightDesc->vVector.x) + L",  Y: " + to_wstring(pLightDesc->vVector.y) + L",  Z: " + to_wstring(pLightDesc->vVector.z) + L"\n";
		OutputDebugStringW(ttDebugLog.c_str());
	}
	if (GetSingle(CGameInstance)->Get_DIKeyState(DIK_DELETE)&DIS_Press)
	{

		LIGHTDESC* pLightDesc = g_pGameInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
		pLightDesc->vVector.x -= 1.f;

		wstring ttDebugLog = L"LightPosLuminece : X: " + to_wstring(pLightDesc->vVector.x) + L",  Y: " + to_wstring(pLightDesc->vVector.y) + L",  Z: " + to_wstring(pLightDesc->vVector.z) + L"\n";
		OutputDebugStringW(ttDebugLog.c_str());
	}

	if (GetSingle(CGameInstance)->Get_DIKeyState(DIK_PGDN)&DIS_Press)
	{

		LIGHTDESC* pLightDesc = g_pGameInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
		pLightDesc->vVector.x += 1.f;

		wstring ttDebugLog = L"LightPosLuminece : X: " + to_wstring(pLightDesc->vVector.x) + L",  Y: " + to_wstring(pLightDesc->vVector.y) + L",  Z: " + to_wstring(pLightDesc->vVector.z) + L"\n";
		OutputDebugStringW(ttDebugLog.c_str());
	}

	if (GetSingle(CGameInstance)->Get_DIKeyState(DIK_INSERT)&DIS_Press)
	{

		LIGHTDESC* pLightDesc = g_pGameInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
		pLightDesc->vVector.y += 1.f;

		wstring ttDebugLog = L"LightPosLuminece : X: " + to_wstring(pLightDesc->vVector.x) + L",  Y: " + to_wstring(pLightDesc->vVector.y) + L",  Z: " + to_wstring(pLightDesc->vVector.z) + L"\n";
		OutputDebugStringW(ttDebugLog.c_str());
	}

	if (GetSingle(CGameInstance)->Get_DIKeyState(DIK_PGUP)&DIS_Press)
	{

		LIGHTDESC* pLightDesc = g_pGameInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
		pLightDesc->vVector.y -= 1.f;

		wstring ttDebugLog = L"LightPosLuminece : X: " + to_wstring(pLightDesc->vVector.x) + L",  Y: " + to_wstring(pLightDesc->vVector.y) + L",  Z: " + to_wstring(pLightDesc->vVector.z) + L"\n";
		OutputDebugStringW(ttDebugLog.c_str());
	}



	return 0;
}

_int CScene_Boss::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	return 0;
}

_int CScene_Boss::Render()
{
	if (__super::Render() < 0)
		return -1;

#ifdef _DEBUG
	if (!g_bIsShowFPS)
		SetWindowText(g_hWnd, TEXT("SCENE_BOSS"));
#endif // _DEBUG

	return 0;
}

_int CScene_Boss::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	return 0;
}

_int CScene_Boss::Change_to_NextScene()
{
	FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
	FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, (SCENEID)m_eNextScene), SCENEID::SCENE_LOADING));

	return _int();
}



HRESULT CScene_Boss::Ready_Light()
{

//LightPosLuminece: X: -100.000000, Y : 122.000000, Z : -55.000000

	const LIGHTDESC* pLightDesc = g_pGameInstance->Get_LightDesc(tagLightDesc::TYPE_DIRECTIONAL, 0);

	if (pLightDesc == nullptr)
	{

		LIGHTDESC LightDesc;

		LightDesc.eLightType = tagLightDesc::TYPE_DIRECTIONAL;
		LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vAmbient = _float4(1.0f);
		LightDesc.vSpecular = _float4(1);
		LightDesc.vVector = _float4(-100.f, 217.f, -55, 0);

		g_pGameInstance->Add_Light(LightDesc);
	}
	else
	{
		g_pGameInstance->Relocate_LightDesc(tagLightDesc::TYPE_DIRECTIONAL, 0, _float4(-100.f, 217.f, -55, 0).XMVector());
	}



	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_MainCamera(const _tchar * pLayerTag)
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
		m_pMainCam->Set_NowSceneNum(SCENE_BOSS);
	}
	
	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_SkyBox(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_SkyBox)));

	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_Terrain(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_Terrain)));

	CTransform* TerrainTransform = (CTransform*)( g_pGameInstance->Get_Commponent_By_LayerIndex(SCENE_BOSS, pLayerTag, TAG_COM(Com_Transform)));


	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_Player(const _tchar * pLayerTag)
{

	CPlayer* pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));
	NULL_CHECK_RETURN(pPlayer, E_FAIL);

	pPlayer->Set_NowSceneNum(SCENE_BOSS);
	pPlayer->Renew_Player(_float3(45, 11, 57), _float3(85, 100, 78));
	

	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_UI(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_UIGamePlay)));
	return S_OK;
}


HRESULT CScene_Boss::Ready_Layer_StaticMapObj(const _tchar * pLayerTag)
{
	//FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_StaticMapObject), &_float3(14, 22, 15)));

	
	//../bin/Resources/Data/Map/
	_tchar szFullPath[MAX_PATH] = L"../bin/Resources/Data/Map/Map_Boss.dat";
	_tchar wFileName[MAX_PATH] = L"";

	//MultiByteToWideChar(CP_UTF8, 0, szFileName, -1, wFileName, sizeof(wFileName));
	//WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
	lstrcat(szFullPath, wFileName);



	//HANDLE hFile = CreateFileW(szFullPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);


	HANDLE hFile = ::CreateFileW(szFullPath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);


	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD	dwByte = 0;

	CGameInstance* pInstance = g_pGameInstance;


	_uint iIDLength = 0;

	// 유니코드임을 알리는 BOM
	//DWORD wc = 0xFF;
	//ReadFile(hFile, &wc, 3, &dwByte, NULL);

	while (true)
	{
		OBJELEMENT	tData{};
		_tchar szBuffer[MAX_PATH] = L"";
		// key 값 로드
		ReadFile(hFile, &(iIDLength), sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, (tData.ObjectID), sizeof(_tchar) * iIDLength, &dwByte, nullptr);
		//lstrcpy(tData.ObjectID, szBuffer);

		ReadFile(hFile, &(iIDLength), sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, (tData.MeshID), sizeof(_tchar) * iIDLength, &dwByte, nullptr);
		//lstrcpy(tData.MeshID, szBuffer);

		ReadFile(hFile, &(iIDLength), sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, (tData.TexturePath), sizeof(_tchar) * iIDLength, &dwByte, nullptr);
		//lstrcpy(tData.TexturePath, szBuffer);

		ReadFile(hFile, &(iIDLength), sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, (tData.TextureKey), sizeof(_tchar) * iIDLength, &dwByte, nullptr);
		//lstrcpy(tData.TextureKey, szBuffer);

		ReadFile(hFile, &(tData.PassIndex), sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &(tData.matSRT.m[0][0]), sizeof(_float) * 16, &dwByte, nullptr);
		ReadFile(hFile, &(tData.matTransform.m[0][0]), sizeof(_float) * 16, &dwByte, nullptr);

		if (0 == dwByte)
			break;

		//객채 생성해주기
		if(!lstrcmp(tData.ObjectID, L"EditorCursor")) continue;

		pInstance->Add_GameObject_To_Layer(SCENE_BOSS,pLayerTag, tData.ObjectID);


		CGameObject* pObject = pInstance->Get_GameObject_By_LayerLastIndex(SCENE_BOSS, pLayerTag);

		NULL_CHECK_RETURN(pObject, E_FAIL);


		if (lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_None)))
		{
			//매쉬 바꿔주기 
			pObject->Change_Component_by_NewAssign(SCENE_BOSS, tData.MeshID, TAG_COM(Com_Model));
			((CStaticMapObject*)pObject)->Set_FrustumSize(XMVectorGetX(XMVector3Length((tData.matTransform.XMatrix()).r[0])) * 7.f);



			if ((!lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_GloryTree_Main))) || (!lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_GloryTree_MainB))))
			{
				((CStaticMapObject*)pObject)->Set_PassIndex(3);
				((CStaticMapObject*)pObject)->Set_FrustumSize(50);
			}

		}

		//트렌스폼
		CTransform* pTrans = (CTransform*)(pObject->Get_Component(TAG_COM(Com_Transform)));
		NULL_CHECK_RETURN(pTrans, E_FAIL);
		pTrans->Set_Matrix(tData.matTransform);



	}



	CloseHandle(hFile);


	((CStaticMapObject*)pInstance->Get_GameObject_By_LayerIndex(SCENE_BOSS, pLayerTag))->Set_FrustumSize(3000);


	return S_OK;
}



HRESULT CScene_Boss::Ready_Layer_DollMaker(const _tchar * pLayerTag)
{

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_DollMaker), &_float3(85, 10, 78)));


	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_HandyBoy(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_HandyBoy), &_float3(76, 10, 63)));
	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_HandyGirl(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_HandyGirl), &_float3(67, 10, 78)));
	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_TriggerCollider(const _tchar * pLayerTag)
{
	CStageBoss_SpwanBoss::SPWANTRIGGERDESC tSwpanDesc;

	tSwpanDesc.vPosition = _float3(50.f, 16.300f, 60.f);

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, L"StageBoss_SpwanBoss", &tSwpanDesc));


	return S_OK;
}

CScene_Boss * CScene_Boss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CScene_Boss* pTempLoby = new CScene_Boss(pDevice,pDeviceContext);

	if (FAILED(pTempLoby->Initialize())) 
	{
		Safe_Release(pTempLoby); 
		MSGBOX("Failed to Creating CScene_Boss");
		return nullptr;
	}

	return pTempLoby;

}

void CScene_Boss::Free()
{
	__super::Free();
}
