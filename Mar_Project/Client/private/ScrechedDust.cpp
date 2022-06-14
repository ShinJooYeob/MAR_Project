#include "stdafx.h"
#include "..\public\ScrechedDust.h"
#include "Terrain.h"
#include "Player.h"



CScrechedDust::CScrechedDust(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonsterWeapon(pDevice,pDeviceContext)
{
}

CScrechedDust::CScrechedDust(const CScrechedDust & rhs)
	: CMonsterWeapon(rhs)
{
}

HRESULT CScrechedDust::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CScrechedDust::Initialize_Clone(void * pArg)
{
	//FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{

		m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Player)));

		NULL_CHECK_RETURN(m_pPlayer, E_FAIL);


		memcpy(&m_tDesc, pArg, sizeof(BREAKEDGAZBODESC));

	}

	FAILED_CHECK(SetUp_Components());

	CUtilityMgr* pUtilMgr = GetSingle(CUtilityMgr);

	if (pArg != nullptr)
	{
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_tDesc.vPosition);
		m_pTransformCom->Scaled_All(_float3(pUtilMgr->RandomFloat(0.5f, 0.8f)));
	}



	m_vRotAxis = (pUtilMgr->RandomFloat3(-10000.f, 10000.f)).Get_Nomalize();;
	m_fRandPower = (pUtilMgr->RandomFloat(25, 30.f));

	m_fMaxTimer = pUtilMgr->RandomFloat(1, 2.f);

	m_pTransformCom->Rotation_CW(m_vRotAxis.XMVector(),XMConvertToRadians(pUtilMgr->RandomFloat(0, 360)));


	m_fStartTimer = 0;
	FAILED_CHECK(Ready_ParticleDesc());
	GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);

	return S_OK;
}

_int CScrechedDust::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);


	if (m_bIsSpout)
	{
		m_fStartTimer += fDeltaTime;
		
		m_pTransformCom->Turn_CW(m_vRotAxis.XMVector(), fDeltaTime);

		m_pTransformCom->MovetoDir_bySpeed(m_tDesc.MoveDir.XMVector(), m_fRandPower,fDeltaTime);


		if (m_fStartTimer > m_fMaxTimer)
		{
			m_pTransformCom->Set_IsOwnerDead(true);
			Set_IsDead();
			return 0;
		}
	}

	if ((!m_pSounDesc || m_pSounDesc->iIdentificationNumber != 60))
	{
		{
			SOUNDDESC tSoundDesc;
			tSoundDesc.pTransform = m_pTransformCom;
			tSoundDesc.vMinMax = _float2(0, 35);
			tSoundDesc.fTargetSound = 0.25f;
			tSoundDesc.iIdentificationNumber = 60;

			wstring SoundTrack = L"";
			SoundTrack = L"HandyGirl_vox_death_a0" + to_wstring(rand() % 2 + 1) + L".ogg";
			//SoundTrack = L"DollMaker_hands_boy_fistwhoosh.ogg";

			g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc,&m_pSounDesc);
		}

	}
	

	//m_pTransformCom->MovetoDir_bySpeed(m_tDesc.MoveDir.XMVector(), 15 , fDeltaTime);


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

_int CScrechedDust::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	//if (!m_bIsPlayerCloser) return _int();




	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CScrechedDust::Render()
{
	if (__super::Render() < 0)
		return -1;



	return _int();
}

_int CScrechedDust::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;


	return _int();
}

void CScrechedDust::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
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

HRESULT CScrechedDust::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));




	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = GetSingle(CUtilityMgr)->RandomFloat(0.06f, 0.08f);
	tDesc.fRotationPerSec = XMConvertToRadians(GetSingle(CUtilityMgr)->RandomFloat(720,1080));
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

HRESULT CScrechedDust::Ready_ParticleDesc()
{
	m_tParticleDesc = PARTICLEDESC();

	m_tParticleDesc.eParticleTypeID = Particle_Fixed;

	m_tParticleDesc.FollowingTarget = m_pTransformCom;

	m_tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	m_tParticleDesc.szTextureLayerTag = L"Dust3";
	m_tParticleDesc.iSimilarLayerNum = 1;

	m_tParticleDesc.TextureChageFrequency = 1;
	m_tParticleDesc.vTextureXYNum = _float2(4, 4);

	m_tParticleDesc.TotalParticleTime = 9999.f;
	m_tParticleDesc.EachParticleLifeTime = 0.34f;
	m_tParticleDesc.MaxParticleCount = 1;

	m_tParticleDesc.SizeChageFrequency = 0;
	m_tParticleDesc.ParticleSize = _float3(5.f);
	m_tParticleDesc.ParticleSize2 = _float3(2.5);

	m_tParticleDesc.ColorChageFrequency = 0;
	m_tParticleDesc.TargetColor = _float4(1.f, 0.6f, 1.f, 1.f);
	m_tParticleDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 1.f);


	m_tParticleDesc.Particle_Power = 2;
	m_tParticleDesc.PowerRandomRange = _float2(0.8f, 1.5f);

	m_tParticleDesc.vUp = _float3(0, 1, 0);

	m_tParticleDesc.MaxBoundaryRadius = 6;

	m_tParticleDesc.m_bIsUI = false;
	m_tParticleDesc.m_bIsBlur = true;
	m_tParticleDesc.m_bUIDepth = 0;

	m_tParticleDesc.ParticleStartRandomPosMin = _float3(0);
	m_tParticleDesc.ParticleStartRandomPosMax = _float3(0);

	m_tParticleDesc.DepthTestON = true;
	m_tParticleDesc.AlphaBlendON = true;

	m_tParticleDesc.m_fAlphaTestValue = 0.1f;
	m_tParticleDesc.m_iPassIndex = 19;


	return S_OK;
}



CScrechedDust * CScrechedDust::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CScrechedDust*	pInstance = new CScrechedDust(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CScrechedDust");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CScrechedDust::Clone(void * pArg)
{
	CScrechedDust*	pInstance = new CScrechedDust(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CScrechedDust");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CScrechedDust::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);

	
}
