#include "stdafx.h"
#include "..\public\CardPeice.h"
#include "Terrain.h"
#include "Player.h"



CCardPiece::CCardPiece(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonsterWeapon(pDevice,pDeviceContext)
{
}

CCardPiece::CCardPiece(const CCardPiece & rhs)
	: CMonsterWeapon(rhs)
{
}

HRESULT CCardPiece::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CCardPiece::Initialize_Clone(void * pArg)
{
	//FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{

		m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Player)));

		NULL_CHECK_RETURN(m_pPlayer, E_FAIL);


		memcpy(&m_tDesc, pArg, sizeof(CARDPIECEDESC));

	}

	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
	{
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_tDesc.vPosition);
	}

	m_fStartTimer = 0;

	CUtilityMgr* pUtilMgr = GetSingle(CUtilityMgr);
	m_vSpoutDir = (pUtilMgr->RandomFloat3(-10000.f, 10000.f)).Get_Nomalize();
	m_fRandPower = (pUtilMgr->RandomFloat(10, 20.f));

	Add_JumpPower(pUtilMgr->RandomFloat(10, 15.f));
	m_fTurningTime = 0;
	//m_vRotAxis = m_vSpoutDir;
	return S_OK;
}

_int CCardPiece::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);

	m_pTransformCom->MovetoDir_bySpeed(m_tDesc.MoveDir.XMVector(), 15 , fDeltaTime);

	m_fTurningTime += fDeltaTime;

	if (m_fTurningTime > 0.00)
	{

		m_fStartTimer += _float(fDeltaTime);

		if (!m_bIsSpout)
		{
			_float fGravity = 0;
			if (m_fJumpPower > 0)
			{
				m_fJumpPower = _float(m_fMaxJumpPower * (m_fStartTimer - 1.f)* (m_fStartTimer - 1.f));
				fGravity = m_fJumpPower;

				if (m_fJumpPower <= 2.0f)
				{
					m_fJumpPower = 0;
					m_fStartTimer = _float(fDeltaTime);
				}
			}
			else
			{
				fGravity = _float((m_fStartTimer) * (m_fStartTimer) * -49.0f);
			}

			m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);
			m_pTransformCom->MovetoDir_bySpeed(m_vSpoutDir.XMVector(), m_fRandPower, fDeltaTime);

			CGameInstance* pInstance = GetSingle(CGameInstance);

			CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

			_bool bIsOn = false;
			_uint eTileKinds = Tile_End;
			pTerrain->PutOnTerrain(&bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector(),nullptr, &eTileKinds);

				m_pTransformCom->Turn_CW(m_vSpoutDir.XMVector(), fDeltaTime * 7);

			if (bIsOn)
			{
				m_bIsSpout = true;
				Set_IsDead();
			}

		}
	}


	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));

	if (m_bIsOnScreen)
	{
		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

		g_pGameInstance->Add_CollisionGroup(CollisionType_MonsterWeapon, this, m_pColliderCom);
#ifdef _DEBUG
		FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG

	}


	return _int();
}

_int CCardPiece::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	//if (!m_bIsPlayerCloser) return _int();



	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CCardPiece::Render()
{
	if (__super::Render() < 0)
		return -1;



	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader_ApplyPivot(m_pShaderCom, "g_WorldMatrix"));

	CGameInstance* pInstance = GetSingle(CGameInstance);
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));


	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamPosition", &pInstance->Get_TargetPostion_float4(PLV_CAMERA), sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamLookDir", &pInstance->Get_TargetPostion_float4(PLV_CAMLOOK), sizeof(_float4)));


	//const LIGHTDESC* pLightDesc = pInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
	//NULL_CHECK_RETURN(pLightDesc, -1);

	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightVector", &(pLightDesc->vVector), sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4)));



	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 1, i));
	}


	return _int();
}

_int CCardPiece::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;
	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));



	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{
		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 9, i));
	}

	return _int();
}

void CCardPiece::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		if (!lstrcmp(pConflictedObj->Get_NameTag(), L"Alice"))
		{
			pConflictedCollider->Set_Conflicted();
			((CPlayer*)(pConflictedObj))->Add_Dmg_to_Player(rand() % 2 + 3);

		}
	}
	break;
	case Engine::CollisionType_Terrain:
		break;

	default:
		break;
	}
}

HRESULT CCardPiece::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));


	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(COMPONENTPROTOTYPEID(m_tDesc.MeshKinds)), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	

	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = GetSingle(CUtilityMgr)->RandomFloat(0.06f, 0.08f);
	tDesc.fRotationPerSec = XMConvertToRadians(GetSingle(CUtilityMgr)->RandomFloat(50,70));
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0.3f, 0);


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));



	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	///* For.Com_AABB */
	//ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));


	//Pivot  : 0.030000f , -10.630148f , -10.410143f , 1
	//size  : 6.080047f , 3.000000f , 3.000000f  
	ColliderDesc.vScale = _float3(4, 1, 1);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));






	return S_OK;
}

void CCardPiece::Add_JumpPower(_float power)
{

	m_fJumpPower = m_fMaxJumpPower = power;

}

CCardPiece * CCardPiece::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CCardPiece*	pInstance = new CCardPiece(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CCardPiece");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCardPiece::Clone(void * pArg)
{
	CCardPiece*	pInstance = new CCardPiece(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CCardPiece");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCardPiece::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);

	
}
