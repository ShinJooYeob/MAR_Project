#include "stdafx.h"
#include "..\public\Knife.h"
#include "Monster.h"
#include "BreakableObj.h"




CKnife::CKnife(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CWeapon(pDevice,pDeviceContext)
{
}

CKnife::CKnife(const CKnife & rhs)
	: CWeapon(rhs)
{
}

HRESULT CKnife::Initialize_Prototype(void * pArg)
{
	__super::Initialize_Prototype(pArg);

	return S_OK;
}

HRESULT CKnife::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	FAILED_CHECK(SetUp_Components());



	m_tParticleDesc.eParticleTypeID = Particle_Fixed_LookFree;

	m_tParticleDesc.FollowingTarget = nullptr;

	m_tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	m_tParticleDesc.szTextureLayerTag = L"SwordHit";
	m_tParticleDesc.iSimilarLayerNum = 2;

	m_tParticleDesc.TextureChageFrequency = 1;
	m_tParticleDesc.vTextureXYNum = _float2(1, 1);

	m_tParticleDesc.TotalParticleTime = 0.01f;
	m_tParticleDesc.EachParticleLifeTime = 0.2f;
	m_tParticleDesc.MaxParticleCount = 1;

	m_tParticleDesc.SizeChageFrequency = 1;
	m_tParticleDesc.ParticleSize = _float3(2.f, 4.f, 2.f);
	m_tParticleDesc.ParticleSize2 = _float3(1.f, 12.f, 1.f);

	m_tParticleDesc.ColorChageFrequency = 1;
	m_tParticleDesc.TargetColor = _float4(1.f, 1.f, 1.f, 1.f);
	m_tParticleDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 0.5f);


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


	//m_pSwordTrailCom->Set_TrailTurnOn(true, m_pColliderCom->Get_ColliderPosition(1) , m_pColliderCom->Get_ColliderPosition(3));
	//m_pSwordTrailCom->Set_Color(_float4(0.4392156862f, 0.75686274f, 1, 1.f));
	m_pSwordTrailCom->Set_Color(_float4(0, 0.84705882f, 1, 1.f));

	GetSingle(CUtilityMgr)->RandomFloat3(0, 1);
	m_pSwordTrailCom->Set_PassIndex(0);
	m_pSwordTrailCom->Set_TextureIndex(1);
	return S_OK;
}

_int CKnife::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	if (m_bIsDead) return 0;
	
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);

	m_pSwordTrailCom->Update_SwordTrail(m_pColliderCom->Get_ColliderPosition(1), m_pColliderCom->Get_ColliderPosition(3),fDeltaTime);

	{

		m_TrailColorChageTime += fDeltaTime;


		if (m_TrailColorChageTime > 1)
		{
			m_TrailColorChageTime = 0;
			m_SourColor = m_TargetColor;
			m_TargetColor = GetSingle(CUtilityMgr)->RandomFloat3(0, 1);
		}
		m_pSwordTrailCom->Set_Color(_float4(g_pGameInstance->Easing_Vector(TYPE_Linear, m_SourColor, m_TargetColor,(_float) m_TrailColorChageTime, 1), 1));


	}


	
	_Matrix			TransformMatrix = XMLoadFloat4x4(m_tATBMat.pUpdatedNodeMat) * XMLoadFloat4x4(m_tATBMat.pDefaultPivotMat);

	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);


	m_BoneMatrix = TransformMatrix = m_pTransformCom->Get_WorldMatrix()* TransformMatrix * m_tWeaponDesc.pParantTransform->Get_WorldMatrix();

	//TransformMatrix = TransformMatrix * m_tWeaponDesc.pParantTransform->Get_WorldMatrix();

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

_int CKnife::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsDead) return 0;


	//if (m_bIsOnScreen)	
	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	FAILED_CHECK(m_pRendererCom->Add_TrailGroup(m_pSwordTrailCom));
	
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CKnife::Render()
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

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 7, i));
	}


	return _int();
}

_int CKnife::LateRender()
{

	return _int();
}

void CKnife::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
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
			((CMonster*)(pConflictedObj))->Add_Dmg_to_Monster(3);

			pUtil->Start_InstanceParticle(m_eNowSceneNum, m_pColliderCom->Get_ColliderPosition(iMyColliderIndex), 0);

			_Vector vMyColliderPos = m_pColliderCom->Get_ColliderPosition(iMyColliderIndex).XMVector();
			_Vector vEnemyColliderPos = pConflictedCollider->Get_ColliderPosition(iConflictedObjColliderIndex).XMVector();

			pUtil->Start_InstanceParticle(m_eNowSceneNum, vMyColliderPos,1);

			m_tParticleDesc.FixedTarget = (vMyColliderPos + vEnemyColliderPos)*0.5f;
			m_tParticleDesc.vUp = XMVector3Normalize(XMVector3Cross(vMyColliderPos - vEnemyColliderPos, g_pGameInstance->Get_TargetPostion_Vector(PLV_CAMERA) - vEnemyColliderPos));
			pUtil->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);


		}
		else
		{
			CUtilityMgr* pUtil = GetSingle(CUtilityMgr);

			pConflictedCollider->Set_Conflicted();
			pUtil->SlowMotionStart();
			((CBreakableObj*)(pConflictedObj))->Add_Dmg_To_BreakableObj(1);

			pUtil->Start_InstanceParticle(m_eNowSceneNum, m_pColliderCom->Get_ColliderPosition(iMyColliderIndex), 0);


			_Vector vMyColliderPos = m_pColliderCom->Get_ColliderPosition(iMyColliderIndex).XMVector();
			_Vector vEnemyColliderPos = pConflictedCollider->Get_ColliderPosition(iConflictedObjColliderIndex).XMVector();

			pUtil->Start_InstanceParticle(m_eNowSceneNum, vMyColliderPos, 1);


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

void CKnife::Set_TrailOn(_bool bBool)
{
	m_pSwordTrailCom->Set_TrailTurnOn(bBool, m_pColliderCom->Get_ColliderPosition(1), m_pColliderCom->Get_ColliderPosition(3));
}

HRESULT CKnife::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Mesh_KnifeUg), TAG_COM(Com_Model), (CComponent**)&m_pModel));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);	
	//ColliderDesc.vPosition = _float4(0, 0, 0, 1.f);
	ColliderDesc.vPosition = _float4(-0.85f, 0.08f, -1.305f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));

	ColliderDesc.vScale = _float3(0.4f, 0.4f, 0.4f);
	ColliderDesc.vPosition = _float4(-0.61f, -0.01f, -1.32f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();

	//ColliderDesc.vPosition = _float4(-0.85f, 0.08f, -1.305f, 1.f);
	ColliderDesc.vPosition = _float4(-0.85f, 0.08f, -1.305f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();


	ColliderDesc.vPosition = _float4(-1.09f, 0.17f, -1.29f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();

	//Pivot: -0.180000, 0.120000, 0.030000,
	//Pivot: -0.480000, 0.180000, 0.030000,


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));
	FAILED_CHECK(Add_Component(SCENE_STATIC, L"Trail", TAG_COM(Com_SwordTrail), (CComponent**)&m_pSwordTrailCom));

	
	return S_OK;
}

CKnife * CKnife::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CKnife*	pInstance = new CKnife(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(nullptr)))
	{
		MSGBOX("Failed to Created CKnife");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CKnife::Clone(void * pArg)
{
	CKnife*	pInstance = new CKnife(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CKnife");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CKnife::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pSwordTrailCom);
	
}
