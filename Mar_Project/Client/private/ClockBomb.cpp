#include "stdafx.h"
#include "..\public\ClockBomb.h"
#include "Terrain.h"
#include "GamePlayUI.h"




CClockBomb::CClockBomb(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CWeapon(pDevice,pDeviceContext)
{
}

CClockBomb::CClockBomb(const CClockBomb & rhs)
	: CWeapon(rhs)
{
}

CClockBomb::CClockBomb(const CClockBomb & rhs, _uint Clone2CloneChecker)
	: CWeapon(rhs,1),
	m_bHavetoMaking(true)
{
}

HRESULT CClockBomb::Initialize_Prototype(void * pArg)
{
	__super::Initialize_Prototype(pArg);

	return S_OK;
}

HRESULT CClockBomb::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	FAILED_CHECK(SetUp_Components());

	m_bHavetoMaking = true;
	return S_OK;
}

HRESULT CClockBomb::Initialize_Clone2Clone(void * pArg)
{

	FAILED_CHECK(SetUp_Components());
	m_SpwanPassedTime = 0;
	m_bHavetoMaking = true;

	m_szNameTag = TAG_LAY(Layer_ClockBomb);
	return S_OK;
}

_int CClockBomb::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	if (m_bIsDead) return 0;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);


	m_SpwanPassedTime += fDeltaTime;
	if (!m_bIsOn && m_SpwanPassedTime < 1.3)
	{
		_Matrix Scale = m_pTransformCom->Get_MatrixScale_All();

		_Matrix			TransformMatrix = XMLoadFloat4x4(m_tATBMat.pUpdatedNodeMat) * XMLoadFloat4x4(m_tATBMat.pDefaultPivotMat);
		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);


		m_pTransformCom->Set_Matrix(TransformMatrix* m_tWeaponDesc.pParantTransform->Get_WorldMatrix());

		m_bIsOn = false;
		m_LevitationTime = 0.3;
		CGameInstance* pInstance = GetSingle(CGameInstance);

		//m_LevitationTime += fDeltaTime;
		//_float fGravity = _float((m_LevitationTime) * (m_LevitationTime)* -29.4f);

		//m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);


		CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

		_uint eNowTile = Tile_End;
		_float3 CaculatedPos = pTerrain->PutOnTerrain(&m_bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + XMVectorSet(0,0.1f,0,0), m_vOldPos.XMVector(), nullptr, &eNowTile);

		if (eNowTile == Tile_None)
		{
			m_bIsOn = true;
			m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos.XMVector() - XMVectorSet(0, 0.1f, 0, 0));
		}
		else if (m_bIsOn)
		{
			m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos.XMVector() - XMVectorSet(0, 0.1f, 0, 0));
		}
	

	}
	else
	{
		if (!m_bIsOn)
		{

			CGameInstance* pInstance = GetSingle(CGameInstance);

			m_LevitationTime += fDeltaTime;
			_float fGravity = _float((m_LevitationTime) * (m_LevitationTime)* -29.4f);
			m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);


			CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

			_uint eNowTile = Tile_End;
			_float3 CaculatedPos = pTerrain->PutOnTerrain(&m_bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + XMVectorSet(0, 0.1f, 0, 0), m_vOldPos.XMVector(), nullptr, &eNowTile);

			if (eNowTile == Tile_None)
			{
				m_bIsOn = true;
				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos.XMVector() - XMVectorSet(0, 0.1f, 0, 0));
			}
			else if (m_bIsOn)
			{
				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos.XMVector() - XMVectorSet(0, 0.1f, 0, 0));
			}

		}
		else
		{
			if (m_pModel->Get_NowAnimIndex() == 4)
				m_pModel->Change_AnimIndex_ReturnTo_Must(5, 0, 0.15, true);


			 
			if (m_ClockPassedTime > 0)
			{
				m_ClockPassedTime -= fDeltaTime;


				if (m_ClockPassedTime < 2.5)
					m_pModel->Change_AnimIndex(1, 0.15, true);


				if (m_ClockPassedTime < 0)
				{
					Set_IsDead();
				}


			}

		}


	}








	m_BoneMatrix = m_pTransformCom->Get_WorldMatrix();
	for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
		m_pColliderCom->Update_Transform(i, m_BoneMatrix.XMatrix());

	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime));
	FAILED_CHECK(Adjust_AnimMovedTransform(fDeltaTime));

	g_pGameInstance->Add_CollisionGroup(CollisionType_PlayerWeapon, this, m_pColliderCom);
