#include "stdafx.h"
#include "..\public\MovableColumBtn.h"
#include "Player.h"
#include "Terrain.h"
#include "ClockBomb.h"
#include "MovableColum.h"
#include "Camera_Main.h"



CMovableColumBtn::CMovableColumBtn(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CMovableColumBtn::CMovableColumBtn(const CMovableColumBtn & rhs)
	: CMapObject(rhs)
{
}

HRESULT CMovableColumBtn::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CMovableColumBtn::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(COLUMBTNDESC));
	}

	FAILED_CHECK(SetUp_Components());


	//FAILED_CHECK(Set_TerrainTileKinds());

	return S_OK;
}

_int CMovableColumBtn::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	FAILED_CHECK(Update_ButtonAnim(fDeltaTime));



	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(
		m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) ,3);

	if (m_bIsOnScreen)
	{

		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

		g_pGameInstance->Add_CollisionGroup(CollisionType_DynaicObject, this, m_pColliderCom);
#ifdef _DEBUG
		FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
	}
	else
	{
		if (m_bChecker)
		{
			g_pGameInstance->Add_CollisionGroup(CollisionType_DynaicObject, this, m_pColliderCom);
#ifdef _DEBUG
			FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
		}


	}

	m_bChecker = false;

	return _int();
}

_int CMovableColumBtn::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	LetWorkButton(!m_bChecker);

	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CMovableColumBtn::Render()
{
	if (__super::Render() < 0)
		return -1;



	NULL_CHECK_RETURN(m_pModel, E_FAIL);
	NULL_CHECK_RETURN(m_pModel, E_FAIL);

	FAILED_CHECK(__super::SetUp_ConstTable(m_pShaderCom));


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	_uint NumMaterial = m_pSubModel->Get_NumMaterial();
	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pSubModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pSubModel->Render(m_pShaderCom, 1, i));
	}


	_float4x4		ShaderWorldMatrix;
	_float4x4		ButtonMat = m_pTransformCom->Get_WorldFloat4x4();
	ButtonMat._42 -= m_ButtonHight;

	XMStoreFloat4x4(&ShaderWorldMatrix, ButtonMat.TransposeXMatrix());
	m_pShaderCom->Set_RawValue("g_WorldMatrix", &ShaderWorldMatrix, sizeof(_float4x4));

	NumMaterial = m_pModel->Get_NumMaterial();
	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 1, i));
	}



	return _int();
}

_int CMovableColumBtn::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;
	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	_uint NumMaterial = m_pSubModel->Get_NumMaterial();
	for (_uint i = 0; i < NumMaterial; i++)
	{

		FAILED_CHECK(m_pSubModel->Render(m_pShaderCom, 9, i));
	}
	_float4x4		ShaderWorldMatrix;
	_float4x4		ButtonMat = m_pTransformCom->Get_WorldFloat4x4();
	ButtonMat._42 -= m_ButtonHight;

	XMStoreFloat4x4(&ShaderWorldMatrix, ButtonMat.TransposeXMatrix());
	m_pShaderCom->Set_RawValue("g_WorldMatrix", &ShaderWorldMatrix, sizeof(_float4x4));


	NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{
		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 9, i));
	}
	return _int();

}




void CMovableColumBtn::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{

		if (m_pPlayerTransform->Get_MatrixState_Float3(CTransform::STATE_POS).y < m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).y + 0.6f - m_ButtonHight)
		{
			m_bChecker = true;
			_float3 PlayerPos = m_pPlayerTransform->Get_MatrixState_Float3(CTransform::STATE_POS);
			PlayerPos.y = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).y + 0.6f - m_ButtonHight;

			m_pPlayer->Set_NotLevitation();
			m_pPlayerTransform->Set_MatrixState(CTransform::STATE_POS, PlayerPos);

			
		}



	}
	break;
	

	default:
		break;
	}


}

