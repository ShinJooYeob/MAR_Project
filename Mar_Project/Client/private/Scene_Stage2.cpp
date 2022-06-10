#include "stdafx.h"
#include "..\Public\Scene_Stage2.h"
#include "Scene_Loading.h"
#include "Player.h"
#include "StaticMapObject.h"
#include "Camera_Main.h"
#include "EscalatorPad.h"
#include "SlidePad.h"
#include "ButtonPad.h"
#include "ShapeMemoryPad.h"
#include "ShpaeMemButton.h"
#include "PresserObj.h"
#include "SceneChageTriger.h"


CScene_Stage2::CScene_Stage2(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CScene(pDevice,pDeviceContext)
{
}





HRESULT CScene_Stage2::Initialize()
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
	FAILED_CHECK(Ready_Layer_SteamPad(TAG_LAY(Layer_SteamPad)));
	FAILED_CHECK(Ready_Layer_HiddenPad(TAG_LAY(Layer_HiddenPad)));
	FAILED_CHECK(Ready_Layer_EscalatorPad(TAG_LAY(Layer_EscalatorPad)));
	FAILED_CHECK(Ready_Layer_SlidePad(TAG_LAY(Layer_SlideObj)));
	FAILED_CHECK(Ready_Layer_ButtonPad(TAG_LAY(Layer_ButtonPad)));
	FAILED_CHECK(Ready_Layer_ShapeMemPad(TAG_LAY(Layer_ShapeMemoryPad)));
	FAILED_CHECK(Ready_Layer_ShapeMemBtn(TAG_LAY(Layer_ButtonPad)));
	FAILED_CHECK(Ready_Layer_PresserObj(TAG_LAY(Layer_Presser)));

	FAILED_CHECK(Ready_Layer_TriggerCollider(TAG_LAY(Layer_TriggerCollider)));
	
	FAILED_CHECK(Ready_Layer_Eyepot(TAG_LAY(Layer_Monster)));
	

	//GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeIn, 0.5, { 0,0,0,1 });
	return S_OK;
}

_int CScene_Stage2::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;



	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
		FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENEID::SCENE_STAGE3), SCENEID::SCENE_LOADING));
	}



	return 0;
}

_int CScene_Stage2::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsNeedToSceneChange)
		return Change_to_NextScene();

	return 0;
}

_int CScene_Stage2::Render()
{
	if (__super::Render() < 0)
		return -1;

#ifdef _DEBUG
	if (!g_bIsShowFPS)
		SetWindowText(g_hWnd, TEXT("SCENE_STAGE2"));
#endif // _DEBUG

	return 0;
}

_int CScene_Stage2::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	return 0;
}

_int CScene_Stage2::Change_to_NextScene()
{

	FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
	FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, (SCENEID)m_eNextScene), SCENEID::SCENE_LOADING));

	return _int();
}



HRESULT CScene_Stage2::Ready_Light()
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

	 
	{

		//LIGHTDESC LightDesc;

		//LightDesc.eLightType = tagLightDesc::TYPE_POINT;
		//LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
		//LightDesc.vAmbient = _float4(1.0f);
		//LightDesc.vSpecular = _float4(1);

		//LightDesc.vVector = _float4(83.293f, 34.2f, 155.192f, 0);
		//LightDesc.fRange = 2.3f;

		//g_pGameInstance->Add_Light(LightDesc);


		//LightDesc = LIGHTDESC();

		//LightDesc.eLightType = tagLightDesc::TYPE_POINT;
		//LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
		//LightDesc.vAmbient = _float4(1.0f);
		//LightDesc.vSpecular = _float4(1);

		//LightDesc.vVector = _float4(101.482f, 34.2f ,154.974f, 0);
		//LightDesc.fRange = 10;

		//g_pGameInstance->Add_Light(LightDesc);
		
	}


	return S_OK;
}

HRESULT CScene_Stage2::Ready_Layer_MainCamera(const _tchar * pLayerTag)
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
		m_pMainCam->Set_NowSceneNum(SCENE_STAGE2);
	}
	
	return S_OK;
}

HRESULT CScene_Stage2::Ready_Layer_SkyBox(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_SkyBox)));

	return S_OK;
}

HRESULT CScene_Stage2::Ready_Layer_Terrain(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_Terrain)));

	CTransform* TerrainTransform = (CTransform*)( g_pGameInstance->Get_Commponent_By_LayerIndex(SCENE_STAGE2, pLayerTag, TAG_COM(Com_Transform)));


	return S_OK;
}

