#include "stdafx.h"
#include "..\public\GrenadeBullet.h"
#include "Monster.h"
#include "BreakableObj.h"
#include "Terrain.h"



CGrenadeBullet::CGrenadeBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CBullet(pDevice,pDeviceContext)
{
}

CGrenadeBullet::CGrenadeBullet(const CGrenadeBullet & rhs)
	: CBullet(rhs)
{
}

HRESULT CGrenadeBullet::Initialize_Prototype(void * pArg)
{
	__super::Initialize_Prototype(pArg);

	return S_OK;
}

HRESULT CGrenadeBullet::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_OilBullet), TAG_COM(Com_Model), (CComponent**)&m_pModel));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));
	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.2f, 1.2f, 1.2f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));

	CUtilityMgr* pUtilMgr = GetSingle(CUtilityMgr);
	m_fTotalLifeTime = pUtilMgr->RandomFloat(2.f, 3.f);
	m_vTargetDir = m_vTargetDir.XMVector() + pUtilMgr->RandomFloat3(-0.1f, 0.1f).XMVector();


	m_szNameTag = L"Layer_GranadeBullet";


	m_pTerrain = (CTerrain*)g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain));
	NULL_CHECK_RETURN(m_pTerrain, E_FAIL);
	Safe_AddRef(m_pTerrain);

	FAILED_CHECK(Ready_ParticleDesc());

	return S_OK;
}

void CGrenadeBullet::Set_IsDead()
{
	m_pTransformCom->Set_IsOwnerDead();
	m_bIsDead = true;
}

_int CGrenadeBullet::Update(_double fDeltaTime)
{
	//if (__super::Update(fDeltaTime) < 0)
	//	return -1;
	if (m_bIsDead) return 0;


	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);

	if (m_bDeadAnimStart)
	{
		m_DeadAnimPassedTime += fDeltaTime;




		if (m_DeadAnimPassedTime < 1.5)
		{
			if (m_iChecker == 0 )
			{
				CUtilityMgr* pUtil = GetSingle(CUtilityMgr);

				m_vecParticleDesc[1].FixedTarget = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					//+ XMVector3Normalize( g_pGameInstance->Get_TargetPostion_Vector(PLV_CAMERA) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS)) * 0.2f;
				pUtil->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[1]);

				_Vector ToCamDir = XMVector3Normalize(g_pGameInstance->Get_TargetPostion_Vector(PLV_CAMERA) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
				_Vector ToCamR = XMVector3Normalize(XMVector2Cross(XMVectorSet(0, 1, 0, 0), ToCamDir));


				m_vecParticleDesc[3].FixedTarget = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) 
					+ XMVector3Normalize(XMVector2Cross(ToCamDir, ToCamR)) * pUtil->RandomFloat(-1,1) + ToCamR* pUtil->RandomFloat(-1, 1)+
					+ToCamDir* 0.2f;
				pUtil->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[3]);


				m_vecParticleDesc[4].FixedTarget = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
				pUtil->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[4]);

				pUtil->Start_InstanceParticle(m_eNowSceneNum, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 2);
				pUtil->Start_InstanceParticle(m_eNowSceneNum, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 3);
				m_iChecker++;
			}
			else if (m_iChecker == 1 && m_DeadAnimPassedTime > 0.15)
			{
				m_vecParticleDesc[2].FixedTarget = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[2]);
				m_iChecker++;
			}

			_float Sacle = g_pGameInstance->Easing(TYPE_SinInOut, 0.6f, 6, _float(m_DeadAnimPassedTime), 1.5f);
			m_pTransformCom->Scaled_All(_float3(Sacle));
		}
		else
		{
			m_pTransformCom->Set_IsOwnerDead();
			m_bIsDead = true;
		}

	}
	else
	{
		m_vecParticleDesc[0].FixedTarget = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
		GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[0]);
		m_pTransformCom->MovetoDir(m_vTargetDir.XMVector(), fDeltaTime);

		m_fLifeTime += _float(fDeltaTime);

		if (m_fLifeTime > m_fTotalLifeTime && !m_bDeadAnimStart)
		{
			m_bDeadAnimStart = true;
			m_DeadAnimPassedTime = 0;
		}
		_float EasedValue = g_pGameInstance->Easing(TYPE_Linear, 20, -100, m_fLifeTime, m_fTotalLifeTime);
		m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1, 0, 0), EasedValue, fDeltaTime);
		m_pTransformCom->LookDir(m_vTargetDir.XMVector() + XMVectorSet(0, 1, 0, 0)* EasedValue);
	}

	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));

	if (m_bIsOnScreen && !m_bIsHit)
	{
		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

		g_pGameInstance->Add_CollisionGroup(CollisionType_PlayerWeapon, this, m_pColliderCom);

	}

	return _int();
}

