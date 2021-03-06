#include "stdafx.h"
#include "..\public\NormalBullet.h"
#include "Monster.h"
#include "BreakableObj.h"


CNormalBullet::CNormalBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CBullet(pDevice,pDeviceContext)
{
}

CNormalBullet::CNormalBullet(const CNormalBullet & rhs)
	: CBullet(rhs)
{
}

HRESULT CNormalBullet::Initialize_Prototype(void * pArg)
{
	__super::Initialize_Prototype(pArg);

	return S_OK;
}

HRESULT CNormalBullet::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_TeaBullet), TAG_COM(Com_Model), (CComponent**)&m_pModel));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(0.6f, 0.6f, 0.6f);
	//ColliderDesc.vScale = _float3(0.3f, 0.3f, 0.3f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));


	CUtilityMgr* pUtilMgr = GetSingle(CUtilityMgr);
	m_fTotalLifeTime = pUtilMgr->RandomFloat(1.f, 1.5f);
	m_vTargetDir = m_vTargetDir.XMVector() + pUtilMgr->RandomFloat3(-0.1f, 0.1f).XMVector();


	FAILED_CHECK(Ready_ParticleDesc());

	m_szNameTag = L"Layer_NormalBullet";


	{
		SOUNDDESC tSoundDesc;

		tSoundDesc.pTransform = m_pTransformCom;
		tSoundDesc.vMinMax = _float2(5, 40);
		tSoundDesc.fTargetSound = 1.f;
		wstring SoundTrack = L"";

		SoundTrack = L"Weapon_pepperg_projectile0" + to_wstring(rand() % 4 + 1) + L".ogg";

		g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);


	}

	LIGHTDESC LightDesc;

	LightDesc.eLightType = tagLightDesc::TYPE_POINT;
	LightDesc.vSpecular = LightDesc.vAmbient = LightDesc.vDiffuse = _float4(0.90588235f, 0.745098039f, 0.f, 1.f);
	LightDesc.vVector = XMVectorSetW(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 1);
	LightDesc.fRange = 2.f;
	g_pGameInstance->Add_Light(LightDesc, &m_pLightDesc);

	return S_OK;
}

void CNormalBullet::Set_IsDead()
{
	m_pTransformCom->Set_IsOwnerDead();
	m_bIsDead = true;
	m_pLightDesc->bIsDead = true;
}

_int CNormalBullet::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	if (m_bIsDead) return 0;
	

	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);

	//_float EasedValue = g_pGameInstance->Easing(TYPE_Linear, 20, -100, m_fLifeTime, m_fTotalLifeTime);
	_float EasedValue = g_pGameInstance->Easing(TYPE_QuinticIn, 0, -5, m_fLifeTime, m_fTotalLifeTime);

	_Vector vNewLook = (m_vTargetDir.XMVector() + XMVectorSet(0, 1, 0, 0) * EasedValue);
	m_pTransformCom->LookDir(vNewLook);
	m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1, 0, 0), EasedValue, fDeltaTime);

	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));
	if (m_bIsOnScreen)
	{
		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

		g_pGameInstance->Add_CollisionGroup(CollisionType_PlayerWeapon, this, m_pColliderCom);

#ifdef _DEBUG
		FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
	}

	return _int();
}

_int CNormalBullet::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsDead) return 0;



	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	m_pLightDesc->vVector = XMVectorSetW(m_vOldPos.XMVector(), 1);
	return _int();
}

_int CNormalBullet::Render()
{

	NULL_CHECK_RETURN(m_pModel, E_FAIL);


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

_int CNormalBullet::LightRender()
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

void CNormalBullet::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Monster:
	{




		if (lstrcmp(pConflictedObj->Get_NameTag(), TAG_LAY(Layer_Breakable)))
		{
			pConflictedCollider->Set_Conflicted();
			((CMonster*)(pConflictedObj))->Add_Dmg_to_Monster(1);

		}
		else
		{
			pConflictedCollider->Set_Conflicted();
			//GetSingle(CUtilityMgr)->SlowMotionStart();
			((CBreakableObj*)(pConflictedObj))->Add_Dmg_To_BreakableObj(1);
		}

	}
	break;
	case Engine::CollisionType_Terrain:
		break;

	default:
		break;
	}
}