HRESULT CScene_Stage2::Ready_Layer_Player(const _tchar * pLayerTag)
{

	CPlayer* pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));
	NULL_CHECK_RETURN(pPlayer, E_FAIL);

	pPlayer->Set_NowSceneNum(SCENE_STAGE2);

	/*찻잔 앞에*/
	pPlayer->Renew_Player(_float3(68.85f, 19.66f, 102.472f), _float3(69.106f, 19.66f, 97.17f));

	//pPlayer->Renew_Player(_float3(187.1f, 25.16f, 175), _float3(200.1f, 25.16f, 175));
	
	/*원래 위치*/
	//pPlayer->Renew_Player(_float3(70, 25.16f, 168), _float3(80, 25.16f, 168));

	return S_OK;
}

HRESULT CScene_Stage2::Ready_Layer_UI(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_UIGamePlay)));
	return S_OK;
}


HRESULT CScene_Stage2::Ready_Layer_StaticMapObj(const _tchar * pLayerTag)
{
	//FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_StaticMapObject), &_float3(14, 22, 15)));

	
	//../bin/Resources/Data/Map/
	_tchar szFullPath[MAX_PATH] = L"../bin/Resources/Data/Map/Map_2.dat";
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

		pInstance->Add_GameObject_To_Layer(SCENE_STAGE2,pLayerTag, tData.ObjectID);


		CGameObject* pObject = pInstance->Get_GameObject_By_LayerLastIndex(SCENE_STAGE2, pLayerTag);

		NULL_CHECK_RETURN(pObject, E_FAIL);


		if (lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_None)))
		{
			//매쉬 바꿔주기 
			pObject->Change_Component_by_NewAssign(SCENE_STAGE2, tData.MeshID, TAG_COM(Com_Model));
			((CStaticMapObject*)pObject)->Set_FrustumSize(XMVectorGetX(XMVector3Length((tData.matTransform.XMatrix()).r[0])) * 7.f);



			if ((!lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_GloryTree_Main))) || (!lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_GloryTree_MainB))))
			{
				((CStaticMapObject*)pObject)->Set_PassIndex(3);
				((CStaticMapObject*)pObject)->Set_FrustumSize(50);
			}

			if ((!lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_HatterPlat_CheckerA))) 
				|| (!lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_HatterPlat_CheckerB)))
				|| (!lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_HatterPlat_CheckerC)))
				|| (!lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_HatterPlat_CheckerE)))
				|| (!lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_HatterPlat_CheckerF))))
			{
				((CStaticMapObject*)pObject)->Set_PassIndex(2);
			}


			

		}

		//트렌스폼
		CTransform* pTrans = (CTransform*)(pObject->Get_Component(TAG_COM(Com_Transform)));
		NULL_CHECK_RETURN(pTrans, E_FAIL);
		pTrans->Set_Matrix(tData.matTransform);



	}



	CloseHandle(hFile);


	((CStaticMapObject*)pInstance->Get_GameObject_By_LayerIndex(SCENE_STAGE2, pLayerTag))->Set_FrustumSize(3000);


	return S_OK;
}

HRESULT CScene_Stage2::Ready_Layer_SteamPad(const _tchar * pLayerTag)
{

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_SteamPad), &_float4(154.370285f, 50, 73.54129f, 10)));
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_SteamPad), &_float4(151.148315f, 42, 76.820267f, 10)));
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_SteamPad), &_float4(147.022751f, 34, 79.846466f, 10)));

	return S_OK;
}

HRESULT CScene_Stage2::Ready_Layer_HiddenPad(const _tchar * pLayerTag)
{

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_HiddenPad), &_float4(168.39f, 55.95f, 72.981f,_float(3))));


	return S_OK;
}

HRESULT CScene_Stage2::Ready_Layer_EscalatorPad(const _tchar * pLayerTag)
{
	CEscalatorPad::ESCLATORDESC tDesc;

	tDesc.MoveSpeed = 3.f;
	tDesc.vStartPos = _float3(118.332f, 28.2f, 97.512f);
	tDesc.vDestPos = _float3(95.632f, 22.5f, 89.112f);

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_EscalatorPad), &tDesc));


	tDesc.MoveSpeed = 2.f;
	tDesc.vStartPos = _float3(138.432f, 40.9f, 159.812f);
	tDesc.vDestPos = _float3(149.232f, 40.9f, 141.812f);

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_EscalatorPad), &tDesc));

	return S_OK;
}

HRESULT CScene_Stage2::Ready_Layer_SlidePad(const _tchar * pLayerTag)
{



	CSlidePad::SLIDEDESC tDesc;

	tDesc.vAngle = _float3(25, 40, 36);
	tDesc.vPosition = _float3(183.183f, 65.310f, 91.964f);
	tDesc.bStartCollider = true;
	tDesc.bEndCollider = false;
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_SlideObj), &tDesc));


	tDesc.vAngle = _float3(25, 40, 36);
	tDesc.vPosition = _float3(170.083f, 55.810f, 105.564f);
	tDesc.bStartCollider = false;
	tDesc.bEndCollider = true;
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_SlideObj), &tDesc));




	return S_OK;
}

