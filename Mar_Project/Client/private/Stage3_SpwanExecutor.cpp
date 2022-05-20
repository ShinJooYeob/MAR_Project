#include "stdafx.h"
#include "..\public\Stage3_SpwanExecutor.h"
#include "Player.h"
#include "Terrain.h"
#include "Camera_Main.h"
#include "JumpPad.h"
#include "Executor.h"



CStage3_SpwanExecutor::CStage3_SpwanExecutor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CStage3_SpwanExecutor::CStage3_SpwanExecutor(const CStage3_SpwanExecutor & rhs)
	: CMapObject(rhs)
{
}



HRESULT CStage3_SpwanExecutor::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CStage3_SpwanExecutor::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(SPWANTRIGGERDESC));
	}

	FAILED_CHECK(SetUp_Components());

	FAILED_CHECK(Load_ActionCam(L"Stage3_CameAction_5"));
	FAILED_CHECK(Load_ActionCam2(L"Stage1_CameAction_3"));

	
	return S_OK;
}



_int CStage3_SpwanExecutor::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	CGameInstance* pInstance = g_pGameInstance;

	if (m_bSpwanStart)
	{
		static _uint iChecker = 0;
		if (m_SpwanPassedTime < 30)
		{
			m_SpwanPassedTime += fDeltaTime;

			if (iChecker ==0 && m_SpwanPassedTime > 2.5)
			{
				iChecker++;
				FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE3, TAG_LAY(Layer_Monster), TAG_OP(Prototype_Executor), &_float3(128, 13.28f, 256)));
			
			/*	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE3, TAG_LAY(Layer_Monster), TAG_OP(Prototype_Executor), &_float3(128, 27.39f, 226.782f)));
				Set_IsDead();
				*/

			}
			else if (iChecker == 1 && m_SpwanPassedTime > 15.4)
			{

				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeOutIn, 1, { 0,0,0,1 });

				iChecker++;

			}
			else if (iChecker == 2 && m_SpwanPassedTime > 17.4)
			{
				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeOutIn, 1, { 0,0,0,1 });
				iChecker++;

			}
			else if (iChecker == 3 && m_SpwanPassedTime > 19.4)
			{
				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeOutIn, 1, { 0,0,0,1 });
				iChecker++;

			}
			else if (iChecker == 4 && m_SpwanPassedTime > 22.4)
			{
				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeOutIn, 1, { 0,0,0,1 });
				iChecker++;

			}



		}
		else
		{

			//list<CGameObject*>* MonsterLayer = g_pGameInstance->Get_ObjectList_from_Layer(SCENE_STAGE3, TAG_LAY(Layer_Monster));

			//NULL_CHECK_RETURN(MonsterLayer, E_FAIL);

			//if (MonsterLayer->size() == 0)
			//{
			//	CCamera_Main* pCamera = (CCamera_Main*)g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Camera_Main));
			//	NULL_CHECK_BREAK(pCamera);

			//	CAMERAACTION tDesc;

			//	tDesc.vecCamPos = m_vecEndCamPositions;
			//	tDesc.vecLookAt = m_vecEndLookPostions;


			//	CAMACTDESC Return;
			//	Return.fDuration = 0.5f;
			//	Return.vPosition = pCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS);
			//	tDesc.vecCamPos.push_back(Return);

			//	Return.fDuration = 0.5f;
			//	Return.vPosition = Return.vPosition.XMVector() + (pCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_LOOK));
			//	tDesc.vecLookAt.push_back(Return);

			//	pCamera->CamActionStart(tDesc);

			//	Set_IsDead();
			//}
		}


	}


	for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
		m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

	if (!m_bSpwanStart)
		pInstance->Add_CollisionGroup(CollisionType_DynaicObject, this, m_pColliderCom);



	return _int();
}

_int CStage3_SpwanExecutor::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

#ifdef _DEBUG

#endif // _DEBUG
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CStage3_SpwanExecutor::Render()
{
	if (__super::Render() < 0)
		return -1;


#ifdef _DEBUG
	if (!m_bSpwanStart)
		m_pColliderCom->Render();
#endif // _DEBUG

	if (m_bOilDropRender)
	{

		NULL_CHECK_RETURN(m_pModel, E_FAIL);


		FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));


		FAILED_CHECK(__super::SetUp_ConstTable(m_pShaderCom));


		_uint NumMaterial = m_pModel->Get_NumMaterial();

		for (_uint i = 0; i < NumMaterial; i++)
		{

			for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
				FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

			FAILED_CHECK(m_pModel->Render(m_pShaderCom, 0, i));
		}


	}


	return _int();
}

_int CStage3_SpwanExecutor::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

	return _int();
}




void CStage3_SpwanExecutor::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		m_bSpwanStart = true;
		m_pPlayer->Set_ReturnPos(_float3(128.3894f,27.39f,194.531f), _float3(128.3894f, 27.39f, 204.531f));
		m_SpwanPassedTime = 0;


		CCamera_Main* pCamera = (CCamera_Main*)g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Camera_Main));
		NULL_CHECK_BREAK(pCamera);

		CAMERAACTION tDesc;

		tDesc.vecCamPos = m_vecCamPositions;
		tDesc.vecLookAt = m_vecLookPostions;


		CAMACTDESC Return;
		Return.fDuration = 0.5f;
		Return.vPosition = pCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS);
		tDesc.vecCamPos.push_back(Return);

		Return.fDuration = 0.5f;
		Return.vPosition = Return.vPosition.XMVector() + (pCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_LOOK));
		tDesc.vecLookAt.push_back(Return);

		pCamera->CamActionStart(tDesc);

	}
	break;

	default:
		break;
	}


}


HRESULT CStage3_SpwanExecutor::Load_ActionCam(const _tchar * szPath)
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

HRESULT CStage3_SpwanExecutor::Load_ActionCam2(const _tchar * szPath)
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


HRESULT CStage3_SpwanExecutor::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	//FAILED_CHECK(Add_Component(m_eNowSceneNum, L"Grunt_StaticSwpanMesh", TAG_COM(Com_Model), (CComponent**)&m_pModel));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));

	_Matrix WorldMatrix =XMMatrixTranslation(m_tDesc.vPosition.x, m_tDesc.vPosition.y, m_tDesc.vPosition.z);

	m_pTransformCom->Set_Matrix(WorldMatrix);


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(30);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0.f, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));


	ColliderDesc.vScale = _float3(25.7f, 20, 1);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_OBB, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();

	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));


	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);



	return S_OK;
}

CStage3_SpwanExecutor * CStage3_SpwanExecutor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CStage3_SpwanExecutor*	pInstance = new CStage3_SpwanExecutor(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CStage3_SpwanExecutor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CStage3_SpwanExecutor::Clone(void * pArg)
{
	CStage3_SpwanExecutor*	pInstance = new CStage3_SpwanExecutor(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CStage3_SpwanExecutor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStage3_SpwanExecutor::Free()
{
	__super::Free();

	m_vecCamPositions.clear();
	m_vecLookPostions.clear();
	
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pShaderCom);

		

}
