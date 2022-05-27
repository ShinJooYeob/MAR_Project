#include "stdafx.h"
#include "..\public\Stage1_SpwanGrunt.h"
#include "Player.h"
#include "Terrain.h"
#include "Camera_Main.h"
#include "Grunt.h"
#include "JumpPad.h"
#include "GamePlayUI.h"



CStage1_SpwanGrunt::CStage1_SpwanGrunt(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CStage1_SpwanGrunt::CStage1_SpwanGrunt(const CStage1_SpwanGrunt & rhs)
	: CMapObject(rhs)
{
}



HRESULT CStage1_SpwanGrunt::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CStage1_SpwanGrunt::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(SPWANTRIGGERDESC));
	}

	FAILED_CHECK(SetUp_Components());

	FAILED_CHECK(Load_ActionCam(L"Stage1_CameAction_0"));
	FAILED_CHECK(Load_ActionCam2(L"Stage1_CameAction_1"));

	
	return S_OK;
}



_int CStage1_SpwanGrunt::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	CGameInstance* pInstance = g_pGameInstance;

#define GruntSwpanTiming 6
	if (m_bSpwanStart )
	{
		//163.5f,23.27f,151.58f
	
		if (m_SpwanPassedTime < 12)
		{

			m_SpwanPassedTime += fDeltaTime;

			if (m_SpwanPassedTime > 12)
			{
				m_SpwanPassedTime = 13;

				return 0;
			}
			else if (m_SpwanPassedTime > GruntSwpanTiming + 5.0)
			{
				m_bOilDropRender = false;

			}
			else if (m_SpwanPassedTime > GruntSwpanTiming + 3.0)
			{
				_float3 EasedScale = g_pGameInstance->Easing_Vector(TYPE_ExpoInOut, _float3(100.0f, 0.1f, 100.0f), _float3(1.0f, 0.1f, 1.0f)
					, _float(m_SpwanPassedTime - (GruntSwpanTiming + 3.0)), 2.f);

				m_pTransformCom->Scaled_All(EasedScale);
			}
			else if (m_SpwanPassedTime > GruntSwpanTiming - 0.5)
			{


				m_pTransformCom->Scaled_All(_float3(100.0f, 0.1f, 100.0f));

			}
			else if (m_SpwanPassedTime > GruntSwpanTiming - 1.5)
			{
				_float3 EasedScale = g_pGameInstance->Easing_Vector(TYPE_ExpoInOut, _float3(1.0f, 0.1f, 1.0f), _float3(100.0f, 0.1f, 100.0f)
					, _float(m_SpwanPassedTime - (GruntSwpanTiming - 1.5)), 1.f);

				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(163.5f, 2.9f, 151.58f));
				m_pTransformCom->Scaled_All(EasedScale);
			}
			else if (m_SpwanPassedTime > GruntSwpanTiming - 2.0)
			{
				m_bOilDropRender = true;


				_float3 EasedPos = g_pGameInstance->Easing_Vector(TYPE_ExpoIn, _float3(163.5f, 23.27f, 151.58f), _float3(163.5f, 1.57f, 151.58f)
					, _float(m_SpwanPassedTime - (GruntSwpanTiming - 2.0)), 0.5);

				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);

			}




			if (m_iChecker == 0)
			{
				list<CGameObject*>* MonsterLayer = g_pGameInstance->Get_ObjectList_from_Layer(SCENE_STAGE1, TAG_LAY(Layer_Monster));

				if (MonsterLayer != nullptr)
				{
					for (auto& pObj : *MonsterLayer)
					{
						pObj->Set_IsDead();
					}
				}
				m_iChecker++;
			}
			else if (m_iChecker == 1 && m_SpwanPassedTime > GruntSwpanTiming)
			{
				FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE1, TAG_LAY(Layer_Monster), TAG_OP(Prototype_Grunt), &_float3(160.f, 2.8701f, 147.668f)));
				m_iChecker++;
			}
			else if (m_iChecker == 2 && m_SpwanPassedTime > GruntSwpanTiming + 0.3)
			{

				FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE1, TAG_LAY(Layer_Monster), TAG_OP(Prototype_Grunt), &_float3(158.576019f, 2.8701f, 156.82135f)));
				m_iChecker++;
			}
			else if (m_iChecker == 3 && m_SpwanPassedTime > GruntSwpanTiming + 0.9)
			{

				FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE1, TAG_LAY(Layer_Monster), TAG_OP(Prototype_Grunt), &_float3(152.774f, 2.8701f, 145.700989f)));
				m_iChecker++;
			}
			else if (m_iChecker == 4 && m_SpwanPassedTime > GruntSwpanTiming + 1.3)
			{
				FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE1, TAG_LAY(Layer_Monster), TAG_OP(Prototype_Grunt), &_float3(168.802f, 2.8701f, 142.16185f)));
				m_iChecker++;
			}
			else if (m_iChecker == 5 && m_SpwanPassedTime > GruntSwpanTiming + 5.5)
			{


				list<CGameObject*>* MonsterLayer = g_pGameInstance->Get_ObjectList_from_Layer(SCENE_STAGE1, TAG_LAY(Layer_Monster));
				for (auto& pObj : *MonsterLayer)
				{
					((CGrunt*)pObj)->Set_SpwanFinished();
				}

			}

		}
		else
		{
			list<CGameObject*>* MonsterLayer = g_pGameInstance->Get_ObjectList_from_Layer(SCENE_STAGE1, TAG_LAY(Layer_Monster));

			NULL_CHECK_RETURN(MonsterLayer, E_FAIL);

			if (MonsterLayer->size() == 0)
			{
				m_SpwanPassedTime += fDeltaTime;


				CJumpPad::JUMPPADDESC tDesc;

				if (!m_JumpPadSpwanCount)
				{

					((CGamePlayUI*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_UI_GamePlay))))->Set_DrawFightUI(false);

					CCamera_Main* pCamera = (CCamera_Main*)g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Camera_Main));
					NULL_CHECK_BREAK(pCamera);

					CAMERAACTION tDesc;


					tDesc.vecCamPos = m_vecCamPositions2;
					tDesc.vecLookAt = m_vecLookPostions2;


					CAMACTDESC Return;
					Return = tDesc.vecCamPos.back();
					Return.fDuration = 1;
					tDesc.vecCamPos.push_back(Return);

					Return.fDuration = 0.5f;
					Return.vPosition = pCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS);
					tDesc.vecCamPos.push_back(Return);

					Return.fDuration = 0.5f;
					Return.vPosition = Return.vPosition.XMVector() + (pCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_LOOK));
					tDesc.vecLookAt.push_back(Return);

					pCamera->CamActionStart(tDesc);




					m_JumpPadSpwanCount++;
				}
				else if (m_JumpPadSpwanCount == 1 && m_SpwanPassedTime > 13.8)
				{


					tDesc.vPosition = _float3(173.862762f, 2.87f, 138.3186f);
					tDesc.vScale = _float3(2);
					tDesc.fPower = 30;

					FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE1, TAG_LAY(Layer_JumpPad), TAG_OP(Prototype_JumpPad), &tDesc));
					m_JumpPadSpwanCount++;
				}
				else if (m_JumpPadSpwanCount == 2 && m_SpwanPassedTime > 15.8)
				{
					tDesc.vPosition = _float3(182.399017f, 10.f, 129.191116f);
					tDesc.vScale = _float3(2);
					tDesc.fPower = 45;

					FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE1, TAG_LAY(Layer_JumpPad), TAG_OP(Prototype_JumpPad), &tDesc));
					m_JumpPadSpwanCount++;


				}
				else if (m_JumpPadSpwanCount == 3 && m_SpwanPassedTime > 23.8)
				{
					tDesc.vPosition = _float3(209.339249f, 19.f, 102.125732f);
					tDesc.vScale = _float3(2);
					tDesc.fPower = 30;

					FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_STAGE1, TAG_LAY(Layer_JumpPad), TAG_OP(Prototype_JumpPad), &tDesc));
					m_JumpPadSpwanCount++;
					Set_IsDead();
					return 0;
				}
			



			}

		}


	}





	for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
		m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

	if (!m_bSpwanStart)
	{
		pInstance->Add_CollisionGroup(CollisionType_DynaicObject, this, m_pColliderCom);

#ifdef _DEBUG
		FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
	}



	return _int();
}

