#include "stdafx.h"
#include "..\public\ThrowOilBullet.h"
#include "Terrain.h"
#include "Player.h"
#include "Monster.h"



CThrowOilBullet::CThrowOilBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonsterWeapon(pDevice,pDeviceContext)
{
}

CThrowOilBullet::CThrowOilBullet(const CThrowOilBullet & rhs)
	: CMonsterWeapon(rhs)
{
}

HRESULT CThrowOilBullet::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CThrowOilBullet::Initialize_Clone(void * pArg)
{
	//FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{

		m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Player)));

		NULL_CHECK_RETURN(m_pPlayer, E_FAIL);


		memcpy(&m_tDesc, pArg, sizeof(BREAKEDGAZBODESC));

	}

	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
	{
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_tDesc.vPosition);
	}

	m_fStartTimer = 0;

	CUtilityMgr* pUtilMgr = GetSingle(CUtilityMgr);

	m_bIsHavetoMoving = true;

	m_pTransformCom->Scaled_All(_float3(2));


	FAILED_CHECK(SetUp_ParticleDesc());

	GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);

	return S_OK;
}

_int CThrowOilBullet::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	if (!m_bIsHavetoMoving)return false;

	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);

	m_fStartTimer += (_float)fDeltaTime;

	if (m_fStartTimer > 3)
	{
		m_pTransformCom->Set_IsOwnerDead(true);
		Set_IsDead();
	}

	m_pTransformCom->MovetoDir_bySpeed(m_tDesc.MoveDir.XMVector(), 25 , fDeltaTime);

	


	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));

	if (m_bIsOnScreen)
	{
		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());


		if (!m_tDesc.MeshKinds)
		{
			g_pGameInstance->Add_CollisionGroup(CollisionType_MonsterWeapon, this, m_pColliderCom);
		}
		else
		{
			g_pGameInstance->Add_CollisionGroup(CollisionType_PlayerWeapon, this, m_pColliderCom);
		}
#ifdef _DEBUG
		FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG

	}


	return _int();
}

_int CThrowOilBullet::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	//if (!m_bIsPlayerCloser) return _int();



	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CThrowOilBullet::Render()
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

_int CThrowOilBullet::LightRender()
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

void CThrowOilBullet::Set_Position(_float3 vPosition)
{
}

void CThrowOilBullet::Set_MovingStart(_float3 vDir)
{
	m_tDesc.MoveDir =vDir.Get_Nomalize();
	m_tDesc.MeshKinds = 1;
	m_fStartTimer = 0;

}

void CThrowOilBullet::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
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
		else
		{
			_float Radian =  XMVectorGetX(XMVector3Dot(XMVector3Normalize(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_LOOK)),
				XMVector3Normalize(m_tDesc.MoveDir.XMVector())));

			if (Radian < 0)
			{
				_Vector Dir = XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0) - XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS), 0);

				m_pPlayer->Set_UmbrellaReflected(true,m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + Dir, Dir);
				GetSingle(CUtilityMgr)->SlowMotionStart();
				Set_MovingStart(Dir);
			}
		}
	}
	break;
	case Engine::CollisionType_Monster:
	{
		pConflictedCollider->Set_Conflicted();
		GetSingle(CUtilityMgr)->SlowMotionStart();
		((CMonster*)(pConflictedObj))->Add_Dmg_to_Monster(5);
		
	}
	break;
	case Engine::CollisionType_Terrain:
		break;

	default:
		break;
	}
}

HRESULT CThrowOilBullet::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));


	FAILED_CHECK(Add_Component(m_eNowSceneNum, L"Grunt_StaticSwpanMesh", TAG_COM(Com_Model), (CComponent**)&m_pModel));
	

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
	ColliderDesc.vScale = _float3(1.5f, 1, 1);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));



	m_pPlayerTransfrom = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));
	NULL_CHECK_RETURN(m_pPlayerTransfrom, E_FAIL);


	return S_OK;
}

HRESULT CThrowOilBullet::SetUp_ParticleDesc()
{

	m_tParticleDesc = PARTICLEDESC();

	m_tParticleDesc.eParticleTypeID = Particle_Straight;

	m_tParticleDesc.FollowingTarget = m_pTransformCom;

	m_tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	m_tParticleDesc.szTextureLayerTag = L"Explosion";
	m_tParticleDesc.iSimilarLayerNum = 2;

	m_tParticleDesc.TextureChageFrequency = 1;
	m_tParticleDesc.vTextureXYNum = _float2(5, 4);

	m_tParticleDesc.TotalParticleTime = 7.f;
	m_tParticleDesc.EachParticleLifeTime = 0.34f;

	m_tParticleDesc.MaxParticleCount = 10;

	m_tParticleDesc.SizeChageFrequency = 2;
	m_tParticleDesc.ParticleSize = _float3(3);
	m_tParticleDesc.ParticleSize2 = _float3(0.2f);

	m_tParticleDesc.ColorChageFrequency = 0;
	m_tParticleDesc.TargetColor = _float4(1.f, 1.f, 1.f, 0.7f);
	m_tParticleDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 1.f);


	m_tParticleDesc.Particle_Power = 3;
	m_tParticleDesc.PowerRandomRange = _float2(0.8f, 1.0f);

	m_tParticleDesc.vUp = _float3(0, 1, 0);

	m_tParticleDesc.MaxBoundaryRadius = 3;

	m_tParticleDesc.m_bIsUI = false;
	m_tParticleDesc.m_bUIDepth = 0;

	m_tParticleDesc.ParticleStartRandomPosMin = _float3(-0.05f, 0.00f, -0.05f);
	m_tParticleDesc.ParticleStartRandomPosMax = _float3(0.05f, 0.01f, 0.05f);

	m_tParticleDesc.DepthTestON = true;
	m_tParticleDesc.AlphaBlendON = true;

	m_tParticleDesc.m_fAlphaTestValue = 0.1f;
	m_tParticleDesc.m_iPassIndex = 3;

	return S_OK;
}



CThrowOilBullet * CThrowOilBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CThrowOilBullet*	pInstance = new CThrowOilBullet(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CThrowOilBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CThrowOilBullet::Clone(void * pArg)
{
	CThrowOilBullet*	pInstance = new CThrowOilBullet(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CThrowOilBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CThrowOilBullet::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);

	
}
