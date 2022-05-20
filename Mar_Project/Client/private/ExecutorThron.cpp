#include "stdafx.h"
#include "..\public\ExecutorThron.h"
#include "Terrain.h"
#include "Player.h"



CExecutorThron::CExecutorThron(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonsterWeapon(pDevice,pDeviceContext)
{
}

CExecutorThron::CExecutorThron(const CExecutorThron & rhs)
	: CMonsterWeapon(rhs)
{
}

HRESULT CExecutorThron::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CExecutorThron::Initialize_Clone(void * pArg)
{
	//FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{

		m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Player)));

		NULL_CHECK_RETURN(m_pPlayer, E_FAIL);
		memcpy(&m_tDesc, pArg, sizeof(THRONDESC));

	}

	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
	{
		_float3 vPos = (*(_float3*)pArg);
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, vPos);
	}

	m_fStartTimer = 0;

	CUtilityMgr* pUtilMgr = GetSingle(CUtilityMgr);
	m_fTurningTime = 0;
	//m_vRotAxis = m_vSpoutDir;
	return S_OK;
}

_int CExecutorThron::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);

	//m_pTransformCom->MovetoDir_bySpeed(m_tDesc.MoveDir.XMVector(), 15 , fDeltaTime);

	m_fStartTimer += fDeltaTime;

	if (m_fStartTimer < 0.5)
	{
		_float EasedValue = g_pGameInstance->Easing(TYPE_ExpoIn, 0, (_float)m_tDesc.MeshKinds, (_float)m_fStartTimer, 0.5f);
		
		if (!m_SummonOther && m_tDesc.MeshKinds < 12 && m_fStartTimer > 0.15)
		{
			m_SummonOther = true;

			THRONDESC tDesc;
			tDesc = m_tDesc;
			tDesc.MoveDir = XMVectorLerp(XMVector3Normalize(m_tDesc.MoveDir.XMVector()),
				XMVector3Normalize(XMVectorSetY(((CTransform*)m_pPlayer->Get_Component(TAG_COM(Com_Transform)))->Get_MatrixState(CTransform::STATE_POS) - m_tDesc.vPosition.XMVector(), 0))
				, 0.3f);
			tDesc.vPosition = m_tDesc.vPosition.XMVector() + m_tDesc.MoveDir.XMVector() * 2 * (1.f + _float(m_tDesc.MeshKinds )* 0.1f) ;
			tDesc.MeshKinds = m_tDesc.MeshKinds + 1;

			g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_Thron), &tDesc);
		}


		m_pTransformCom->Scaled_All( _float3(EasedValue));
	}
	else if(m_fStartTimer >= 0.5  + (12 - _int(m_tDesc.MeshKinds)) * 0.1)
	{
		Set_IsDead();
	}


	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));

	if (m_bIsOnScreen)
	{
		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

		g_pGameInstance->Add_CollisionGroup(CollisionType_MonsterWeapon, this, m_pColliderCom);

	}


	return _int();
}

_int CExecutorThron::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	//if (!m_bIsPlayerCloser) return _int();



	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CExecutorThron::Render()
{
	if (__super::Render() < 0)
		return -1;

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG


	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader_ApplyPivot(m_pShaderCom, "g_WorldMatrix"));

	CGameInstance* pInstance = GetSingle(CGameInstance);
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));


	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamPosition", &pInstance->Get_TargetPostion_float4(PLV_CAMERA), sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamLookDir", &pInstance->Get_TargetPostion_float4(PLV_CAMLOOK), sizeof(_float4)));


	const LIGHTDESC* pLightDesc = pInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
	NULL_CHECK_RETURN(pLightDesc, -1);

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightVector", &(pLightDesc->vVector), sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4)));



	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 1, i));
	}


	return _int();
}

_int CExecutorThron::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

	return _int();
}

void CExecutorThron::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		pConflictedCollider->Set_Conflicted();
		((CPlayer*)(pConflictedObj))->Add_Dmg_to_Player(rand() % 2 + 3);

	}
	break;
	case Engine::CollisionType_Terrain:
		break;

	default:
		break;
	}
}

HRESULT CExecutorThron::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));


	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_SK_QBattleTowerTentacle_Giant), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	

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
	ColliderDesc.vScale = _float3(3, 1, 1);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 1.5f, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));


	ColliderDesc.vScale = _float3(1, 2.5f, 1);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 1.25f, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_AABB, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();





	return S_OK;
}


CExecutorThron * CExecutorThron::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CExecutorThron*	pInstance = new CExecutorThron(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CExecutorThron");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CExecutorThron::Clone(void * pArg)
{
	CExecutorThron*	pInstance = new CExecutorThron(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CExecutorThron");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CExecutorThron::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);

	
}
