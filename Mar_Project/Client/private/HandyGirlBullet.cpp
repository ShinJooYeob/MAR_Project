#include "stdafx.h"
#include "..\public\HandyGirlBullet.h"
#include "Terrain.h"
#include "Player.h"
#include "Monster.h"
#include "CircleTornado.h"



CHandyGirlBullet::CHandyGirlBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonsterWeapon(pDevice,pDeviceContext)
{
}

CHandyGirlBullet::CHandyGirlBullet(const CHandyGirlBullet & rhs)
	: CMonsterWeapon(rhs)
{
}

HRESULT CHandyGirlBullet::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CHandyGirlBullet::Initialize_Clone(void * pArg)
{
	//FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{

		m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Player)));

		NULL_CHECK_RETURN(m_pPlayer, E_FAIL);


		memcpy(&m_tDesc, pArg, sizeof(HANDYGRILBULLETDESC));

	}

	FAILED_CHECK(SetUp_Components());

	
	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_tDesc.vPosition);
	m_fStartTimer = 0;

	CUtilityMgr* pUtilMgr = GetSingle(CUtilityMgr);


	m_tDesc.MoveDir = XMVector3Normalize(m_tDesc.MoveDir.XMVector() * 8.f + pUtilMgr->RandomFloat3(-999.0f, 999.f).Get_Nomalize());

	m_pTransformCom->Scaled_All(_float3(1));

	m_RandScale = pUtilMgr->RandomFloat(3.5f, 8.5f);
	m_fRandHeight = pUtilMgr->RandomFloat(13.5f, 15.f);
	m_TargetTime = pUtilMgr->RandomFloat(0.5f,0.8f);
	m_fStartTimer = 0;
	m_DeathTimer = 0;
	m_bGoingtoDeath = false;

	FAILED_CHECK(SetUp_ParticleDesc());

	GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[0]);

	return S_OK;
}

_int CHandyGirlBullet::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);



	if (m_bGoingtoDeath)
	{
		m_DeathTimer += fDeltaTime;

		if (m_DeathTimer < 0.3f)
		{
			_float3 EasedScale = g_pGameInstance->Easing_Vector(TYPE_ExpoOut, _float3(1,1,1), _float3(m_RandScale, 0.2f, m_RandScale), (_float)m_DeathTimer,0.3f);

			m_pTransformCom->Scaled_All(EasedScale);

		}
		else if (m_DeathTimer < 1.3f)
		{
			_float3 EasedScale = g_pGameInstance->Easing_Vector(TYPE_SinIn, _float3(m_RandScale, 0.2f, m_RandScale), _float3(0.1f, 0.2f, 0.1f), (_float)( m_DeathTimer - 0.3f), 1.f);

			m_pTransformCom->Scaled_All(EasedScale);
		}
		else
		{
			Set_IsDead();
			return 0;
		}


	}
	else
	{
		m_fStartTimer += fDeltaTime;

		m_pTransformCom->MovetoDir(m_tDesc.MoveDir.XMVector(), fDeltaTime);

		if (m_fStartTimer < 0.35f * m_TargetTime)
		{
			_float EasedHeight = g_pGameInstance->Easing(TYPE_SinOut, m_tDesc.vPosition.y, m_fRandHeight, (_float)m_fStartTimer, 0.35f * (_float)m_TargetTime);
			_float3 OldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
			OldPos.y = EasedHeight;
			m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, OldPos);
		}
		else
		{
			_float EasedHeight = g_pGameInstance->Easing(TYPE_SinIn, m_fRandHeight, 8, (_float)( m_fStartTimer - (0.35f * m_TargetTime)), 0.65f * (_float)m_TargetTime);

			if (EasedHeight < 9.7f)
			{
				m_pTransformCom->Set_IsOwnerDead(true);
				EasedHeight = 9.8f;
				m_bGoingtoDeath = true;
				m_DeathTimer = 0;

				CUtilityMgr* pUtil = GetSingle(CUtilityMgr);

				pUtil->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[1]);


				m_vecParticleDesc[1].FixedTarget = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
				m_vecParticleDesc[1].FixedTarget.y = 10.1f;

				pUtil->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[1]);


				pUtil->Start_InstanceParticle(m_eNowSceneNum,m_vecParticleDesc[1].FixedTarget, 0);
				pUtil->Start_InstanceParticle(m_eNowSceneNum,m_vecParticleDesc[1].FixedTarget, 0);



				CCircleTornado::CIRCLETORNADODESC tDesc;
				tDesc.vLook = _float3(0.00000001f, 1.f, 0);
				tDesc.vPosition = m_vecParticleDesc[1].FixedTarget;
				tDesc.vPosition.y -=0.05f ;
				tDesc.fSize = 1.5f;

				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc);
				tDesc.vPosition = tDesc.vPosition;
				tDesc.vPosition.y += 0.1f;
				tDesc.fSize = 1;
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc);



			}

			_float3 OldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
			OldPos.y = EasedHeight;

			m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, OldPos);

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

_int CHandyGirlBullet::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	//if (!m_bIsPlayerCloser) return _int();



	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CHandyGirlBullet::Render()
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

_int CHandyGirlBullet::LightRender()
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


