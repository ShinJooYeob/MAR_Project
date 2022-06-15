#include "stdafx.h"
#include "..\Public\Scene_Ending.h"
#include "Scene_Loading.h"
#include "Player.h"
#include "StaticMapObject.h"
#include "Camera_Main.h"
#include "StageBoss_SpwanBoss.h"
#include "EndingAlice.h"



CScene_Ending::CScene_Ending(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CScene(pDevice,pDeviceContext)
{
}





HRESULT CScene_Ending::Initialize()
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
	
	FAILED_CHECK(Load_ActionCam(L"StageEnd_CameAction_0"));

	m_bStartChecker = false;

	FAILED_CHECK(g_pGameInstance->PlayBGM(L"BGM_STAGE_5.ogg", 0.1f));
	m_fTimer = 0;
	return S_OK;
}

_int CScene_Ending::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	if (!m_bStartChecker)
	{
		if (m_fTimer == 0)
		{
			{

				SOUNDDESC tSoundDesc;

				tSoundDesc.vPosition = tSoundDesc.vPosition = g_pGameInstance->Get_TargetPostion_float4(PLV_PLAYER);

				tSoundDesc.vMinMax = _float2(0.f, 100000.f);
				tSoundDesc.fTargetSound = 0.09f;
				wstring SoundTrack = L"";
				SoundTrack = L"5_12_1_K_Bumby.ogg";

				//SoundTrack = L"MapObject_shrinkflower_open.ogg";

				g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
			}
		}

		m_fTimer += fDeltaTime;
		if (m_fTimer > 4.5f)
		{
			m_bStartChecker = true;
			GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeFlickeringIn, 10.f, _float4(0, 0, 0, 1.f));
			m_iChecker = 0;


			CCamera_Main* pCamera = (CCamera_Main*)g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_ENDING, TAG_LAY(Layer_Camera_Main));
			NULL_CHECK_BREAK(pCamera);

			CAMERAACTION tDesc;

			tDesc.vecCamPos = m_vecCamPositions;
			tDesc.vecLookAt = m_vecLookPostions;


			m_fTimer = 0;
			pCamera->CamActionStart(tDesc);
			
		}
	

	}
	else
	{
		m_fTimer += fDeltaTime;

		if (!m_iChecker &&  m_fTimer > 25.)
		{

			FAILED_CHECK(Ready_Layer_EndingAlice(L"EndingAlice"));
			m_iChecker++;
		}
		else if (m_iChecker == 1 && m_fTimer > 27)
		{
			CEndingAlice* pAlice =(CEndingAlice*) g_pGameInstance->Get_GameObject_By_LayerLastIndex(SCENE_ENDING, L"EndingAlice");
			NULL_CHECK_BREAK(pAlice);

			pAlice->StartEndingMoving();

			m_iChecker++;

		}
		else if (m_iChecker == 2 && m_fTimer > 40.5)
		{
			g_pGameInstance->Stop_ChannelSound(CHANNEL_BGM);
			m_iChecker++;

		}
		else if (m_iChecker == 3 && m_fTimer > 41)
		{
			{

				SOUNDDESC tSoundDesc;

				tSoundDesc.vPosition = g_pGameInstance->Get_TargetPostion_float4(PLV_PLAYER);
				tSoundDesc.vMinMax = _float2(0.f, 100000.f);
				tSoundDesc.fTargetSound = 0.30f;
				wstring SoundTrack = L"";
				SoundTrack = L"1_1_1_A_Bumby_cin_rb.ogg";

				//SoundTrack = L"MapObject_shrinkflower_open.ogg";

				g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
			}

			m_iChecker++;
		}
		else if (m_iChecker == 4 && m_fTimer > 45)
		{
			GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeOut, 2.f, _float4(0, 0, 0, 1.f));
			m_iChecker++;

		}
		else if (m_iChecker == 5 && m_fTimer > 47)
		{
			FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
			FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENEID::SCENE_LOBY), SCENEID::SCENE_LOADING));
			m_iChecker++;
			return 0;
		}


	}



	return 0;
}

_int CScene_Ending::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	return 0;
}

_int CScene_Ending::Render()
{
	if (__super::Render() < 0)
		return -1;

#ifdef _DEBUG
	if (!g_bIsShowFPS)
		SetWindowText(g_hWnd, TEXT("SCENE_ENDING"));
#endif // _DEBUG

	return 0;
}

_int CScene_Ending::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	return 0;
}

_int CScene_Ending::Change_to_NextScene()
{
	FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
	FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, (SCENEID)m_eNextScene), SCENEID::SCENE_LOADING));

	return _int();
}



HRESULT CScene_Ending::Ready_Light()
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
		LightDesc.vVector = _float4(-30.f, 100.f, -30, 0);

		g_pGameInstance->Add_Light(LightDesc);
	}
	else
	{
		g_pGameInstance->Relocate_LightDesc(tagLightDesc::TYPE_DIRECTIONAL, 0, _float4(-30.f, 100.f, -30, 0).XMVector());
	}



	return S_OK;
}

