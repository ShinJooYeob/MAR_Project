#include "stdafx.h"
#include "..\public\StageSelect_CatNScene.h"
#include "StageSelectAlice.h"
#include "Camera_Main.h"
#include "GamePlayUI.h"
#include "CatNpc.h"
#include "Scene.h"


CStageSelect_CatNScene::CStageSelect_CatNScene(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CStageSelect_CatNScene::CStageSelect_CatNScene(const CStageSelect_CatNScene & rhs)
	: CMapObject(rhs)
{
}



HRESULT CStageSelect_CatNScene::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CStageSelect_CatNScene::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(SPWANTRIGGERDESC));
	}

	FAILED_CHECK(SetUp_Components());

	FAILED_CHECK(Load_ActionCam(L"StageSelect_CameAction_0"));
	FAILED_CHECK(Load_ActionCam2(L"StageSelect_CameAction_1"));

	
	return S_OK;
}



_int CStageSelect_CatNScene::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;



	CGameInstance* pInstance = g_pGameInstance;

	for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
		m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());


	pInstance->Add_CollisionGroup(CollisionType_DynaicObject, this, m_pColliderCom);

#ifdef _DEBUG
	FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
	



	return _int();
}

_int CStageSelect_CatNScene::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	CGameInstance* pInstance = g_pGameInstance;


	if (m_ActionCount == 1)
	{
		m_Timer += fDeltaTime;

		if (m_Timer > 1 && !m_iChecker)
		{

			//
			CCatNpc* pCat = (CCatNpc*)g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, L"Layer_CatNpc");
			NULL_CHECK_BREAK(pCat);


			pCat->MoveToNext();
			m_iChecker++;
		}
	}
	else if (m_ActionCount == 2)
	{
		m_Timer += fDeltaTime;

		if (m_Timer > 1 && !m_iChecker)
		{

			//
			CCatNpc* pCat = (CCatNpc*)g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, L"Layer_CatNpc");
			NULL_CHECK_BREAK(pCat);
			pCat->MoveToNext();

			m_iChecker++;
		}

	}

	else if (m_ActionCount == 3)
	{
		m_Timer += fDeltaTime;

		if (m_Timer > 1 && !m_iChecker)
		{
			GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeOut, 0.9f, { 0,0,0,1 });

			m_iChecker++;
		}
		else if (m_Timer > 2 && m_iChecker == 1)
		{
			FAILED_CHECK(GetSingle(CUtilityMgr)->Clear_RenderGroup_forSceneChange());
			g_pGameInstance->Get_NowScene()->Set_SceneChanging(SCENE_STAGE1);
			return 0;
		}
	}


	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CStageSelect_CatNScene::Render()
{
	if (__super::Render() < 0)
		return -1;






	return _int();
}

_int CStageSelect_CatNScene::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	return _int();
}




