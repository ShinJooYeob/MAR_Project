#include "stdafx.h"
#include "..\Public\Scene_StageSelect.h"
#include "Scene_Loading.h"
#include "Camera_Main.h"
#include "StaticMapObject.h"
#include "StageSelectAlice.h"
#include "Npc.h"


CScene_StageSelect::CScene_StageSelect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CScene(pDevice,pDeviceContext)
{
}

HRESULT CScene_StageSelect::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;


	FAILED_CHECK(Ready_Light());
	FAILED_CHECK(Ready_Layer_MainCamera(TAG_LAY(Layer_Camera_Main)));
	FAILED_CHECK(Ready_Layer_SkyBox(TAG_LAY(Layer_SkyBox)));
	FAILED_CHECK(Ready_Layer_Terrain(TAG_LAY(Layer_Terrain)));
	FAILED_CHECK(Ready_Layer_Player(TAG_LAY(Layer_Player)));
	FAILED_CHECK(Ready_Layer_UI(TAG_LAY(Layer_UI_GamePlay)));

	FAILED_CHECK(Ready_Layer_NPC(TAG_LAY(Layer_NPC)));
	FAILED_CHECK(Ready_Layer_TriggerCollider(TAG_LAY(Layer_TriggerCollider)));
	

	FAILED_CHECK(Ready_Layer_StaticMapObj(TAG_LAY(Layer_StaticMapObj)));


	GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeIn, 0.5, { 0,0,0,1 });
	GetSingle(CUtilityMgr)->DistFogRendOn(true);


	FAILED_CHECK(g_pGameInstance->PlayBGM(L"BGM_STAGE_6.ogg", 0.05f));






	return S_OK;
}

_int CScene_StageSelect::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	if (m_bIsNeedToSceneChange)
		return Change_to_NextScene();
	
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
		FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENEID::SCENE_STAGE1), SCENEID::SCENE_LOADING));
	}


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
	if (!g_bIsShowFPS)

	SetWindowText(g_hWnd, TEXT("GameScene"));
#endif // _DEBUG


	return 0;
}

_int CScene_StageSelect::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	return 0;
}

_int CScene_StageSelect::Change_to_NextScene()
{

	FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
	FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, (SCENEID)m_eNextScene), SCENEID::SCENE_LOADING));


	return _int();
}


HRESULT CScene_StageSelect::Ready_Light()
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
	return S_OK;
}

HRESULT CScene_StageSelect::Ready_Layer_MainCamera(const _tchar * pLayerTag)
{
	CCamera::CAMERADESC CameraDesc;
	CameraDesc.vWorldRotAxis = _float3(0, 0, 0);
	CameraDesc.vEye = _float3(0, 25.f, 20.f);
	CameraDesc.vAt = _float3(71, 20.5f, 24.5f);
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
		m_pMainCam->Set_NowSceneNum(SCENE_STAGESELECT);
	}
	
	return S_OK;
}

HRESULT CScene_StageSelect::Ready_Layer_SkyBox(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, pLayerTag, TAG_OP(Prototype_SkyBox)));

	return S_OK;
}

HRESULT CScene_StageSelect::Ready_Layer_Terrain(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, pLayerTag, TAG_OP(Prototype_Terrain)));

	CTransform* TerrainTransform = (CTransform*)(g_pGameInstance->Get_Commponent_By_LayerIndex(SCENE_STAGESELECT, pLayerTag, TAG_COM(Com_Transform)));

	return S_OK;
}

HRESULT CScene_StageSelect::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CStageSelectAlice* pPlayer = (CStageSelectAlice*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STAGESELECT, TAG_LAY(Layer_Player)));

	if (pPlayer == nullptr)
	{
		FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, pLayerTag, L"Prototype_StageSelectAlice", &_float3(71, 20.5f, 24.5f)));

	}


	return S_OK;
}

HRESULT CScene_StageSelect::Ready_Layer_UI(const _tchar * pLayerTag)
{


	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, pLayerTag, L"Prototype_SelectSceneUI"));

	
	return S_OK;
}