HRESULT CScene_Ending::Ready_Layer_MainCamera(const _tchar * pLayerTag)
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
		m_pMainCam->Set_NowSceneNum(SCENE_ENDING);
		CTransform* pCamTransform =m_pMainCam->Get_Camera_Transform();
		NULL_CHECK_RETURN(pCamTransform, E_FAIL);

		pCamTransform->Set_MatrixState(CTransform::STATE_POS, _float3(22.041f, 21.5f, 22.178f));
		pCamTransform->LookAt(XMVectorSet(21.f, 25.f, 22.178f, 1));
		m_pMainCam->Set_ViewMatrix();
	}
	
	return S_OK;
}

HRESULT CScene_Ending::Ready_Layer_SkyBox(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_ENDING, pLayerTag, TAG_OP(Prototype_SkyBox)));

	return S_OK;
}

HRESULT CScene_Ending::Ready_Layer_Terrain(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_ENDING, pLayerTag, TAG_OP(Prototype_Terrain)));

	CTransform* TerrainTransform = (CTransform*)( g_pGameInstance->Get_Commponent_By_LayerIndex(SCENE_ENDING, pLayerTag, TAG_COM(Com_Transform)));


	return S_OK;
}

HRESULT CScene_Ending::Ready_Layer_Player(const _tchar * pLayerTag)
{

	CPlayer* pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));
	NULL_CHECK_RETURN(pPlayer, E_FAIL);

	pPlayer->Set_NowSceneNum(SCENE_ENDING);
	pPlayer->Set_PlayerPosition(_float3(16, 20, 16));

	pPlayer->Set_IsDead();
	
	return S_OK;
}

HRESULT CScene_Ending::Ready_Layer_UI(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_ENDING, pLayerTag, TAG_OP(Prototype_UIGamePlay)));
	return S_OK;
}

HRESULT CScene_Ending::Ready_Layer_EndingAlice(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_ENDING, pLayerTag, TAG_OP(Prototype_PlayerEnding),&_float3(20,20,20)));

	return S_OK;
}


HRESULT CScene_Ending::Ready_Layer_StaticMapObj(const _tchar * pLayerTag)
{
	//FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_ENDING, pLayerTag, TAG_OP(Prototype_StaticMapObject), &_float3(14, 22, 15)));

	
	//../bin/Resources/Data/Map/
	_tchar szFullPath[MAX_PATH] = L"../bin/Resources/Data/Map/Map_Ending.dat";
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

		pInstance->Add_GameObject_To_Layer(SCENE_ENDING,pLayerTag, tData.ObjectID);


		CGameObject* pObject = pInstance->Get_GameObject_By_LayerLastIndex(SCENE_ENDING, pLayerTag);

		NULL_CHECK_RETURN(pObject, E_FAIL);


		if (lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_None)))
		{
			//매쉬 바꿔주기 
			pObject->Change_Component_by_NewAssign(SCENE_ENDING, tData.MeshID, TAG_COM(Com_Model));
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


	((CStaticMapObject*)pInstance->Get_GameObject_By_LayerIndex(SCENE_ENDING, pLayerTag))->Set_FrustumSize(3000);


	return S_OK;
}



HRESULT CScene_Ending::Ready_Layer_TriggerCollider(const _tchar * pLayerTag)
{


	return S_OK;
}

HRESULT CScene_Ending::Load_ActionCam(const _tchar * szPath)
{

	{
		{
			m_vecCamPositions.clear();
			m_vecLookPostions.clear();
		}



		//../bin/Resources/Data/Map/
		_tchar szFullPath[MAX_PATH] = L"../bin/Resources/Data/CameraAction/";
		//_tchar wFileName[MAX_PATH] = L"";

		//MultiByteToWideChar(CP_UTF8, 0, szPath, -1, wFileName, sizeof(wFileName));
		//WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		lstrcat(szFullPath, szPath);

		//HANDLE hFile = CreateFileW(szFullPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);


		HANDLE hFile = ::CreateFileW(szFullPath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);


		if (INVALID_HANDLE_VALUE == hFile)
			return E_FAIL;

		DWORD	dwByte = 0;

		CGameInstance* pInstance = g_pGameInstance;

		_uint iCount = 0;
		ReadFile(hFile, &(iCount), sizeof(_uint), &dwByte, nullptr);

		CAMACTDESC tDesc;

		for (_uint i = 0; i < iCount; i++)
		{
			ReadFile(hFile, &(tDesc.fDuration), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tDesc.vPosition), sizeof(_float3), &dwByte, nullptr);

			m_vecCamPositions.push_back(tDesc);

		}


		iCount = 0;
		ReadFile(hFile, &(iCount), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iCount; i++)
		{
			ReadFile(hFile, &(tDesc.fDuration), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tDesc.vPosition), sizeof(_float3), &dwByte, nullptr);

			m_vecLookPostions.push_back(tDesc);
		}


		CloseHandle(hFile);


	}

	return S_OK;

	return S_OK;
}

CScene_Ending * CScene_Ending::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CScene_Ending* pTempLoby = new CScene_Ending(pDevice,pDeviceContext);

	if (FAILED(pTempLoby->Initialize())) 
	{
		Safe_Release(pTempLoby); 
		MSGBOX("Failed to Creating CScene_Ending");
		return nullptr;
	}

	return pTempLoby;

}

void CScene_Ending::Free()
{
	__super::Free();
}