_int CGrenadeBullet::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsDead) return 0;



	FAILED_CHECK(Set_Player_On_Terrain());


	if (m_bIsOnScreen && m_DeadAnimPassedTime < 0.5f)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CGrenadeBullet::Render()
{

	NULL_CHECK_RETURN(m_pModel, E_FAIL);

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG

	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	CGameInstance* pInstance = GetSingle(CGameInstance);

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));


	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 1, i));
	}


	return _int();
}

_int CGrenadeBullet::LateRender()
{

	return _int();
}

void CGrenadeBullet::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Monster:
	{
		if (lstrcmp(pConflictedObj->Get_NameTag(), TAG_LAY(Layer_Breakable)))
		{
			pConflictedCollider->Set_Conflicted();
			((CMonster*)(pConflictedObj))->Add_Dmg_to_Monster(15);


			m_bIsHit = true;
			if (!m_bDeadAnimStart)
			{
				m_bDeadAnimStart = true;
				m_DeadAnimPassedTime = 0;
			}

		}
		else
		{
			pConflictedCollider->Set_Conflicted();
			//GetSingle(CUtilityMgr)->SlowMotionStart();
			((CBreakableObj*)(pConflictedObj))->Add_Dmg_To_BreakableObj(15);

			m_bIsHit = true;
			if (!m_bDeadAnimStart)
			{
				m_bDeadAnimStart = true;
				m_DeadAnimPassedTime = 0;
			}

		}

	}
	break;
	case Engine::CollisionType_Terrain:
		break;

	default:
		break;
	}
}

HRESULT CGrenadeBullet::Set_Player_On_Terrain()
{
	if (m_bDeadAnimStart) return S_FALSE;
	CGameInstance* pInstance = GetSingle(CGameInstance);

	CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

	_bool bIsOn = false;

	_uint eNowTile = Tile_End;
	_float3 CaculatedPos = pTerrain->PutOnTerrain_Stage2(&bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector(), nullptr, &eNowTile);



	if (eNowTile == Tile_None || eNowTile == Tile_DynamicNoneTile || bIsOn)
	{
		m_bDeadAnimStart = true;
		m_DeadAnimPassedTime = 0;
	}
	
	return S_OK;
}

