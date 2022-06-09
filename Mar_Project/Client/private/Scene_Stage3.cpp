#include "stdafx.h"
#include "..\Public\Scene_Stage3.h"
#include "Scene_Loading.h"
#include "Player.h"
#include "StaticMapObject.h"
#include "Camera_Main.h"
#include "VentObj.h"
#include "MovableColum.h"
#include "MovableColumBtn.h"
#include "SceneChageTriger.h"
#include "Stage3_SpwanExecutor.h"



CScene_Stage3::CScene_Stage3(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CScene(pDevice,pDeviceContext)
{
}





HRESULT CScene_Stage3::Initialize()
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
	FAILED_CHECK(Ready_VentObject(TAG_LAY(Layer_Vent)));
	FAILED_CHECK(Ready_MovableColum(TAG_LAY(Layer_MazeDoor)));
	FAILED_CHECK(Ready_MovableColumBtn(TAG_LAY(Layer_ButtonPad)));
	FAILED_CHECK(Ready_BreakableWall(TAG_LAY(Layer_Breakable)));
	
	

	FAILED_CHECK(Ready_Layer_TriggerCollider(TAG_LAY(Layer_TriggerCollider)));
	
	//FAILED_CHECK(Ready_Layer_Executor(TAG_LAY(Layer_Monster)));


	//GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeIn, 0.5, { 0,0,0,1 });

	return S_OK;
}

_int CScene_Stage3::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;



	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
		FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENEID::SCENE_BOSS), SCENEID::SCENE_LOADING));
	}



	return 0;
}

_int CScene_Stage3::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsNeedToSceneChange)
		return Change_to_NextScene();

	return 0;
}

_int CScene_Stage3::Render()
{
	if (__super::Render() < 0)
		return -1;

#ifdef _DEBUG
	if (!g_bIsShowFPS)
		SetWindowText(g_hWnd, TEXT("SCENE_STAGE3"));
#endif // _DEBUG

	return 0;
}

_int CScene_Stage3::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	return 0;
}

_int CScene_Stage3::Change_to_NextScene()
{

	FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
	FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, (SCENEID)m_eNextScene), SCENEID::SCENE_LOADING));

	return _int();
}



HRESULT CScene_Stage3::Ready_Light()
{

	const LIGHTDESC* pLightDesc = g_pGameInstance->Get_LightDesc(tagLightDesc::TYPE_DIRECTIONAL, 0);

	if (pLightDesc == nullptr)
	{

		LIGHTDESC LightDesc;

		LightDesc.eLightType = tagLightDesc::TYPE_DIRECTIONAL;
		LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vAmbient = _float4(1.0f);
		LightDesc.vSpecular = _float4(1);
		LightDesc.vVector = _float4(0.f, 256, 128.f, 0);

		g_pGameInstance->Add_Light(LightDesc);
	}
	else
	{
		g_pGameInstance->Relocate_LightDesc(tagLightDesc::TYPE_DIRECTIONAL, 0, _float4(0.f, 256, 128.f, 0).XMVector());
	}


	return S_OK;
}

HRESULT CScene_Stage3::Ready_Layer_MainCamera(const _tchar * pLayerTag)
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
		m_pMainCam->Set_NowSceneNum(SCENE_STAGE3);
	}
	
	return S_OK;
}

HRESULT CScene_Stage3::Ready_Layer_SkyBox(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE3, pLayerTag, TAG_OP(Prototype_SkyBox)));

	return S_OK;
}

HRESULT CScene_Stage3::Ready_Layer_Terrain(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE3, pLayerTag, TAG_OP(Prototype_Terrain)));

	CTransform* TerrainTransform = (CTransform*)( g_pGameInstance->Get_Commponent_By_LayerIndex(SCENE_STAGE3, pLayerTag, TAG_COM(Com_Transform)));


	return S_OK;
}

