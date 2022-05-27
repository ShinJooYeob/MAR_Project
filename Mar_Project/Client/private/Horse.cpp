#include "stdafx.h"
#include "..\public\Horse.h"
#include "Monster.h"
#include "BreakableObj.h"




CHorse::CHorse(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CWeapon(pDevice,pDeviceContext)
{
}

CHorse::CHorse(const CHorse & rhs)
	: CWeapon(rhs)
{
}

HRESULT CHorse::Initialize_Prototype(void * pArg)
{
	__super::Initialize_Prototype(pArg);

	return S_OK;
}

HRESULT CHorse::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	FAILED_CHECK(SetUp_Components());

	m_tParticleDesc.eParticleTypeID = Particle_Fixed_LookFree;

	m_tParticleDesc.FollowingTarget = nullptr;

	m_tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	m_tParticleDesc.szTextureLayerTag = L"HorseHit";
	m_tParticleDesc.iSimilarLayerNum = 2;

	m_tParticleDesc.TextureChageFrequency = 1;
	m_tParticleDesc.vTextureXYNum = _float2(3, 3);

	m_tParticleDesc.TotalParticleTime = 0.01f;
	m_tParticleDesc.EachParticleLifeTime = 0.34f;
	m_tParticleDesc.MaxParticleCount = 1;

	m_tParticleDesc.SizeChageFrequency = 2;
	m_tParticleDesc.ParticleSize = _float3(2.f);
	m_tParticleDesc.ParticleSize2 = _float3(5.5f);

	m_tParticleDesc.ColorChageFrequency = 1;
	m_tParticleDesc.TargetColor = _float4(0.8f, 0.8f, 0.5f, 0.3f);
	m_tParticleDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 0.8f);


	m_tParticleDesc.Particle_Power = 0;
	m_tParticleDesc.PowerRandomRange = _float2(0.5f, 1.5f);

	m_tParticleDesc.vUp = _float3(0, 1, 0);

	m_tParticleDesc.MaxBoundaryRadius = 20;

	m_tParticleDesc.m_bIsUI = false;
	m_tParticleDesc.m_bUIDepth = 0;

	m_tParticleDesc.ParticleStartRandomPosMin = _float3(-0, 0, -0);
	m_tParticleDesc.ParticleStartRandomPosMax = _float3(0, 0, 0);

	m_tParticleDesc.DepthTestON = true;
	m_tParticleDesc.AlphaBlendON = false;

	m_tParticleDesc.m_fAlphaTestValue = 0.1f;
	m_tParticleDesc.m_iPassIndex = 9;
	

	//m_pSwordTrailCom->Set_Color(_float4(0, 0.84705882f, 1, 1.f));

	//m_pSwordTrailCom->Set_PassIndex(2);
	//m_pSwordTrailCom->Set_TextureIndex(2);

	//m_pSubSwordTrailCom->Set_PassIndex(2);
	//m_pSubSwordTrailCom->Set_TextureIndex(2);

	return S_OK;
}

_int CHorse::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	if (m_bIsDead) return 0;
	
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);

	_Vector ColDir = XMVector3Normalize(m_pColliderCom->Get_ColliderPosition(3).XMVector() - m_pColliderCom->Get_ColliderPosition(1).XMVector());
	_Vector Right = XMVector3Normalize(XMVector3Cross(XMVectorSet(0, 1, 0, 0), ColDir)) * 0.5f;

	m_pSwordTrailCom->Update_SwordTrail(m_pColliderCom->Get_ColliderPosition(1).XMVector() - ColDir,
		m_pColliderCom->Get_ColliderPosition(3).XMVector() + ColDir, fDeltaTime);
	m_pSubSwordTrailCom->Update_SwordTrail(m_pColliderCom->Get_ColliderPosition(3).XMVector() - Right,
		m_pColliderCom->Get_ColliderPosition(3).XMVector() + Right, fDeltaTime);


	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime * 2));


	_Matrix			TransformMatrix = XMLoadFloat4x4(m_tATBMat.pUpdatedNodeMat) * XMLoadFloat4x4(m_tATBMat.pDefaultPivotMat);

	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_BoneMatrix = TransformMatrix = m_pTransformCom->Get_WorldMatrix()* TransformMatrix * m_tWeaponDesc.pParantTransform->Get_WorldMatrix();


	for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
		m_pColliderCom->Update_Transform(i, TransformMatrix);


	if (m_bIsAttackAble)
	{
		g_pGameInstance->Add_CollisionGroup(CollisionType_PlayerWeapon, this, m_pColliderCom);

#ifdef _DEBUG
		FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
	}

	return _int();
}

_int CHorse::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsDead) return 0;

	
	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	FAILED_CHECK(m_pRendererCom->Add_TrailGroup(m_pSwordTrailCom));
	FAILED_CHECK(m_pRendererCom->Add_TrailGroup(m_pSubSwordTrailCom));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CHorse::Render()
{

	NULL_CHECK_RETURN(m_pModel, E_FAIL);




	_float4x4 ShaderMat = m_BoneMatrix.TransposeXMatrix();
	m_pShaderCom->Set_RawValue("g_AttechMatrix", &ShaderMat, sizeof(_float4x4));



	CGameInstance* pInstance = GetSingle(CGameInstance);

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));


	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 8, i, "g_BoneMatrices"));
	}


	return _int();
}