HRESULT CNormalBullet::Ready_ParticleDesc()
{


	//m_tParticleDesc.eParticleTypeID = Particle_Fixed;

	//m_tParticleDesc.FollowingTarget = m_pTransformCom;

	//m_tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	//m_tParticleDesc.szTextureLayerTag = L"Dust2";
	//m_tParticleDesc.iSimilarLayerNum = 1;

	//m_tParticleDesc.TextureChageFrequency = 8;
	//m_tParticleDesc.vTextureXYNum = _float2(2, 2);
	//
	////m_tParticleDesc.TotalParticleTime = m_fTotalLifeTime;
	//m_tParticleDesc.TotalParticleTime = 9999999999.f;
	//m_tParticleDesc.EachParticleLifeTime = 0.64f;
	//m_tParticleDesc.MaxParticleCount = 3;

	//m_tParticleDesc.SizeChageFrequency = 1;
	//m_tParticleDesc.ParticleSize = _float3(1.f);
	//m_tParticleDesc.ParticleSize2 = _float3(3.f);

	//m_tParticleDesc.ColorChageFrequency = 0;
	//m_tParticleDesc.TargetColor = _float4(1.f, 1.f, 0, 1.f);
	//m_tParticleDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 0.f);

	//m_tParticleDesc.MaxBoundaryRadius = 1.;

	//m_tParticleDesc.m_bIsUI = false;
	//m_tParticleDesc.m_bUIDepth = 0;

	//m_tParticleDesc.ParticleStartRandomPosMin = _float3(-0, 0, -0);

	//m_tParticleDesc.DepthTestON = true;
	//m_tParticleDesc.AlphaBlendON = true;

	//m_tParticleDesc.m_fAlphaTestValue = 0.18f;
	//m_tParticleDesc.m_iPassIndex = 5;

	//GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);




	//m_tParticleDesc.eParticleTypeID = Particle_Straight;
	//m_tParticleDesc.FollowingTarget = m_pTransformCom;
	//m_tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	//m_tParticleDesc.szTextureLayerTag = L"Fire";
	//m_tParticleDesc.iSimilarLayerNum = 1;
	//m_tParticleDesc.szNoiseTextureLayerTag = L"Noise";
	//m_tParticleDesc.iNoiseTextureIndex = 0;
	//m_tParticleDesc.TextureChageFrequency = 1;


	//m_tParticleDesc.vTextureXYNum = _float2(1, 1);
	//m_tParticleDesc.TotalParticleTime = 99999999.f;
	//m_tParticleDesc.EachParticleLifeTime = 0.6f;
	//m_tParticleDesc.MaxParticleCount = 15;

	//m_tParticleDesc.SizeChageFrequency = 0;
	//m_tParticleDesc.ParticleSize = _float3(0.4f, 0.4f, 0.4f);
	//m_tParticleDesc.ParticleSize2 = _float3(0.4f,0.4f,0.4f);

	//m_tParticleDesc.ColorChageFrequency = 0;
	//m_tParticleDesc.TargetColor = _float4(1.f, 1.f, 1.f, 1.f);
	//m_tParticleDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 0.f);
	//m_tParticleDesc.Particle_Power = 2.f;
	//m_tParticleDesc.PowerRandomRange = _float2(1.f, 1.f);
	//m_tParticleDesc.vUp = m_vTargetDir;
	//m_tParticleDesc.MaxBoundaryRadius = 0.5f;
	//m_tParticleDesc.m_bIsUI = false;
	//m_tParticleDesc.m_bIsBlur = true;
	//m_tParticleDesc.m_bUIDepth = 0;
	//m_tParticleDesc.ParticleStartRandomPosMin = _float3(0.f);
	//m_tParticleDesc.ParticleStartRandomPosMax = _float3(0.f);
	//m_tParticleDesc.DepthTestON = true;
	//m_tParticleDesc.AlphaBlendON = true;
	//m_tParticleDesc.m_fAlphaTestValue = 0.1f;
	//m_tParticleDesc.m_iPassIndex = 18;


	//GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);



	m_tParticleDesc = PARTICLEDESC();

	m_tParticleDesc.eParticleTypeID = Particle_Straight;

	m_tParticleDesc.FollowingTarget = m_pTransformCom;

	m_tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	m_tParticleDesc.szTextureLayerTag = L"Explosion";
	m_tParticleDesc.iSimilarLayerNum =1;

	m_tParticleDesc.TextureChageFrequency = 1;
	m_tParticleDesc.vTextureXYNum = _float2(5, 4);

	m_tParticleDesc.TotalParticleTime = 99999.1f;
	m_tParticleDesc.EachParticleLifeTime = 0.34f;

	m_tParticleDesc.MaxParticleCount = 10;

	m_tParticleDesc.SizeChageFrequency = 1;
	m_tParticleDesc.ParticleSize = _float3(0.2f, 0.2f, 0.2f);
	m_tParticleDesc.ParticleSize2 = _float3(0.4f, 0.4f, 0.4f);

	m_tParticleDesc.ColorChageFrequency = 0;
	m_tParticleDesc.TargetColor = _float4(1.f, 1.f, 1.f, 0.7f);
	m_tParticleDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 1.f);


	m_tParticleDesc.Particle_Power = 1;
	m_tParticleDesc.PowerRandomRange = _float2(0.5f, 1.0f);
	
	m_tParticleDesc.vUp = m_vTargetDir;

	m_tParticleDesc.MaxBoundaryRadius = 0.1f;

	m_tParticleDesc.m_bIsUI = false;
	m_tParticleDesc.m_bIsBlur= true;
	m_tParticleDesc.m_bUIDepth = 0;

	m_tParticleDesc.ParticleStartRandomPosMin = _float3(-0.1f);
	m_tParticleDesc.ParticleStartRandomPosMax = _float3(0.1f);

	m_tParticleDesc.DepthTestON = true;
	m_tParticleDesc.AlphaBlendON = true;

	m_tParticleDesc.m_fAlphaTestValue = 0.1f;
	m_tParticleDesc.m_iPassIndex = 19;

	GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);


	return S_OK;
}

CNormalBullet * CNormalBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CNormalBullet*	pInstance = new CNormalBullet(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(nullptr)))
	{
		MSGBOX("Failed to Created CNormalBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CNormalBullet::Clone(void * pArg)
{
	CNormalBullet*	pInstance = new CNormalBullet(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CNormalBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNormalBullet::Free()
{
	__super::Free();



}
