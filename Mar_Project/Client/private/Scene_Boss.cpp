#include "stdafx.h"
#include "..\Public\Scene_Boss.h"
#include "Scene_Loading.h"
#include "Player.h"
#include "StaticMapObject.h"
#include "Camera_Main.h"



CScene_Boss::CScene_Boss(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CScene(pDevice,pDeviceContext)
{
}





HRESULT CScene_Boss::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;



	FAILED_CHECK(Ready_Light());

	FAILED_CHECK(Ready_Layer_MainCamera(TAG_LAY(Layer_Camera_Main)));
	FAILED_CHECK(Ready_Layer_SkyBox(TAG_LAY(Layer_SkyBox)));
	FAILED_CHECK(Ready_Layer_Terrain(TAG_LAY(Layer_Terrain)));
	
	
	FAILED_CHECK(Ready_Layer_Player(TAG_LAY(Layer_Player)));
	FAILED_CHECK(Ready_Layer_UI(TAG_LAY(Layer_UI_GamePlay)));
	

	//FAILED_CHECK(Ready_Layer_JumpPad(TAG_LAY(Layer_JumpPad)));
	//FAILED_CHECK(Ready_Layer_SteamPad(TAG_LAY(Layer_SteamPad)));
	//FAILED_CHECK(Ready_Layer_HiddenPad(TAG_LAY(Layer_HiddenPad)));
	//FAILED_CHECK(Ready_Layer_TeethObj(TAG_LAY(Layer_TeethObj)));
	//FAILED_CHECK(Ready_Layer_RoseObj(TAG_LAY(Layer_RoseObj)));
	//FAILED_CHECK(Ready_Layer_BreakableObj(TAG_LAY(Layer_Breakable)))
	
	
	FAILED_CHECK(Ready_Layer_StaticMapObj(TAG_LAY(Layer_StaticMapObj)));


	FAILED_CHECK(Ready_Layer_DollMaker(TAG_LAY(Layer_Monster)));
	
	//FAILED_CHECK(Ready_Layer_Grunt(TAG_LAY(Layer_Monster)));
	//FAILED_CHECK(Ready_Layer_Eyepot(TAG_LAY(Layer_Monster)));
	//FAILED_CHECK(Ready_Layer_WaspInk(TAG_LAY(Layer_Monster)));
	//FAILED_CHECK(Ready_Layer_WaspArrow(TAG_LAY(Layer_Monster)));
	//FAILED_CHECK(Ready_Layer_Executor(TAG_LAY(Layer_Monster)));


		
			
			
		
			

	return S_OK;
}

_int CScene_Boss::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	if (g_pGameInstance->Get_DIKeyState(DIK_P)&DIS_Down)
		FAILED_CHECK(Ready_Layer_BreakableObj(TAG_LAY(Layer_Breakable)));

	

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

_int CScene_Boss::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

	return 0;
}



HRESULT CScene_Boss::Ready_Light()
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
	pPlayer->Renew_Player(_float3(50, 11, 50));
	

	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_UI(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_UIGamePlay)));
	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_JumpPad(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_JumpPad),&_float3(7, 20, 7) ));

	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_SteamPad(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_SteamPad), &_float4(9, 22.5, 7, 5)));

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_SteamPad), &_float4(9, 22.5, 5, 10)));

	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_HiddenPad(const _tchar * pLayerTag)
{
	///for (_uint i = 0; i < 4; i++)
	///{
	///	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_HiddenPad), &_float4(_float(11 + i *3), 22, _float(7 +i*2), _float(0 +i))));
	///}

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_HiddenPad), &_float4(_float(11), 20, _float(7), _float(0))));
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_HiddenPad), &_float4(_float(95), 20, _float(85), _float(1))));
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_HiddenPad), &_float4(_float(100), 20, _float(90), _float(2))));
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_HiddenPad), &_float4(_float(105), 20, _float(95), _float(3))));

	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_TeethObj(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_TeethObj), &_float4(13, 22, 7,0)));

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_TeethObj), &_float4(13, 22, 9, 1)));


	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_RoseObj(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_RoseObj), &_float3(14, 22, 7)));

	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_BreakableObj(const _tchar * pLayerTag)
{

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_BreakableObj), &_float4(16, 21, 7, _float(Prototype_Mesh_BreakableBox))));
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_BreakableObj), &_float4(19, 21, 7, _float(Prototype_Mesh_BreakableBarrel))));
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_BreakableObj), &_float4(22, 21, 7, _float(Prototype_Mesh_GiftBasket))));

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

	// �����ڵ����� �˸��� BOM
	//DWORD wc = 0xFF;
	//ReadFile(hFile, &wc, 3, &dwByte, NULL);

	while (true)
	{
		OBJELEMENT	tData{};
		_tchar szBuffer[MAX_PATH] = L"";
		// key �� �ε�
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

		//��ä �������ֱ�
		if(!lstrcmp(tData.ObjectID, L"EditorCursor")) continue;

		pInstance->Add_GameObject_To_Layer(SCENE_BOSS,pLayerTag, tData.ObjectID);


		CGameObject* pObject = pInstance->Get_GameObject_By_LayerLastIndex(SCENE_BOSS, pLayerTag);

		NULL_CHECK_RETURN(pObject, E_FAIL);


		if (lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_None)))
		{
			//�Ž� �ٲ��ֱ� 
			pObject->Change_Component_by_NewAssign(SCENE_BOSS, tData.MeshID, TAG_COM(Com_Model));
			((CStaticMapObject*)pObject)->Set_FrustumSize(XMVectorGetX(XMVector3Length((tData.matTransform.XMatrix()).r[0])) * 7.f);



			if ((!lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_GloryTree_Main))) || (!lstrcmp(tData.MeshID, TAG_CP(Prototype_Mesh_GloryTree_MainB))))
			{
				((CStaticMapObject*)pObject)->Set_PassIndex(3);
				((CStaticMapObject*)pObject)->Set_FrustumSize(50);
			}

		}

		//Ʈ������
		CTransform* pTrans = (CTransform*)(pObject->Get_Component(TAG_COM(Com_Transform)));
		NULL_CHECK_RETURN(pTrans, E_FAIL);
		pTrans->Set_Matrix(tData.matTransform);



	}



	CloseHandle(hFile);


	((CStaticMapObject*)pInstance->Get_GameObject_By_LayerIndex(SCENE_BOSS, pLayerTag))->Set_FrustumSize(3000);


	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_Executor(const _tchar * pLayerTag)
{


		FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_Executor), &_float3(210, 22, 45)));

	

	

	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_Eyepot(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_Eyepot), &_float3(6, 22, 8)));

	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_Grunt(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_Grunt), &_float3(6, 22, 8)));

	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_WaspArrow(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_WaspArrow), &_float3(0, 21, 4)));

	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_WaspInk(const _tchar * pLayerTag)
{
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_WaspInk), &_float3(6, 22, 8)));

	return S_OK;
}

HRESULT CScene_Boss::Ready_Layer_DollMaker(const _tchar * pLayerTag)
{

	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, pLayerTag, TAG_OP(Prototype_DollMaker), &_float3(50, 15, 16)));


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