void CHandyGirlBullet::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		if (!lstrcmp(pConflictedObj->Get_NameTag(), L"Alice"))
		{
			pConflictedCollider->Set_Conflicted();
			((CPlayer*)(pConflictedObj))->Add_Dmg_to_Player(rand() % 2 + 1);

		}

	}
	break;
	case Engine::CollisionType_Terrain:
		break;

	default:
		break;
	}
}

HRESULT CHandyGirlBullet::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));


	FAILED_CHECK(Add_Component(m_eNowSceneNum, L"HandyGirlBulletMesh", TAG_COM(Com_Model), (CComponent**)&m_pModel));
	

	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = GetSingle(CUtilityMgr)->RandomFloat(5.f, 16.f);
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
	ColliderDesc.vScale = _float3(1.f, 1, 1);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));



	m_pPlayerTransfrom = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));
	NULL_CHECK_RETURN(m_pPlayerTransfrom, E_FAIL);


	return S_OK;
}

HRESULT CHandyGirlBullet::SetUp_ParticleDesc()
{

	PARTICLEDESC tParticleDesc;

	tParticleDesc = PARTICLEDESC();

	tParticleDesc.eParticleTypeID = Particle_Fixed;

	tParticleDesc.FollowingTarget = m_pTransformCom;

	tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	tParticleDesc.szTextureLayerTag = L"Explosion";
	tParticleDesc.iSimilarLayerNum = 2;

	tParticleDesc.TextureChageFrequency = 1;
	tParticleDesc.vTextureXYNum = _float2(5, 4);

	tParticleDesc.TotalParticleTime = 999.f;
	tParticleDesc.EachParticleLifeTime = 0.34f;

	tParticleDesc.MaxParticleCount = 10;

	tParticleDesc.SizeChageFrequency = 2;
	tParticleDesc.ParticleSize = _float3(3);
	tParticleDesc.ParticleSize2 = _float3(0.2f);

	tParticleDesc.ColorChageFrequency = 0;
	tParticleDesc.TargetColor = _float4(1.f, 1.f, 1.f, 0.7f);
	tParticleDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 1.f);


	tParticleDesc.Particle_Power = 3;
	tParticleDesc.PowerRandomRange = _float2(0.8f, 1.0f);

	tParticleDesc.vUp = _float3(0, 1, 0);

	tParticleDesc.MaxBoundaryRadius = 3;

	tParticleDesc.m_bIsUI = false;
	tParticleDesc.m_bUIDepth = 0;

	tParticleDesc.ParticleStartRandomPosMin = _float3(-0.05f, 0.00f, -0.05f);
	tParticleDesc.ParticleStartRandomPosMax = _float3(0.05f, 0.01f, 0.05f);

	tParticleDesc.DepthTestON = true;
	tParticleDesc.AlphaBlendON = true;

	tParticleDesc.m_fAlphaTestValue = 0.1f;
	tParticleDesc.m_iPassIndex = 3;



	m_vecParticleDesc.push_back(tParticleDesc);

	//////////////////////////////////////////////////////////////////////////


	tParticleDesc = PARTICLEDESC();

	tParticleDesc.eParticleTypeID = Particle_Cone;

	tParticleDesc.FollowingTarget = nullptr;

	tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	tParticleDesc.szTextureLayerTag = L"Explosion2";
	tParticleDesc.iSimilarLayerNum = 1;

	tParticleDesc.TextureChageFrequency = 1;
	tParticleDesc.vTextureXYNum = _float2(4, 4);

	tParticleDesc.TotalParticleTime = 0.1f;
	tParticleDesc.EachParticleLifeTime = 0.34f;
	tParticleDesc.MaxParticleCount = 30;

	tParticleDesc.SizeChageFrequency = 1;
	tParticleDesc.ParticleSize = _float3(0.15f, 0.15f, 0.15f);
	tParticleDesc.ParticleSize2 = _float3(5.f, 5.f, 5.f);

	tParticleDesc.ColorChageFrequency = 1;
	tParticleDesc.TargetColor = _float4(1.f, 0.643f, 0.141f, 1.f);
	tParticleDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 0.f);


	tParticleDesc.Particle_Power = 4;
	tParticleDesc.PowerRandomRange = _float2(0.8f, 1.5f);

	tParticleDesc.vUp = _float3(0, 1, 0);

	tParticleDesc.MaxBoundaryRadius = 6;

	tParticleDesc.m_bIsUI = false;
	tParticleDesc.m_bUIDepth = 0;

	tParticleDesc.ParticleStartRandomPosMin = _float3(0.f, 0, 0.f);
	tParticleDesc.ParticleStartRandomPosMax = _float3(0, 0, 0);

	tParticleDesc.DepthTestON = true;
	tParticleDesc.AlphaBlendON = true;

	tParticleDesc.m_fAlphaTestValue = 0.1f;
	tParticleDesc.m_iPassIndex = 10;



	m_vecParticleDesc.push_back(tParticleDesc);

	return S_OK;
}



CHandyGirlBullet * CHandyGirlBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CHandyGirlBullet*	pInstance = new CHandyGirlBullet(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CHandyGirlBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHandyGirlBullet::Clone(void * pArg)
{
	CHandyGirlBullet*	pInstance = new CHandyGirlBullet(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CHandyGirlBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHandyGirlBullet::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);

	
}
