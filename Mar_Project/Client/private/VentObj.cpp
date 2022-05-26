#include "stdafx.h"
#include "..\public\VentObj.h"
#include "Player.h"
#include "Terrain.h"
#include "Camera_Main.h"



CVentObj::CVentObj(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CVentObj::CVentObj(const CVentObj & rhs)
	: CMapObject(rhs)
{
}



HRESULT CVentObj::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CVentObj::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(VENTDESC));
	}

	FAILED_CHECK(SetUp_Components());


	//FAILED_CHECK(Set_TerrainTileKinds());

	return S_OK;
}

HRESULT CVentObj::Set_TargetVent(CGameObject * pVentTargetObject)
{
	NULL_CHECK_RETURN(pVentTargetObject, E_FAIL);

	m_tDesc.pTargetVent = (CVentObj*)pVentTargetObject;
	return S_OK;
}

_int CVentObj::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	CGameInstance* pInstance = g_pGameInstance;

	if (m_bVentingStart && m_VentingPassedTime <= 2.f)
	{
		m_VentingPassedTime += fDeltaTime;

		_float3 EasedPos = pInstance->Easing_Vector(TYPE_Linear, m_vTempPlayerPosition, m_vTempTargetPosition, (_float)m_VentingPassedTime, 2);

		if (m_VentingPassedTime > 2.f)
		{
			if (!(m_tDesc.pTargetVent->Come_Over_Player_By_Vent()))
			{
				m_bVentingStart = false;
				m_tDesc.pTargetVent->m_bVentingStart = false;
				m_pPlayer->Set_IsVenting(false, m_vTempTargetPosition);
			}
			EasedPos = m_vTempTargetPosition;
		}

		m_pPlayerTransform->Set_MatrixState(CTransform::STATE_POS, EasedPos);

	}



	m_bIsOnScreen = pInstance->IsNeedToRender( m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) ,5);

	if (m_bIsOnScreen)
	{

		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

		if (!m_bVentingStart)
		{
			pInstance->Add_CollisionGroup(CollisionType_DynaicObject, this, m_pColliderCom);

#ifdef _DEBUG
			FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
		}
	}


	return _int();
}

_int CVentObj::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;



#ifdef _DEBUG

	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
#endif // _DEBUG
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CVentObj::Render()
{
	if (__super::Render() < 0)
		return -1;



	return _int();
}

_int CVentObj::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

	return _int();
}




void CVentObj::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		if (!m_bVentingStart && !m_pPlayer->Get_IsGiant())
		{
			m_bVentingStart = true;

			m_vTempTargetPosition = m_tDesc.vPosition;
			m_vTempPlayerPosition =  m_pPlayerTransform->Get_MatrixState_Float3(CTransform::STATE_POS);
			m_VentingPassedTime = 0;

			m_pPlayer->Set_IsVenting(true, m_vTempTargetPosition.XMVector() - m_vTempPlayerPosition.XMVector());


			CCamera_Main* pCamera =(CCamera_Main*)g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Camera_Main));
			NULL_CHECK_BREAK(pCamera);

			CAMERAACTION tDesc;

			tDesc.vecCamPos = m_vecCamPositions;
			tDesc.vecLookAt = m_vecLookPostions;

		
			pCamera->CamActionStart(tDesc);



		}
		

	}
	break;

	default:
		break;
	}


}

_bool CVentObj::Come_Over_Player_By_Vent()
{
	if (m_bVentingStart) return false;


	m_bVentingStart = true;
	m_VentingPassedTime = 0;

	m_vTempTargetPosition = m_tDesc.vTargetPosition;
	m_vTempPlayerPosition = m_tDesc.vPosition;

	m_pPlayer->Set_IsVenting(true, m_vTempTargetPosition.XMVector() - m_vTempPlayerPosition.XMVector());

	return true;
}

HRESULT CVentObj::Load_ActionCam(const _tchar * szPath)
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


HRESULT CVentObj::SetUp_Components()
{
#ifdef _DEBUG
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));
#endif // _DEBUG

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));

	_Matrix WorldMatrix =XMMatrixTranslation(m_tDesc.vPosition.x, m_tDesc.vPosition.y, m_tDesc.vPosition.z);

	m_pTransformCom->Set_Matrix(WorldMatrix);


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(5.0f, 1.0f, 1.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	static int i = 0;
	if (i < 2)
		ColliderDesc.vPosition = XMVectorSet(0, 1, 0, 0);
	else
		ColliderDesc.vPosition = XMVectorSet(0, 1, 0, 0) + XMVector3Normalize(m_tDesc.vTargetPosition.XMVector() - m_tDesc.vPosition.XMVector()) * 2;

	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	i++;





	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);



	return S_OK;
}

CVentObj * CVentObj::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CVentObj*	pInstance = new CVentObj(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CVentObj");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CVentObj::Clone(void * pArg)
{
	CVentObj*	pInstance = new CVentObj(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CVentObj");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVentObj::Free()
{
	__super::Free();

	m_vecCamPositions.clear();
	m_vecLookPostions.clear();
	
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);

#ifdef _DEBUG
    Safe_Release(m_pRendererCom);
#endif // _DEBUG
}