void CStageSelect_CatNScene::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		if (m_ActionCount == 0 )
		{

			CCamera_Main* pCamera = (CCamera_Main*)g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Camera_Main));
			NULL_CHECK_BREAK(pCamera);

			CAMERAACTION tDesc;

			tDesc.vecCamPos = m_vecCamPositions;
			tDesc.vecLookAt = m_vecLookPostions;


			CAMACTDESC Return;
			Return.fDuration = 1.f;
			Return.vPosition = pCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS);
			tDesc.vecCamPos.push_back(Return);

			Return.fDuration = 1.f;
			Return.vPosition = Return.vPosition.XMVector() + (pCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_LOOK));
			tDesc.vecLookAt.push_back(Return);

			pCamera->CamActionStart(tDesc);



			m_pTransformCom->Scaled_All(_float3(10.f, 10.f, 1));
			m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(84.730f, 20.0f, 64.397f));


			m_Timer = 0;
			m_iChecker = 0;
			m_ActionCount++;
		}
		else if(m_ActionCount == 1)
		{

			CCamera_Main* pCamera = (CCamera_Main*)g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Camera_Main));
			NULL_CHECK_BREAK(pCamera);

			CAMERAACTION tDesc;


			tDesc.vecCamPos = m_vecEndCamPositions;
			tDesc.vecLookAt = m_vecEndLookPostions;


			CAMACTDESC Return;
			Return.fDuration = 1.f;
			Return.vPosition = pCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS);
			tDesc.vecCamPos.push_back(Return);

			Return.fDuration = 1.f;
			Return.vPosition = Return.vPosition.XMVector() + (pCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_LOOK));
			tDesc.vecLookAt.push_back(Return);

			pCamera->CamActionStart(tDesc);


			m_pTransformCom->Scaled_All(_float3(10.f, 10.f, 1));
			m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(97.626f, 20.0f, 90.299f));



			m_Timer = 0;
			m_iChecker = 0;
			m_ActionCount++;

		}
		else if (m_ActionCount == 2)
		{
			CCamera_Main* pCamera = (CCamera_Main*)g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Camera_Main));
			NULL_CHECK_BREAK(pCamera);

			CAMERAACTION tDesc;

			tDesc.vecCamPos.clear();
			tDesc.vecLookAt.clear();


			CAMACTDESC Return;
			Return.fDuration = 2;
			Return.vPosition = pCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS) + (m_pPlayerTransform->Get_MatrixState(CTransform::STATE_LOOK) * -4.5f);
			tDesc.vecCamPos.push_back(Return);

			Return.vPosition = Return.vPosition.XMVector() + (pCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_LOOK));
			tDesc.vecLookAt.push_back(Return);

			pCamera->CamActionStart(tDesc);
			m_Timer = 0;
			m_iChecker = 0;
			m_ActionCount++;
		}

	}
	break;

	default:
		break;
	}


}


HRESULT CStageSelect_CatNScene::Load_ActionCam(const _tchar * szPath)
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
}

HRESULT CStageSelect_CatNScene::Load_ActionCam2(const _tchar * szPath)
{


	{
		{
			m_vecEndCamPositions.clear();
			m_vecEndLookPostions.clear();
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

			m_vecEndCamPositions.push_back(tDesc);

		}


		iCount = 0;
		ReadFile(hFile, &(iCount), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iCount; i++)
		{
			ReadFile(hFile, &(tDesc.fDuration), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tDesc.vPosition), sizeof(_float3), &dwByte, nullptr);

			m_vecEndLookPostions.push_back(tDesc);
		}


		CloseHandle(hFile);


	}

	return S_OK;

}

HRESULT CStageSelect_CatNScene::Load_ActionCam3(const _tchar * szPath)
{
	return S_OK;
}


HRESULT CStageSelect_CatNScene::SetUp_Components()
{
#ifdef _DEBUG
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));
#endif // _DEBUG

	//FAILED_CHECK(Add_Component(m_eNowSceneNum, L"Grunt_StaticSwpanMesh", TAG_COM(Com_Model), (CComponent**)&m_pModel));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));

	_Matrix WorldMatrix =XMMatrixTranslation(m_tDesc.vPosition.x, m_tDesc.vPosition.y, m_tDesc.vPosition.z);

	m_pTransformCom->Set_Matrix(WorldMatrix);


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(1);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0.f, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_AABB, &ColliderDesc));


	m_pPlayer = (CStageSelectAlice*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Player)));


	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);




	m_pTransformCom->Scaled_All(_float3(10.f,10.f,1));
	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(75.991f, 20.f, 34.423f));

	return S_OK;
}

CStageSelect_CatNScene * CStageSelect_CatNScene::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CStageSelect_CatNScene*	pInstance = new CStageSelect_CatNScene(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CStageSelect_CatNScene");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CStageSelect_CatNScene::Clone(void * pArg)
{
	CStageSelect_CatNScene*	pInstance = new CStageSelect_CatNScene(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CStageSelect_CatNScene");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStageSelect_CatNScene::Free()
{
	__super::Free();

	m_vecCamPositions.clear();
	m_vecLookPostions.clear();
	
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
#ifdef _DEBUG

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
#endif // _DEBUG

		

}