HRESULT CGrenadeBullet::Ready_ParticleDesc()
{

	m_vecParticleDesc.clear();
	m_vecParticleDesc.reserve(5);

	//GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[0]);
	PARTICLEDESC tDesc;

	{
		tDesc = PARTICLEDESC();
		tDesc.eParticleTypeID = Particle_Cone;

		tDesc.FollowingTarget = nullptr;

		tDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
		tDesc.szTextureLayerTag = L"Dust3";
		tDesc.iSimilarLayerNum = 1;

		tDesc.TextureChageFrequency = 3;
		tDesc.vTextureXYNum = _float2(4, 4);

		tDesc.TotalParticleTime = 0.1f;
		tDesc.EachParticleLifeTime = 1.f;
		tDesc.MaxParticleCount = 1;

		tDesc.SizeChageFrequency = 1;
		tDesc.ParticleSize = _float3(0.3f);
		tDesc.ParticleSize2 = _float3(0.7f);

		tDesc.ColorChageFrequency = 0;
		tDesc.TargetColor = _float4(1.f, 0.843f, 0.341f, 1.f);
		tDesc.TargetColor2 = _float4(-0.3f, -0.3f, -0.3f, 1.f);


		tDesc.Particle_Power = 0.7f;
		tDesc.PowerRandomRange = _float2(0.5f, 1.5f);

		tDesc.vUp = _float3(0, 1, 0);

		tDesc.MaxBoundaryRadius = 5;

		tDesc.m_bIsUI = false;
		tDesc.m_bUIDepth = 0;

		tDesc.ParticleStartRandomPosMin = _float3(0.f);
		tDesc.ParticleStartRandomPosMax = _float3(0.f);

		tDesc.DepthTestON = true;
		tDesc.AlphaBlendON = true;

		tDesc.m_fAlphaTestValue = 0.1f;
		tDesc.m_iPassIndex = 3;

		m_vecParticleDesc.push_back(tDesc);
	}
	/////////1//////////////////
	
	{
		tDesc = PARTICLEDESC();

		tDesc.eParticleTypeID = Particle_Ball;

		tDesc.FollowingTarget = nullptr;

		tDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
		tDesc.szTextureLayerTag = L"Explosion";
		tDesc.iSimilarLayerNum = 1;

		tDesc.TextureChageFrequency = 1;
		tDesc.vTextureXYNum = _float2(5, 4);

		tDesc.TotalParticleTime = 0.1f;
		tDesc.EachParticleLifeTime = 0.5f;

		tDesc.MaxParticleCount = 1;

		tDesc.SizeChageFrequency = 1;
		tDesc.ParticleSize = _float3(0.2f, 0.2f, 0.2f);
		tDesc.ParticleSize2 = _float3(7.f, 7.f, 7.f);

		tDesc.ColorChageFrequency = 0;
		tDesc.TargetColor = _float4(1.f, 1.f, 1.f, 1.f);
		tDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 1.f);


		tDesc.Particle_Power = 0;
		tDesc.PowerRandomRange = _float2(0.5f, 1.0f);

		tDesc.vUp = _float3(0, 1, 0);

		tDesc.MaxBoundaryRadius = 3;

		tDesc.m_bIsUI = false;
		tDesc.m_bUIDepth = 0;

		tDesc.ParticleStartRandomPosMin = _float3(0.f);
		tDesc.ParticleStartRandomPosMax = _float3(0.f);

		tDesc.DepthTestON = true;
		tDesc.AlphaBlendON = true;

		tDesc.m_fAlphaTestValue = 0.1f;
		tDesc.m_iPassIndex = 3;


		m_vecParticleDesc.push_back(tDesc);
	}
	/////////2//////////////////
	{
		tDesc = PARTICLEDESC();
		tDesc.eParticleTypeID = Particle_Spread;

		tDesc.FollowingTarget = nullptr;

		tDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
		tDesc.szTextureLayerTag = L"Dust3";
		tDesc.iSimilarLayerNum = 1;

		tDesc.TextureChageFrequency = 6;
		tDesc.vTextureXYNum = _float2(4, 4);

		tDesc.TotalParticleTime = 0.1f;
		tDesc.EachParticleLifeTime = 2.f;
		tDesc.MaxParticleCount = 10;

		tDesc.SizeChageFrequency = 1;
		tDesc.ParticleSize = _float3(0.7f);
		tDesc.ParticleSize2 = _float3(1.5f);

		tDesc.ColorChageFrequency = 1;
		tDesc.TargetColor = _float4(1.f, 0.843f, 0.341f, 1.f);
		tDesc.TargetColor2 = _float4(-0.3f, -0.3f, -0.3f, 0.5f);


		tDesc.Particle_Power = 1.5f;
		tDesc.PowerRandomRange = _float2(0.8f, 1.2f);

		tDesc.vUp = _float3(0, 1, 0);

		tDesc.MaxBoundaryRadius = 10000;

		tDesc.m_bIsUI = false;
		tDesc.m_bUIDepth = 0;

		tDesc.ParticleStartRandomPosMin = _float3(0.f);
		tDesc.ParticleStartRandomPosMax = _float3(0.f);

		tDesc.DepthTestON = true;
		tDesc.AlphaBlendON = true;

		tDesc.m_fAlphaTestValue = 0.1f;
		tDesc.m_iPassIndex = 3;

		m_vecParticleDesc.push_back(tDesc);
	}
	///////////3/////////////////////////////////////////////////////////////
	
	{

		tDesc = PARTICLEDESC();

		tDesc.eParticleTypeID = Particle_Fixed;

		tDesc.FollowingTarget = nullptr;

		tDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
		tDesc.szTextureLayerTag = L"Face";
		tDesc.iSimilarLayerNum = 1;

		tDesc.TextureChageFrequency = 1;
		tDesc.vTextureXYNum = _float2(1, 1);

		tDesc.TotalParticleTime = 0.1f;
		tDesc.EachParticleLifeTime = 0.7f;

		tDesc.MaxParticleCount = 1;

		tDesc.SizeChageFrequency = 1;
		tDesc.ParticleSize = _float3(1.5f);
		tDesc.ParticleSize2 = _float3(1.7f);

		tDesc.ColorChageFrequency = 2;
		tDesc.TargetColor = _float4(1.f, 1.f, 1.f, 0.f);
		tDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 0.5f);


		tDesc.Particle_Power = 0;
		tDesc.PowerRandomRange = _float2(0.5f, 1.0f);

		tDesc.vUp = _float3(0, 1, 0);

		tDesc.MaxBoundaryRadius = 3;

		tDesc.m_bIsUI = false;
		tDesc.m_bUIDepth = 0;

		tDesc.ParticleStartRandomPosMin = _float3(0.f);
		tDesc.ParticleStartRandomPosMax = _float3(0.f);

		tDesc.DepthTestON = true;
		tDesc.AlphaBlendON = false;

		tDesc.m_fAlphaTestValue = 0.1f;
		tDesc.m_iPassIndex = 3;


		m_vecParticleDesc.push_back(tDesc);

	}
	///////////3/////////////////////////////////////////////////////////////

	{

		tDesc = PARTICLEDESC();

		tDesc.eParticleTypeID = Particle_Fixed_LookFree;

		tDesc.FollowingTarget = nullptr;

		tDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
		tDesc.szTextureLayerTag = L"Circle";
		tDesc.iSimilarLayerNum = 1;

		tDesc.TextureChageFrequency = 1;
		tDesc.vTextureXYNum = _float2(1, 1);

		tDesc.TotalParticleTime = 0.1f;
		tDesc.EachParticleLifeTime = 1.2f;

		tDesc.MaxParticleCount = 1;

		tDesc.SizeChageFrequency = 1;
		tDesc.ParticleSize = _float3(0.5f);
		tDesc.ParticleSize2 = _float3(2.5f);

		tDesc.ColorChageFrequency = 1;
		tDesc.TargetColor = _float4(0,0, 0, 4.f);
		tDesc.TargetColor2 = _float4(0, 0, 0, 0.f);


		tDesc.Particle_Power = 0;
		tDesc.PowerRandomRange = _float2(0.5f, 1.0f);

		tDesc.vUp = _float3(0, 0, 1);

		tDesc.MaxBoundaryRadius = 3;

		tDesc.m_bIsUI = false;
		tDesc.m_bUIDepth = 0;

		tDesc.ParticleStartRandomPosMin = _float3(0.f);
		tDesc.ParticleStartRandomPosMax = _float3(0.f);

		tDesc.DepthTestON = true;
		tDesc.AlphaBlendON = false;

		tDesc.m_fAlphaTestValue = 0.1f;
		tDesc.m_iPassIndex = 11;



		m_vecParticleDesc.push_back(tDesc);

	}




	return S_OK;
}

CGrenadeBullet * CGrenadeBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGrenadeBullet*	pInstance = new CGrenadeBullet(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(nullptr)))
	{
		MSGBOX("Failed to Created CGrenadeBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGrenadeBullet::Clone(void * pArg)
{
	CGrenadeBullet*	pInstance = new CGrenadeBullet(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CGrenadeBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGrenadeBullet::Free()
{
	__super::Free();

	Safe_Release(m_pTerrain);

}