HRESULT CScene_StageSelect::Ready_Layer_NPC(const _tchar * pLayerTag)
{
	CGameInstance* pInstance = g_pGameInstance;


	
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, L"Layer_CatNpc", L"Prototype_CatNpc"));

	CNpc::NPCDESC tDesc;
	
	tDesc.StartPosition = _float3(28,  20, 28.5);
	tDesc.DestPosition =_float3(61.f,  20.f, 28.5f) ;
	tDesc.MeshKinds =3 ;
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, pLayerTag, L"Prototype_NPC", &tDesc));

	tDesc.StartPosition = _float3(28, 20, 28);
	tDesc.DestPosition = _float3(61.f, 20.f, 28.f);
	tDesc.MeshKinds = 2;
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, pLayerTag, L"Prototype_NPC", &tDesc));

	tDesc.StartPosition = _float3(28, 20, 29);
	tDesc.DestPosition = _float3(61.f, 20.f, 29.f);
	tDesc.MeshKinds = 1;
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, pLayerTag, L"Prototype_NPC", &tDesc));



	tDesc.StartPosition = _float3(93.f, 20.f, 26.5f);
	tDesc.DestPosition = _float3(116.215f, 20.f, 26.5f);
	tDesc.MeshKinds = 2;
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, pLayerTag, L"Prototype_NPC", &tDesc));
	tDesc.StartPosition = _float3(93.f, 20.f, 28.f);
	tDesc.DestPosition = _float3(116.215f, 20.f, 28.f);
	tDesc.MeshKinds = 4;
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, pLayerTag, L"Prototype_NPC", &tDesc));


	tDesc.StartPosition = _float3(60.594f, 20.f, 55.701f);
	tDesc.DestPosition = _float3(72.558f, 20.f, 56.542f);
	tDesc.MeshKinds = 3;
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, pLayerTag, L"Prototype_NPC", &tDesc));


	tDesc.StartPosition = _float3(90.910f, 20.f, 59.951f);
	tDesc.DestPosition = _float3(108.010f, 20.f, 59.951f);
	tDesc.MeshKinds = 4;
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, pLayerTag, L"Prototype_NPC", &tDesc));
	tDesc.StartPosition = _float3(90.910f, 20.f, 58.664f);
	tDesc.DestPosition = _float3(108.010f, 20.f, 58.664f);
	tDesc.MeshKinds = 1;
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, pLayerTag, L"Prototype_NPC", &tDesc));


	tDesc.StartPosition = _float3(67.348f, 20.f, 21.322f);
	tDesc.DestPosition = _float3(80.966f, 20.f, 21.218f);
	tDesc.MeshKinds = 0;
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, pLayerTag, L"Prototype_NPC", &tDesc));


	tDesc.StartPosition = _float3(98.073f, 20.f, 56.335f);
	tDesc.DestPosition = _float3(80.258f, 20.f, 55.956f);
	tDesc.MeshKinds = 0;
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, pLayerTag, L"Prototype_NPC", &tDesc));

	tDesc.StartPosition = _float3(67.596f, 20.f, 59.273f);
	tDesc.DestPosition = _float3(81.645f, 20.f, 59.235f);
	tDesc.MeshKinds = 0;
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, pLayerTag, L"Prototype_NPC", &tDesc));


	return S_OK;
}

HRESULT CScene_StageSelect::Ready_Layer_StaticMapObj(const _tchar * pLayerTag)
{

	_tchar szFullPath[MAX_PATH] = L"../bin/Resources/Data/Map/Map_StageSelect.dat";
	_tchar wFileName[MAX_PATH] = L"";

	lstrcat(szFullPath, wFileName);


	HANDLE hFile = ::CreateFileW(szFullPath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);


	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD	dwByte = 0;

	CGameInstance* pInstance = g_pGameInstance;


	_uint iIDLength = 0;

	while (true)
	{
		OBJELEMENT	tData{};
		_tchar szBuffer[MAX_PATH] = L"";
		// key °ª ·Îµå
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

		//°´Ã¤ »ý¼ºÇØÁÖ±â
		if (!lstrcmp(tData.ObjectID, L"EditorCursor")) continue;

		pInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, pLayerTag, tData.ObjectID);


		CGameObject* pObject = pInstance->Get_GameObject_By_LayerLastIndex(SCENE_STAGESELECT, pLayerTag);

		NULL_CHECK_RETURN(pObject, E_FAIL);


		if (lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_None)))
		{
			//¸Å½¬ ¹Ù²ãÁÖ±â 
			pObject->Change_Component_by_NewAssign(SCENE_STAGESELECT, tData.MeshID, TAG_COM(Com_Model));
			((CStaticMapObject*)pObject)->Set_FrustumSize(XMVectorGetX(XMVector3Length((tData.matTransform.XMatrix()).r[0])) * 7.f);


			if ((!lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_GloryTree_Main))) || (!lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_GloryTree_MainB))))
			{
				((CStaticMapObject*)pObject)->Set_PassIndex(3);
				((CStaticMapObject*)pObject)->Set_FrustumSize(50);
			}

		}

		//Æ®·»½ºÆû
		CTransform* pTrans = (CTransform*)(pObject->Get_Component(TAG_COM(Com_Transform)));
		NULL_CHECK_RETURN(pTrans, E_FAIL);
		pTrans->Set_Matrix(tData.matTransform);



	}



	CloseHandle(hFile);
	return S_OK;
}

HRESULT CScene_StageSelect::Ready_Layer_TriggerCollider(const _tchar * pLayerTag)
{


	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGESELECT, pLayerTag, L"Prototype_StageSelect_CatNScene"));


	




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
