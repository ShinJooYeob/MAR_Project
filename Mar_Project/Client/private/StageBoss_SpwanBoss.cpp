#include "stdafx.h"
#include "..\public\StageBoss_SpwanBoss.h"
#include "Player.h"
#include "Terrain.h"
#include "Camera_Main.h"
#include "GamePlayUI.h"


CStageBoss_SpwanBoss::CStageBoss_SpwanBoss(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CStageBoss_SpwanBoss::CStageBoss_SpwanBoss(const CStageBoss_SpwanBoss & rhs)
	: CMapObject(rhs),
	m_pColliderCom(false)
{
}



HRESULT CStageBoss_SpwanBoss::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CStageBoss_SpwanBoss::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(SPWANTRIGGERDESC));
	}

	FAILED_CHECK(SetUp_Components());

	FAILED_CHECK(Load_ActionCam(L"StageBoss_CameAction_0"));
	FAILED_CHECK(Load_ActionCam2(L"StageBoss_CameAction_1"));

	
	return S_OK;
}



_int CStageBoss_SpwanBoss::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	CGameInstance* pInstance = g_pGameInstance;

	if (m_bSpwanStart)
	{
		
		if (m_iChecker == 0)
		{
				m_SpwanPassedTime+=fDeltaTime;

				_float EasedPosX = (g_pGameInstance->Easing(TYPE_SinOut, -100.f, 288.f, (_float)m_SpwanPassedTime ,10.f));
				_float EasedPosY = (g_pGameInstance->Easing(TYPE_SinInOut, 217.f, 125.f, (_float)m_SpwanPassedTime , 10.f));
				_float EasedPosZ = (g_pGameInstance->Easing(TYPE_SinIn, -55.f, 196.f, (_float)m_SpwanPassedTime , 10.f));

				if (m_SpwanPassedTime > 10.f)
				{
					m_SpwanPassedTime = 10;
					EasedPosX = 288.f;
					EasedPosY = 125.f;
					EasedPosZ = 196.f;
					g_pGameInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0)->vVector.x = EasedPosX;
					g_pGameInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0)->vVector.y = EasedPosY;
					g_pGameInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0)->vVector.z = EasedPosZ;

					FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_BOSS, TAG_LAY(Layer_Monster), TAG_OP(Prototype_DollMaker), &_float3(85, 10, 78)));
					m_iChecker++;
				}


				g_pGameInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0)->vVector.x = EasedPosX;;
				g_pGameInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0)->vVector.y = EasedPosY;
				g_pGameInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0)->vVector.z = EasedPosZ;;
			//	g_pGameInstance->Relocate_LightDesc(tagLightDesc::TYPE_DIRECTIONAL, 0, _float4(-100.f, 122.f, -55, 0).XMVector());


				//LightPosLuminece: X: 288.000000, Y : 125.000000, Z : 196.000000
			}

	}


	for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
		m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());


	_float3 Temp = m_pColliderCom->Get_ColliderPosition();

	if (!m_bSpwanStart)
	{
		pInstance->Add_CollisionGroup(CollisionType_DynaicObject, this, m_pColliderCom);

#ifdef _DEBUG
		FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
	}



	return _int();
}

_int CStageBoss_SpwanBoss::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;


	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CStageBoss_SpwanBoss::Render()
{
	if (__super::Render() < 0)
		return -1;






	return _int();
}

_int CStageBoss_SpwanBoss::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	return _int();
}

void CStageBoss_SpwanBoss::PlayCamAction()
{
	CCamera_Main* pCamera = (CCamera_Main*)g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Camera_Main));
	NULL_CHECK_BREAK(pCamera);

	CAMERAACTION tDesc;

	tDesc.vecCamPos = m_vecCamPositions;
	tDesc.vecLookAt = m_vecLookPostions;


	CAMACTDESC Return;
	Return.fDuration = 0.3f;
	Return.vPosition = m_pPlayerTransform->Get_MatrixState(CTransform::STATE_POS) + XMVectorSet(0.00000001f, 0.7f, 0, 0);
	tDesc.vecCamPos.push_back(Return);

	Return.fDuration = 1.0f;
	Return.vPosition = m_pPlayerTransform->Get_MatrixState(CTransform::STATE_POS) + XMVectorSet(0,0.8f,0,0);
	tDesc.vecLookAt.push_back(Return);
	Return.fDuration = 0.3f;
	Return.vPosition = m_pPlayerTransform->Get_MatrixState(CTransform::STATE_POS) + XMVectorSet(0, 0.8f, 0, 0);
	tDesc.vecLookAt.push_back(Return);

	pCamera->CamActionStart(tDesc);

}




void CStageBoss_SpwanBoss::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		m_bSpwanStart = true;
		//m_pPlayer->Set_ReturnPos(_float3(75.830f, 23.670f, 77.509f), _float3(84.321f, 23.670f, 95.669f));
		m_SpwanPassedTime = 0;
		m_iChecker = 0;
		((CGamePlayUI*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_UI_GamePlay))))->Set_DrawFightUI(true);


		CCamera_Main* pCamera = (CCamera_Main*)g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Camera_Main));
		NULL_CHECK_BREAK(pCamera);

		CAMERAACTION tDesc;

		tDesc.vecCamPos = m_vecEndCamPositions;
		tDesc.vecLookAt = m_vecEndLookPostions;


		CAMACTDESC Return;
		Return.fDuration = 2.f;
		Return.vPosition = pCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS);
		tDesc.vecCamPos.push_back(Return);

		Return.fDuration = 2.f;
		Return.vPosition = Return.vPosition.XMVector() + (pCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_LOOK));
		tDesc.vecLookAt.push_back(Return);

		pCamera->CamActionStart(tDesc);


	}
	break;

	default:
		break;
	}


}


HRESULT CStageBoss_SpwanBoss::Load_ActionCam(const _tchar * szPath)
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

HRESULT CStageBoss_SpwanBoss::Load_ActionCam2(const _tchar * szPath)
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


HRESULT CStageBoss_SpwanBoss::SetUp_Components()
{
#ifdef _DEBUG
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));
#endif // _DEBUG

	//FAILED_CHECK(Add_Component(m_eNowSceneNum, L"Grunt_StaticSwpanMesh", TAG_COM(Com_Model), (CComponent**)&m_pModel));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));

	_Matrix WorldMatrix = XMMatrixRotationY(XMConvertToRadians(60))* XMMatrixTranslation(m_tDesc.vPosition.x, m_tDesc.vPosition.y, m_tDesc.vPosition.z);

	m_pTransformCom->Set_Matrix(WorldMatrix);


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));


	ColliderDesc.vScale = _float3(47, 17, 1);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_OBB, &ColliderDesc));




	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));
	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);
	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));
	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);



	return S_OK;
}

CStageBoss_SpwanBoss * CStageBoss_SpwanBoss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CStageBoss_SpwanBoss*	pInstance = new CStageBoss_SpwanBoss(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CStageBoss_SpwanBoss");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CStageBoss_SpwanBoss::Clone(void * pArg)
{
	CStageBoss_SpwanBoss*	pInstance = new CStageBoss_SpwanBoss(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CStageBoss_SpwanBoss");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStageBoss_SpwanBoss::Free()
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