_int CStage1_SpwanGrunt::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

#ifdef _DEBUG

#endif // _DEBUG
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CStage1_SpwanGrunt::Render()
{
	if (__super::Render() < 0)
		return -1;




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

_int CStage1_SpwanGrunt::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	return _int();
}




void CStage1_SpwanGrunt::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		m_bSpwanStart = true;
		m_pPlayer->Set_ReturnPos(_float3(144.58f, 10.f, 129.779f), _float3(160.f, 2.8701f, 147.668f));
		m_SpwanPassedTime = 0;

		((CGamePlayUI*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_UI_GamePlay))))->Set_DrawFightUI(true);

		CCamera_Main* pCamera = (CCamera_Main*)g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Camera_Main));
		NULL_CHECK_BREAK(pCamera);

		CAMERAACTION tDesc;

		tDesc.vecCamPos = m_vecCamPositions;
		tDesc.vecLookAt = m_vecLookPostions;


		CAMACTDESC Return;
		Return.fDuration = 3.5f;
		Return.vPosition = pCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS);
		tDesc.vecCamPos.push_back(Return);

		Return.fDuration = 5.5f;
		Return.vPosition = Return.vPosition.XMVector() + (pCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_LOOK));
		tDesc.vecLookAt.push_back(Return);

		pCamera->CamActionStart(tDesc);



	}
	break;

	default:
		break;
	}


}


HRESULT CStage1_SpwanGrunt::Load_ActionCam(const _tchar * szPath)
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

HRESULT CStage1_SpwanGrunt::Load_ActionCam2(const _tchar * szPath)
{


	{
		{
			m_vecCamPositions2.clear();
			m_vecLookPostions2.clear();
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

			m_vecCamPositions2.push_back(tDesc);

		}


		iCount = 0;
		ReadFile(hFile, &(iCount), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iCount; i++)
		{
			ReadFile(hFile, &(tDesc.fDuration), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tDesc.vPosition), sizeof(_float3), &dwByte, nullptr);

			m_vecLookPostions2.push_back(tDesc);
		}


		CloseHandle(hFile);


	}

	return S_OK;

}


HRESULT CStage1_SpwanGrunt::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, L"Grunt_StaticSwpanMesh", TAG_COM(Com_Model), (CComponent**)&m_pModel));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));

	_Matrix WorldMatrix =XMMatrixTranslation(m_tDesc.vPosition.x, m_tDesc.vPosition.y, m_tDesc.vPosition.z);

	m_pTransformCom->Set_Matrix(WorldMatrix);

	m_pTransformCom->Rotation_CW(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(319));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(30);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0.f, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));


	ColliderDesc.vScale = _float3(3, 10, 30);
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

CStage1_SpwanGrunt * CStage1_SpwanGrunt::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CStage1_SpwanGrunt*	pInstance = new CStage1_SpwanGrunt(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CStage1_SpwanGrunt");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CStage1_SpwanGrunt::Clone(void * pArg)
{
	CStage1_SpwanGrunt*	pInstance = new CStage1_SpwanGrunt(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CStage1_SpwanGrunt");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStage1_SpwanGrunt::Free()
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