HRESULT CScene_Stage2::Ready_Layer_ButtonPad(const _tchar * pLayerTag)
{
	CGameInstance* pInstance = g_pGameInstance;

	list<CGameObject*>* ButtonList = pInstance->Get_ObjectList_from_Layer(SCENE_STAGE2, TAG_LAY(Layer_EscalatorPad));
	NULL_CHECK_RETURN(ButtonList, E_FAIL);

	auto iter = ButtonList->begin();

	CButtonPad::BUTTONDESC tDesc;

	tDesc.vAngle = _float3(0,180,0);
	tDesc.vPosition = _float3(69.462f, 23.670f, 69.492f);
	tDesc.eKindsOfObject = 0;
	tDesc.pTargetObject = *iter;
	iter++;
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_ButtonPad), &tDesc));


	tDesc.vAngle = _float3(0,270,0);
	tDesc.vPosition = _float3(133.142f, 43.2f, 129.99f);
	tDesc.eKindsOfObject = 1;
	tDesc.pTargetObject = *iter;
	iter++;
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_ButtonPad), &tDesc));

	return S_OK;
}

HRESULT CScene_Stage2::Ready_Layer_ShapeMemPad(const _tchar * pLayerTag)
{
	CShapeMemoryPad::SHAPEMEMDESC tDesc;

	tDesc.vStartPos = _float3(147.66f, 40.8f, 177.803f);

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_ShapeMemoryPad), &tDesc));

	return S_OK;
}

HRESULT CScene_Stage2::Ready_Layer_ShapeMemBtn(const _tchar * pLayerTag)
{
	CGameInstance* pInstance = g_pGameInstance;

	list<CGameObject*>* ButtonList = pInstance->Get_ObjectList_from_Layer(SCENE_STAGE2, TAG_LAY(Layer_ShapeMemoryPad));
	NULL_CHECK_RETURN(ButtonList, E_FAIL);

	auto iter = ButtonList->begin();

	CShpaeMemButton::BUTTONDESC tDesc;

	tDesc.vAngle = _float3(270.f, 0, 0.f);
	tDesc.vPosition = _float3(151.467f, 51.6f, 189.276f);
	tDesc.eKindsOfObject = 0;
	tDesc.pTargetObject = *iter;
	iter++;
	FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_ShapeMemBtn), &tDesc));

	return S_OK;
}

HRESULT CScene_Stage2::Ready_Layer_PresserObj(const _tchar * pLayerTag)
{

	CPresserObj::PRESSERDESC tDesc;

	tDesc.MoveSpeed = 0.5f;
	tDesc.vStartPos = _float3(69.187f, 30.0f, 118.203f);
	tDesc.vDestPos = _float3(69.187f, 40.5f, 118.203f);

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_Presser), &tDesc));


	tDesc.vStartPos = _float3(69.187f, 30.0f, 107.803f);
	tDesc.vDestPos = _float3(69.187f, 40.5f, 107.803f);

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_Presser), &tDesc));

	tDesc.vStartPos = _float3(69.187f, 30.0f, 97.403f);
	tDesc.vDestPos = _float3(69.187f, 40.5f, 97.403f);

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_Presser), &tDesc));
	return S_OK;
}

HRESULT CScene_Stage2::Ready_Layer_TriggerCollider(const _tchar * pLayerTag)
{
	CSceneChageTriger::SCNCHGTRGDESC tDesc;

	tDesc.eTargetScene = SCENE_STAGE3;
	tDesc.vPosition = _float3(190.1f, 25.16f, 175);
	tDesc.vScale = _float3(2, 1, 13);
	tDesc.vTargetPosition = _float3(193.1f, 25.16f, 175);


	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_TriggerCollider_SceneChager), &tDesc));



	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, L"Stage2_SpwanEyePot", &_float3(76.493f,23.67f,77.06f)));



	return S_OK;
}

HRESULT CScene_Stage2::Ready_Layer_Eyepot(const _tchar * pLayerTag)
{

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE2, pLayerTag, TAG_OP(Prototype_Eyepot), &_float3(76.407188f, 23.67001f, 68.761749f)));


	return S_OK;
}



CScene_Stage2 * CScene_Stage2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CScene_Stage2* pTempLoby = new CScene_Stage2(pDevice,pDeviceContext);

	if (FAILED(pTempLoby->Initialize())) 
	{
		Safe_Release(pTempLoby); 
		MSGBOX("Failed to Creating CScene_Stage2");
		return nullptr;
	}

	return pTempLoby;

}

void CScene_Stage2::Free()
{
	__super::Free();
}