void CMovableColumBtn::LetWorkButton(_bool bBool)
{
	if (m_PassedTime > 1 && bBool != m_bIsUp)
	{
		m_bIsUp = bBool;
		m_PassedTime = 0;


		if (!m_bIsUp)
		{

			{
				SOUNDDESC tSoundDesc;

				tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
				tSoundDesc.vMinMax = _float2(0, 25);
				tSoundDesc.fTargetSound = 0.3f;
				wstring SoundTrack = L"";
				SoundTrack = L"MapObject_ppad_down.ogg";

				//SoundTrack = L"MapObject_shrinkflower_open.ogg";

				g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
			}


			_bool	TempBool =  true;
			switch (m_tDesc.eKindsOfObject)
			{
			case 0:
				TempBool = ((CMovableColum*)m_tDesc.pTargetObject)->IsSameNowTrunedKinds(0);
				break;
			case 1:
				TempBool = ((CMovableColum*)m_tDesc.pTargetObject)->IsSameNowTrunedKinds(2);
				break;
			case 2:
				TempBool = ((CMovableColum*)m_tDesc.pTargetObject)->IsSameNowTrunedKinds(1);
				break;
			default:
				break;
			}

			if (!TempBool)
			{

				CCamera_Main* pCamera = (CCamera_Main*)g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Camera_Main));
				NULL_CHECK_BREAK(pCamera);

				CAMERAACTION tDesc;

				tDesc.vecCamPos = m_vecCamPositions;
				tDesc.vecLookAt = m_vecLookPostions;

				CAMACTDESC Return;
				Return.fDuration = 0.5;
				Return.vPosition = m_pPlayerTransform->Get_MatrixState(CTransform::STATE_POS);

				tDesc.vecCamPos.push_back(Return);

				Return.vPosition = m_pPlayerTransform->Get_MatrixState(CTransform::STATE_POS) + m_pPlayerTransform->Get_MatrixState(CTransform::STATE_LOOK);

				tDesc.vecLookAt.push_back(Return);

				pCamera->CamActionStart(tDesc);
			}
		}
		else
		{

			{
				SOUNDDESC tSoundDesc;

				tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
				tSoundDesc.vMinMax = _float2(0, 25);
				tSoundDesc.fTargetSound = 0.3f;
				wstring SoundTrack = L"";
				SoundTrack = L"MapObject_ppad_down.ogg";

				//SoundTrack = L"MapObject_shrinkflower_open.ogg";

				g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
			}

		}

	}

}

HRESULT CMovableColumBtn::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));

	_Matrix WorldMatrix = 	XMMatrixTranslation(m_tDesc.vPosition.x, m_tDesc.vPosition.y, m_tDesc.vPosition.z);

	m_pTransformCom->Set_Matrix(WorldMatrix);


	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_Button), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_Button_Frame), TAG_COM(Com_SubModel), (CComponent**)&m_pSubModel));


	ZeroMemory(m_NevRectPoint, sizeof(_float4) * 4);



	m_NevRectPoint[0] = _float4(0.8f, 0.6f, 0.8f, 1);
	m_NevRectPoint[1] = _float4(0.8f, 0.6f, -0.8f, 1);
	m_NevRectPoint[2] = _float4(-0.8f, 0.6f, -0.8f, 1);
	m_NevRectPoint[3] = _float4(-0.8f, 0.6f, 0.8f, 1);


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(4., 1.0f, 1.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.f, 0, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));


	ColliderDesc.vScale = _float3(2.8f,2.8f, 2.8f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, -0.475f, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_AABB, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();





	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);



	FAILED_CHECK(Load_ActionCam(L"Stage3_CameAction_4"));

	return S_OK;
}

HRESULT CMovableColumBtn::Update_ButtonAnim(_double fDeltaTime)
{
	if (m_PassedTime > 1)
	{
		m_bSoundChecker = false;
		return S_FALSE;
	}

	m_PassedTime += fDeltaTime;

	if (!m_bIsUp)
	{
		if (!m_bSoundChecker && m_PassedTime > 0.35f)
		{
			m_bSoundChecker = true;

			SOUNDDESC tSoundDesc;

			tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
			tSoundDesc.vMinMax = _float2(0, 25);
			tSoundDesc.fTargetSound = 0.3f;
			wstring SoundTrack = L"";
			SoundTrack = L"MapObject_ppad_down.ogg";

			//SoundTrack = L"MapObject_shrinkflower_open.ogg";

			g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);

		}

		m_ButtonHight = g_pGameInstance->Easing(TYPE_ExpoInOut, 0, 0.3f, (_float)m_PassedTime, 1);

		if (m_ButtonHight > 0.2)
		{

			switch (m_tDesc.eKindsOfObject)
			{
			case 0:
				((CMovableColum*)m_tDesc.pTargetObject)->Start_Turning_Colum(0);
				break;
			case 1:
				((CMovableColum*)m_tDesc.pTargetObject)->Start_Turning_Colum(2);
				break;
			case 2:
				((CMovableColum*)m_tDesc.pTargetObject)->Start_Turning_Colum(1);
				break;
			default:
				break;
			}
		}

	}
	else
	{
		if (!m_bSoundChecker && m_PassedTime > 0.35f)
		{
			m_bSoundChecker = true;

			SOUNDDESC tSoundDesc;

			tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
			tSoundDesc.vMinMax = _float2(0, 25);
			tSoundDesc.fTargetSound = 0.3f;
			wstring SoundTrack = L"";
			SoundTrack = L"MapObject_ppad_down.ogg";

			//SoundTrack = L"MapObject_shrinkflower_open.ogg";

			g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);

		}
		m_ButtonHight = g_pGameInstance->Easing(TYPE_ExpoInOut, 0.3f, 0, (_float)m_PassedTime, 1);
	}

	return S_OK;
}

HRESULT CMovableColumBtn::Load_ActionCam(const _tchar * szPath)
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


CMovableColumBtn * CMovableColumBtn::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CMovableColumBtn*	pInstance = new CMovableColumBtn(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CMovableColumBtn");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMovableColumBtn::Clone(void * pArg)
{
	CMovableColumBtn*	pInstance = new CMovableColumBtn(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CMovableColumBtn");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMovableColumBtn::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pSubModel);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
