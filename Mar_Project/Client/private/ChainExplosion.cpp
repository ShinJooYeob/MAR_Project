#include "stdafx.h"
#include "..\public\ChainExplosion.h"
#include "Terrain.h"
#include "Player.h"



CChainExplosion::CChainExplosion(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonsterWeapon(pDevice,pDeviceContext)
{
}

CChainExplosion::CChainExplosion(const CChainExplosion & rhs)
	: CMonsterWeapon(rhs)
{
}

HRESULT CChainExplosion::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CChainExplosion::Initialize_Clone(void * pArg)
{
	//FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{

		m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Player)));

		NULL_CHECK_RETURN(m_pPlayer, E_FAIL);
		memcpy(&m_tDesc, pArg, sizeof(THRONDESC));

	}

	FAILED_CHECK(SetUp_Components());
	FAILED_CHECK(SetUp_ParticleDesc());

	if (pArg != nullptr)
	{
		_float3 vPos = (*(_float3*)pArg);
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, vPos);
	}

	m_fStartTimer = 0;
	m_fTurningTime = 0;

	GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);

	return S_OK;
}

_int CChainExplosion::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);

	//m_pTransformCom->MovetoDir_bySpeed(m_tDesc.MoveDir.XMVector(), 15 , fDeltaTime);

	m_fStartTimer += fDeltaTime;

	if (m_fStartTimer < 0.5)
	{

		
		if (!m_SummonOther && m_tDesc.MeshKinds < 7 && m_fStartTimer > 0.15)
		{
			m_SummonOther = true;

			THRONDESC tDesc;
			tDesc = m_tDesc;

			tDesc.MoveDir = XMVectorLerp(XMVector3Normalize(m_tDesc.MoveDir.XMVector()),
				XMVector3Normalize(XMVectorSetY(((CTransform*)m_pPlayer->Get_Component(TAG_COM(Com_Transform)))->Get_MatrixState(CTransform::STATE_POS) - m_tDesc.vPosition.XMVector(), 0))
				, 0.3f);

			tDesc.vPosition = m_tDesc.vPosition.XMVector() + m_tDesc.MoveDir.XMVector() * 2 * (1.f + _float(m_tDesc.MeshKinds )* 0.1f) ;
			tDesc.MeshKinds = m_tDesc.MeshKinds + 1;

			g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), L"Prototype_ChainExplosion", &tDesc);
		}



	}
	else if(m_fStartTimer >= 0.5)
	{
		Set_IsDead();
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

_int CChainExplosion::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	//if (!m_bIsPlayerCloser) return _int();



#ifdef _DEBUG
	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
#endif // _DEBUG

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CChainExplosion::Render()
{
	if (__super::Render() < 0)
		return -1;




	return _int();
}

_int CChainExplosion::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	return _int();
}

void CChainExplosion::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
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

HRESULT CChainExplosion::SetUp_Components()
{
#ifdef _DEBUG
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));
#endif // _DEBUG

	
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
	ColliderDesc.vPosition = _float4(0, 1.5f, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));






	return S_OK;
}

HRESULT CChainExplosion::SetUp_ParticleDesc()
{

	m_tParticleDesc = PARTICLEDESC();

	m_tParticleDesc.eParticleTypeID = Particle_Ball;

	m_tParticleDesc.FollowingTarget = m_pTransformCom;

	m_tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	m_tParticleDesc.szTextureLayerTag = L"Explosion";
	m_tParticleDesc.iSimilarLayerNum = 2;

	m_tParticleDesc.TextureChageFrequency = 1;
	m_tParticleDesc.vTextureXYNum = _float2(5, 4);

	m_tParticleDesc.TotalParticleTime = 0.1f;
	m_tParticleDesc.EachParticleLifeTime = 0.34f;

	m_tParticleDesc.MaxParticleCount = 5;

	m_tParticleDesc.SizeChageFrequency = 2;
	m_tParticleDesc.ParticleSize = _float3(0.2f);
	m_tParticleDesc.ParticleSize2 = _float3(7);

	m_tParticleDesc.ColorChageFrequency = 0;
	m_tParticleDesc.TargetColor = _float4(1.f, 1.f, 1.f, 0.7f);
	m_tParticleDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 1.f);


	m_tParticleDesc.Particle_Power = 0;
	m_tParticleDesc.PowerRandomRange = _float2(0.5f, 1.0f);

	m_tParticleDesc.vUp = _float3(0, 1, 0);

	m_tParticleDesc.MaxBoundaryRadius = 3;

	m_tParticleDesc.m_bIsUI = false;
	m_tParticleDesc.m_bUIDepth = 0;

	//m_tParticleDesc.ParticleStartRandomPosMin = _float3(-0.05f, 0.7f, -0.05f);
	//m_tParticleDesc.ParticleStartRandomPosMax = _float3(0.05f, 0.75f, 0.05f);
	m_tParticleDesc.ParticleStartRandomPosMin = _float3(-0.05f, 0.0f, -0.05f);
	m_tParticleDesc.ParticleStartRandomPosMax = _float3(0.05f, 0.1f, 0.05f);

	m_tParticleDesc.DepthTestON = true;
	m_tParticleDesc.AlphaBlendON = true;

	m_tParticleDesc.m_fAlphaTestValue = 0.1f;
	m_tParticleDesc.m_iPassIndex = 3;


	return S_OK;
}


CChainExplosion * CChainExplosion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CChainExplosion*	pInstance = new CChainExplosion(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CChainExplosion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CChainExplosion::Clone(void * pArg)
{
	CChainExplosion*	pInstance = new CChainExplosion(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CChainExplosion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChainExplosion::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);

#ifdef _DEBUG
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
#endif // _DEBUG
	
}