#ifdef _DEBUG
	FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG


	return _int();
}

_int CClockBomb::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsDead) return 0;


	//if (m_bIsOnScreen)	
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CClockBomb::Render()
{

	NULL_CHECK_RETURN(m_pModel, E_FAIL);





	_float4x4 ShaderMat = m_BoneMatrix.TransposeXMatrix();
	m_pShaderCom->Set_RawValue("g_AttechMatrix", &ShaderMat, sizeof(_float4x4));



	CGameInstance* pInstance = GetSingle(CGameInstance);

	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamPosition", &pInstance->Get_TargetPostion_float4(PLV_CAMERA), sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamLookDir", &pInstance->Get_TargetPostion_float4(PLV_CAMLOOK), sizeof(_float4)));


	//const LIGHTDESC* pLightDesc = pInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
	//NULL_CHECK_RETURN(pLightDesc, -1);

	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightVector", &(pLightDesc->vVector), sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4)));
	//FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));



	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));

	FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_MskingTextrue", 0));
	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{

		if (m_bHavetoMaking && i > 0) continue;

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		
		if (m_bHavetoMaking)
		{
			FAILED_CHECK(m_pModel->Render(m_pShaderCom, 7, i, "g_BoneMatrices"));
		}
		else 
		{
			FAILED_CHECK(m_pModel->Render(m_pShaderCom, 6, i, "g_BoneMatrices"));
		}
	}


	return _int();
}

_int CClockBomb::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	NULL_CHECK_RETURN(m_pModel, E_FAIL);

	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{
		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 11, i, "g_BoneMatrices"));

	}
	return _int();
}

void CClockBomb::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
}



HRESULT CClockBomb::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Mesh_ClockBomb), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	m_pModel->Change_AnimIndex(4, 0, true);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Texture_MskTex), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));
	m_pTextureCom->Change_TextureLayer(L"ClockBomb");

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);	
	ColliderDesc.vPosition = _float4(-1.065f, 0.306f, -1.229f,1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));

	


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));


	return S_OK;
}

HRESULT CClockBomb::Adjust_AnimMovedTransform(_double fDeltatime)
{
	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	_double PlayRate = m_pModel->Get_PlayRate();

	if (iNowAnimIndex != m_iOldAnimIndex || PlayRate > 0.95)
		m_iAdjMovedIndex = 0;



	if (PlayRate <= 0.95)
	{
		switch (iNowAnimIndex)
		{
		case 5:
			if (m_bHavetoMaking && PlayRate > 0.9)
			{
				m_ClockPassedTime = 5;

				CGamePlayUI* pGameObj = (CGamePlayUI*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_UI_GamePlay)));
				NULL_CHECK_RETURN(pGameObj, E_FAIL);
				pGameObj->Set_DrawClockBombUI();
				
				m_bHavetoMaking = false;
			}
			
			break;
		}
	}
	else
	{
		switch (iNowAnimIndex)
		{
		case 5:
			if (m_bHavetoMaking)
			{
				m_ClockPassedTime = 5;

				CGamePlayUI* pGameObj = (CGamePlayUI*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_UI_GamePlay)));
				NULL_CHECK_RETURN(pGameObj, E_FAIL);
				pGameObj->Set_DrawClockBombUI();

				m_bHavetoMaking = false;
			}
			
			break;
		}

	}

	return S_OK;
}

CClockBomb * CClockBomb::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CClockBomb*	pInstance = new CClockBomb(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(nullptr)))
	{
		MSGBOX("Failed to Created CClockBomb");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CClockBomb::Clone(void * pArg)
{
	CClockBomb*	pInstance = new CClockBomb(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CClockBomb");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CClockBomb::Clone2Clone(void * pArg)
{
	CClockBomb*	pInstance = new CClockBomb(*this,0);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created C2CClockBomb");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CClockBomb::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	
}