HRESULT CScene_Stage3::Ready_Layer_Player(const _tchar * pLayerTag)
{

	CPlayer* pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));
	NULL_CHECK_RETURN(pPlayer, E_FAIL);

	pPlayer->Set_NowSceneNum(SCENE_STAGE3);
	/*executor 앞*/
	//pPlayer->Renew_Player(_float3(128, 23.134f, 174.085f), _float3(128, 23.134f, 200.085f));
	/*원래 위치*/
	pPlayer->Renew_Player(_float3(128, 20.f, 88) , _float3(128, 20.f, 108));
	

	return S_OK;
}

HRESULT CScene_Stage3::Ready_Layer_UI(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE3, pLayerTag, TAG_OP(Prototype_UIGamePlay)));
	return S_OK;
}


HRESULT CScene_Stage3::Ready_Layer_StaticMapObj(const _tchar * pLayerTag)
{
	//FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE3, pLayerTag, TAG_OP(Prototype_StaticMapObject), &_float3(14, 22, 15)));

	
	//../bin/Resources/Data/Map/
	_tchar szFullPath[MAX_PATH] = L"../bin/Resources/Data/Map/Map_3.dat";
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

		pInstance->Add_GameObject_To_Layer(SCENE_STAGE3,pLayerTag, tData.ObjectID);


		CGameObject* pObject = pInstance->Get_GameObject_By_LayerLastIndex(SCENE_STAGE3, pLayerTag);

		NULL_CHECK_RETURN(pObject, E_FAIL);


		if (lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_None)))
		{
			//매쉬 바꿔주기 
			pObject->Change_Component_by_NewAssign(SCENE_STAGE3, tData.MeshID, TAG_COM(Com_Model));
			((CStaticMapObject*)pObject)->Set_FrustumSize(XMVectorGetX(XMVector3Length((tData.matTransform.XMatrix()).r[0])) * 7.f);



			if ((!lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_Gazebo_S))))
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


	((CStaticMapObject*)pInstance->Get_GameObject_By_LayerIndex(SCENE_STAGE3, pLayerTag))->Set_FrustumSize(3000);


	return S_OK;
}

HRESULT CScene_Stage3::Ready_VentObject(const _tchar * pLayerTag)
{

	CGameInstance* pInstance = g_pGameInstance;

	list<CGameObject*>* VentList;
	CVentObj::VENTDESC tDesc;

	CVentObj* pSourceVent = nullptr;

	tDesc.vPosition = _float3(147.f, 20.f, 140.6f);
	tDesc.vTargetPosition = _float3(149.f, 20.f, 137.5f);

	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGE3, pLayerTag, TAG_OP(Prototype_Vent), &tDesc));
	VentList = pInstance->Get_ObjectList_from_Layer(SCENE_STAGE3, pLayerTag);
	NULL_CHECK_RETURN(VentList, E_FAIL);

	pSourceVent = (CVentObj*)(VentList->back());
	NULL_CHECK_RETURN(pSourceVent, E_FAIL);
	pSourceVent->Load_ActionCam(L"Stage3_CameAction_0");

	tDesc.vPosition = _float3(114.5f, 20.f, 129.f);
	tDesc.vTargetPosition = _float3(114.5f, 20.f, 125.f);
	tDesc.pTargetVent = pSourceVent;

	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGE3, pLayerTag, TAG_OP(Prototype_Vent), &tDesc));

	((CVentObj*)(VentList->back()))->Load_ActionCam(L"Stage3_CameAction_1");
	pSourceVent->Set_TargetVent((VentList->back()));





	pSourceVent = nullptr;

	tDesc.vPosition = _float3(86.561f, 20.f, 145.772f);
	tDesc.vTargetPosition = _float3(92.490f, 20.f, 143.29f);
	

	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGE3, pLayerTag, TAG_OP(Prototype_Vent), &tDesc));
	VentList = pInstance->Get_ObjectList_from_Layer(SCENE_STAGE3, pLayerTag);
	NULL_CHECK_RETURN(VentList, E_FAIL);

	pSourceVent = (CVentObj*)(VentList->back());
	NULL_CHECK_RETURN(pSourceVent, E_FAIL);
	pSourceVent->Load_ActionCam(L"Stage3_CameAction_2");

	tDesc.vPosition = _float3(106.8f, 20.f, 125.828f);
	tDesc.vTargetPosition = _float3(106.8f, 20.f, 131.935f);
	tDesc.pTargetVent = pSourceVent;

	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGE3, pLayerTag, TAG_OP(Prototype_Vent), &tDesc));

	((CVentObj*)(VentList->back()))->Load_ActionCam(L"Stage3_CameAction_3");
	pSourceVent->Set_TargetVent((VentList->back()));





	return S_OK;
}