_int CHorse::LightRender()
{

	return _int();
}

void CHorse::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{

	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Monster:
	{


		if (lstrcmp(pConflictedObj->Get_NameTag(), TAG_LAY(Layer_Breakable)))
		{
			CUtilityMgr* pUtil = GetSingle(CUtilityMgr);

			pConflictedCollider->Set_Conflicted();
			pUtil->SlowMotionStart();
			((CMonster*)(pConflictedObj))->Add_Dmg_to_Monster(5);

			_Vector vMyColliderPos = m_pColliderCom->Get_ColliderPosition(iMyColliderIndex).XMVector();
			_Vector vEnemyColliderPos = pConflictedCollider->Get_ColliderPosition(iConflictedObjColliderIndex).XMVector();

			pUtil->Start_InstanceParticle(m_eNowSceneNum, vMyColliderPos, 0);

			m_tParticleDesc.FixedTarget = (vMyColliderPos + vEnemyColliderPos)*0.5f;
			m_tParticleDesc.vUp = XMVector3Normalize(XMVector3Cross(vMyColliderPos - vEnemyColliderPos, g_pGameInstance->Get_TargetPostion_Vector(PLV_CAMERA) - vEnemyColliderPos));
			pUtil->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);
		}
		else
		{
			CUtilityMgr* pUtil = GetSingle(CUtilityMgr);

			pConflictedCollider->Set_Conflicted();
			pUtil->SlowMotionStart();
			((CBreakableObj*)(pConflictedObj))->Add_Dmg_To_BreakableObj(5);

			pUtil->Start_InstanceParticle(m_eNowSceneNum, m_pColliderCom->Get_ColliderPosition(iMyColliderIndex), 0);


			_Vector vMyColliderPos = m_pColliderCom->Get_ColliderPosition(iMyColliderIndex).XMVector();
			_Vector vEnemyColliderPos = pConflictedCollider->Get_ColliderPosition(iConflictedObjColliderIndex).XMVector();

			pUtil->Start_InstanceParticle(m_eNowSceneNum, vMyColliderPos, 0);

			m_tParticleDesc.FixedTarget = (vMyColliderPos + vEnemyColliderPos)*0.5f;
			m_tParticleDesc.vUp = XMVector3Normalize(XMVector3Cross(vMyColliderPos - vEnemyColliderPos, g_pGameInstance->Get_TargetPostion_Vector(PLV_CAMERA) - vEnemyColliderPos));
			pUtil->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);

		}

	}
	break;
	case Engine::CollisionType_Terrain:
		break;

	default:
		break;
	}

}

void CHorse::Set_TrailOn(_bool bBool)
{
	m_pSwordTrailCom->Set_TrailTurnOn(bBool, m_pColliderCom->Get_ColliderPosition(1), m_pColliderCom->Get_ColliderPosition(3));
	m_pSubSwordTrailCom->Set_TrailTurnOn(bBool, m_pColliderCom->Get_ColliderPosition(1), m_pColliderCom->Get_ColliderPosition(3));
}

HRESULT CHorse::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Mesh_Horse), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	m_pModel->Change_AnimIndex(1);
	m_pModel->Change_AnimIndex(0);


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.4f, 1.4f, 1.4f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);	
	ColliderDesc.vPosition = _float4(-0.85f, 0.08f, -1.305f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));

	ColliderDesc.vScale = _float3(0.3f, 0.3f, 0.3f);
	ColliderDesc.vPosition = _float4(-0.61f, -0.01f, -1.32f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();

	ColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vPosition = _float4(-0.85f, 0.08f, -1.305f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();


	ColliderDesc.vScale = _float3(0.8f, 0.8f, 0.8f);
	ColliderDesc.vPosition = _float4(0,0,0, 1.f);
	//-0.180000, 0.210000, 0.150000,
	ColliderDesc.vPosition = _float4(-1.03f, 0.29f, -1.155f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();

	//Pivot: -0.180000, 0.120000, 0.030000,
	//Pivot: -0.480000, 0.180000, 0.030000,


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));


	CSwordTrail::TRAILDESC tTrailDsec;
	tTrailDsec.iPassIndex = 2;
	tTrailDsec.iTextureIndex = 2;
	tTrailDsec.NoiseSpeed = 10;
	tTrailDsec.vColor =  _float4(0, 0.84705882f, 1, 1.f);

	FAILED_CHECK(Add_Component(SCENE_STATIC,TAG_CP(Prototype_Trail), TAG_COM(Com_SwordTrail), (CComponent**)&m_pSwordTrailCom,&tTrailDsec));
	FAILED_CHECK(Add_Component(SCENE_STATIC,TAG_CP(Prototype_Trail), TAG_COM(Com_SubSwordTrail), (CComponent**)&m_pSubSwordTrailCom, &tTrailDsec));

	m_pPlayerTransform = (CTransform*)m_pPlayer->Get_Component(TAG_COM(Com_Transform));
	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);

	return S_OK;
}

CHorse * CHorse::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CHorse*	pInstance = new CHorse(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(nullptr)))
	{
		MSGBOX("Failed to Created CHorse");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHorse::Clone(void * pArg)
{
	CHorse*	pInstance = new CHorse(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CHorse");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHorse::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);

	Safe_Release(m_pSwordTrailCom);
	Safe_Release(m_pSubSwordTrailCom);
	
}