HRESULT CScene_Stage3::Ready_MovableColum(const _tchar * pLayerTag)
{
	CMovableColum::CLOCKCOLUMDESC tDesc;

	tDesc.vPosition = _float3(128, 22, 128);
	tDesc.vAngle = _float3(0,180,0);

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE3, pLayerTag, TAG_OP(Prototype_MazeDoor), &tDesc));


	return S_OK;
}

HRESULT CScene_Stage3::Ready_MovableColumBtn(const _tchar * pLayerTag)
{
	CGameInstance* pInstance = g_pGameInstance;

	CMovableColumBtn::COLUMBTNDESC tDesc;


	tDesc.pTargetObject = pInstance->Get_GameObject_By_LayerLastIndex(SCENE_STAGE3, TAG_LAY(Layer_MazeDoor));
	NULL_CHECK_RETURN(tDesc.pTargetObject, E_FAIL);




	tDesc.vPosition = _float3(140, 20, 90);
	tDesc.eKindsOfObject = 0;
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGE3, pLayerTag, TAG_OP(Prototype_MazeDoorBtn), &tDesc));



	tDesc.vPosition = _float3(108, 20, 150.5f);
	tDesc.eKindsOfObject = 1;
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGE3, pLayerTag, TAG_OP(Prototype_MazeDoorBtn), &tDesc));


	tDesc.vPosition = _float3(117.2f, 20, 137);
	tDesc.eKindsOfObject = 2;
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGE3, pLayerTag, TAG_OP(Prototype_MazeDoorBtn), &tDesc));

	return S_OK;
}

HRESULT CScene_Stage3::Ready_BreakableWall(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE3, pLayerTag, TAG_OP(Prototype_BreakableWall)));
	return S_OK;
}

HRESULT CScene_Stage3::Ready_Layer_TriggerCollider(const _tchar * pLayerTag)
{
	CSceneChageTriger::SCNCHGTRGDESC tDesc;

	tDesc.eTargetScene = SCENE_BOSS;
	tDesc.vPosition = _float3(129.3f, 27.39f, 236.739f);
	tDesc.vScale = _float3(17, 2, 1);
	//tDesc.vTargetPosition = _float3(127.489f, 21.0f, 254.976f);
	tDesc.vTargetPosition = _float3(127.489f, 25.89f, 254.976f);


	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE3, pLayerTag, TAG_OP(Prototype_TriggerCollider_SceneChager), &tDesc));



	CStage3_SpwanExecutor::SPWANTRIGGERDESC tExcutorSwpanDesc;

	tExcutorSwpanDesc.vPosition = _float3(126.581f, 35.59f, 187.311f);
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE3, pLayerTag, L"Stage3_SpwanExecutor", &tExcutorSwpanDesc));


	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE3, pLayerTag, L"Stage3_GientTrigger"));

	


	return S_OK;
}

HRESULT CScene_Stage3::Ready_Layer_Executor(const _tchar * pLayerTag)
{


	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE3, pLayerTag, TAG_OP(Prototype_Executor), &_float3(126.581f, 35.59f, 188)));


	return S_OK;
}



CScene_Stage3 * CScene_Stage3::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CScene_Stage3* pTempLoby = new CScene_Stage3(pDevice,pDeviceContext);

	if (FAILED(pTempLoby->Initialize())) 
	{
		Safe_Release(pTempLoby); 
		MSGBOX("Failed to Creating CScene_Stage3");
		return nullptr;
	}

	return pTempLoby;

}

void CScene_Stage3::Free()
{
	__super::Free();
}
