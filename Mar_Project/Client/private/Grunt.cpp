#include "stdafx.h"
#include "..\public\Grunt.h"
#include "ChainExplosion.h"



CGrunt::CGrunt(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonster(pDevice, pDeviceContext)
{
}

CGrunt::CGrunt(const CGrunt & rhs)
	: CMonster(rhs)
{
}

HRESULT CGrunt::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CGrunt::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
	{
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, *((_float3*)pArg));
	
	}
	ZeroMemory(m_bIsDmgAnimUpdated, sizeof(_bool) * 3);
	m_fHP = m_fMaxHP = 28;
	m_SpwanedPassedTime = 0;
	m_pModel->Change_AnimIndex_ReturnTo_Must(26, 0, 0.15, true);


	_float3 RandDir = GetSingle(CUtilityMgr)->RandomFloat3(-9999.f, 9999.f);
	RandDir.y = 0;
	m_pTransformCom->LookDir(RandDir.Get_Nomalize());

	m_ePattern = _uint(GetSingle(CUtilityMgr)->RandomFloat(0, 2));

	__super::SetUp_WanderLook(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK));

	m_iSpwanMeshRend = 0;
	m_PatternDelayTime = 0;

	FAILED_CHECK(SetUp_ParticleDesc());

	return S_OK;
}

_int CGrunt::Update(_double fDeltaTime)
{

	if (__super::Update(fDeltaTime) < 0)return -1;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);


	{
		_Matrix			TransformMatrix = XMLoadFloat4x4(m_tCollisionAttachPtr.pUpdatedNodeMat)	* XMLoadFloat4x4(m_tCollisionAttachPtr.pDefaultPivotMat);

		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

		TransformMatrix = XMMatrixTranslation(-1.394999f, -0.140000f, -0.515000f) * TransformMatrix * m_pTransformCom->Get_WorldMatrix();

		m_pParticleTargetTransformCom->Set_Matrix(TransformMatrix);
	}



	if (g_pGameInstance->Get_DIKeyState(DIK_F1)&DIS_Down)
		m_pModel->Change_AnimIndex(0);
	if (g_pGameInstance->Get_DIKeyState(DIK_F2)&DIS_Down)
		m_pModel->Change_AnimIndex(8);

	if (g_pGameInstance->Get_DIKeyState(DIK_O)&DIS_Down)
		Add_Dmg_to_Monster(1000);
	
	m_bSpwanedAnimFinished = true;
	m_iSpwanMeshRend = 2;
	/*
	static float testFloat = 1.;
	if (g_pGameInstance->Get_DIKeyState(DIK_1)&DIS_Down)
	m_pModel->Change_AnimIndex(2);
	if (g_pGameInstance->Get_DIKeyState(DIK_2)&DIS_Down)
	m_pModel->Change_AnimIndex(6);
	if (g_pGameInstance->Get_DIKeyState(DIK_3)&DIS_Down)
	m_pModel->Change_AnimIndex(17);


	if (m_pInstance->Get_DIKeyState(DIK_UP) & DIS_Down)
	{
	testFloat += 0.1f;
	m_pTransformCom->Set_MoveSpeed(testFloat);


	string ttszLog = "Monster Speed: " + to_string(testFloat) + "\n";
	wstring ttDebugLog;
	ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

	OutputDebugStringW(ttDebugLog.c_str());
	}
	else if (m_pInstance->Get_DIKeyState(DIK_DOWN) & DIS_Down)
	{
	testFloat -= 0.1f;
	m_pTransformCom->Set_MoveSpeed(testFloat);

	string ttszLog = "Monster Speed: " + to_string(testFloat) + "\n";
	wstring ttDebugLog;
	ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

	OutputDebugStringW(ttDebugLog.c_str());

	if (g_pGameInstance->Get_DIKeyState(DIK_C)&DIS_Down)
	Add_Dmg_to_Monster(1);

	if (g_pGameInstance->Get_DIKeyState(DIK_V)&DIS_Down)
	{
	Add_Force(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_UP), 30);
	}

	}
	*/


	if (m_bDeathAnimStart)
	{
		m_SpwanedPassedTime += fDeltaTime;
		_float PlayRate = (_float)m_pSubModel->Get_PlayRate();

		FAILED_CHECK(m_pSubModel->Update_AnimationClip(fDeltaTime, m_bIsOnScreen));


		if (PlayRate > 0.4105263 && m_iSpwanMeshRend == 1)
		{
			m_iSpwanMeshRend = 0;
		}
		else if (PlayRate > 0.95)
		{
			Set_IsDead();
			return 0;
		}
		


	}
	else if (m_bSpwanedAnimFinished)
	{
		_uint AnimIndex = m_pModel->Get_NowAnimIndex();

		if (AnimIndex == 1)
			m_pModel->Set_NextAnim_Must(2);

		if (!(AnimIndex >= 23 && AnimIndex <= 25 || AnimIndex == 1))
		{

			if ((!m_bIsPatternFinished || Distance_BetweenPlayer(m_pTransformCom) < 8) && m_PatternDelayTime > 4)
			{
				Update_Pattern(fDeltaTime);
			}
			else
			{
				m_PatternDelayTime += fDeltaTime;
				if (!m_pModel->Get_IsUntillPlay())
				{
					m_pModel->Change_AnimIndex(2);
					FAILED_CHECK(__super::Update_WanderAround(m_pTransformCom, fDeltaTime, 0.05f));
				}
			}
		}

	}
	else
	{
		m_SpwanedPassedTime += fDeltaTime;
		_float PlayRate = (_float)m_pSubModel->Get_PlayRate();

		if (m_iSpwanMeshRend < 2)
		{
			FAILED_CHECK(m_pSubModel->Update_AnimationClip(fDeltaTime, m_bIsOnScreen));

			m_SpwanMeshScale = 2;

			if (PlayRate > 0.64285714 && !m_iSpwanMeshRend)
			{
				m_iSpwanMeshRend = 1;
			}
			else if (m_iSpwanMeshRend == 1 && PlayRate > 0.95)
			{
				m_iSpwanMeshRend = 2; 
				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.2f, _float4(0.2f));
			}
			else if (PlayRate > 0.82)
			{
				m_SpwanMeshScale = _float(2 * (5.1f - PlayRate * 5));
			}


		}


	}




	Update_DmgCalculate(fDeltaTime);

	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));


	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime, m_bIsOnScreen));
	FAILED_CHECK(Adjust_AnimMovedTransform(fDeltaTime));
	
	if (m_bIsOnScreen)
	{
		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer() - 1; i++)
			m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

		{

			m_pColliderCom->Update_Transform(m_pColliderCom->Get_NumColliderBuffer() - 1, m_pParticleTargetTransformCom->Get_WorldMatrix());

		}

		if (!m_bDeathAnimStart)
			g_pGameInstance->Add_CollisionGroup(CollisionType_Monster, this, m_pColliderCom);

		//if(!m_bIsPatternFinished)
			g_pGameInstance->Add_CollisionGroup(CollisionType_MonsterWeapon, this, m_pColliderCom);

	}

	return _int();
}

_int CGrunt::LateUpdate(_double fDeltaTime)
{
	if (m_bSpwanedAnimFinished)
		FAILED_CHECK(__super::Set_Monster_On_Terrain(m_pTransformCom, fDeltaTime));

	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));


	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CGrunt::Render()
{
	if (__super::Render() < 0)
		return -1;


	NULL_CHECK_RETURN(m_pModel, E_FAIL);

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG



	CGameInstance* pInstance = GetSingle(CGameInstance);

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));



	if (m_iSpwanMeshRend < 2 && m_SpwanedPassedTime >0.1)
	{
		m_pTransformCom->Scaled_All({ m_SpwanMeshScale,m_SpwanMeshScale,m_SpwanMeshScale });
		FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
		_uint NumMaterial = m_pSubModel->Get_NumMaterial();

		for (_uint i = 0; i < NumMaterial; i++)
		{
			for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
				FAILED_CHECK(m_pSubModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

			FAILED_CHECK(m_pSubModel->Render(m_pShaderCom, 0, i, "g_BoneMatrices"));
		}

		m_pTransformCom->Scaled_All({ 1, 1, 1 });
	}
	if (m_iSpwanMeshRend > 0)
	{
		FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
		_uint NumMaterial = m_pModel->Get_NumMaterial();

		for (_uint i = 0; i < NumMaterial; i++)
		{
			for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
				FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

			FAILED_CHECK(m_pModel->Render(m_pShaderCom, 0, i, "g_BoneMatrices"));
		}
	}

	return _int();
}

_int CGrunt::LateRender()
{
	return _int();
}

void CGrunt::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		if (iMyColliderIndex == 2)
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

_int CGrunt::Update_DmgCalculate(_double fDeltaTime)
{
	if (m_DmgPassedTime <= 0)
	{
		if (m_fDmgAmount > 0)
		{
			ZeroMemory(m_bIsDmgAnimUpdated, sizeof(_bool) * 3);
			m_fDmgAmount = 0;
			m_DmgPassedTime = 0;
		}
		return 0;
	}
	m_DmgPassedTime -= fDeltaTime;

	if (m_fHP <=0)
	{
		if (!m_bDeathAnimStart)
		{
			m_iSpwanMeshRend = 1;
			m_SpwanMeshScale = 2;
			m_SpwanedPassedTime = 0;
			m_pSubModel->Change_AnimIndex(1, 0.15f, true);
			m_pModel->Change_AnimIndex_ReturnTo_Must(27, 27, 0, true);
			m_bDeathAnimStart = true;
		}
		return 0;
	}

	if (!m_bIsDmgAnimUpdated[0] && m_fMaxHP * 0.07 < m_fDmgAmount)
	{
		m_pModel->Change_AnimIndex_ReturnTo_Must(23, 1, 0.15, true);
		m_bIsPatternFinished = true;
		m_PatternPassedTime = 0;
		m_pParticleTargetTransformCom->Set_IsOwnerDead(true);
		Add_Force(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)* -1, 10);
		m_pTransformCom->Set_MoveSpeed(0.5);
		m_bIsDmgAnimUpdated[0] = true;
	}
	else if (!m_bIsDmgAnimUpdated[1] && m_fMaxHP * 0.15 < m_fDmgAmount)
	{
		m_pModel->Change_AnimIndex_ReturnTo_Must(24, 1, 0.15, true);
		m_bIsPatternFinished = true;
		m_pParticleTargetTransformCom->Set_IsOwnerDead(true);
		m_PatternPassedTime = 0;
		m_pTransformCom->Set_MoveSpeed(0.5);
		Add_Force(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)* -1, 20);
		m_bIsDmgAnimUpdated[1] = true;
	}



	return 0;
}

_int CGrunt::Update_Pattern(_double fDeltaTime)
{

	m_PatternPassedTime += fDeltaTime;
	if (m_bIsPatternFinished)
	{
		m_ePattern += 1;

		if (m_ePattern > 4) m_ePattern = 0;
		m_bIsPatternFinished = false;
		m_PatternPassedTime = 0;
		//m_pModel->Change_AnimIndex(2);
	}

	switch (m_ePattern)
	{
	case 0:
		if (!m_PatternPassedTime)
		{
			m_pModel->Change_AnimIndex_UntilNReturn(3, 4,0, 0.15, true);
		}
		else
		{
			if (!m_pModel->Get_IsUntillPlay())
			{
				m_bIsPatternFinished = true;
				m_PatternDelayTime = 0;
				m_PatternPassedTime = 0;
			}
		}


		break;
	case 1:
		if (!m_PatternPassedTime)
		{
			m_pModel->Change_AnimIndex_UntilTo(5, 6, 0.15, true);
			m_pTransformCom->Set_MoveSpeed(6.5);
		}
		else
		{

			if (m_PatternPassedTime < 5)
			{
				if (m_pModel->Get_NowAnimIndex() == 6)
				{
					if (Distance_BetweenPlayer(m_pTransformCom) < 5)
					{
						m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
						m_pTransformCom->LookDir(m_vLookDir.XMVector() * 0.35f + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * 0.65f);
						m_PatternPassedTime = 7;
					}
					else
					{
						FAILED_CHECK(DashPatternWander(fDeltaTime));
					}
				}
			}
			else if(m_pModel->Get_NowAnimIndex() == 6)
			{
				m_pModel->Change_AnimIndex_UntilNReturn(7, 8, 2, 0.15, true);
				Add_Force(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 10);
			
				m_pTransformCom->Set_MoveSpeed(0.5);
			}
			else if (m_pModel->Get_NowAnimIndex() == 2 && m_pModel->Get_PlayRate() > 0.01)
			{
				m_bIsPatternFinished = true;
				m_PatternPassedTime = 0; 
				m_PatternDelayTime = 0;

			}
		}


		break;
	case 2:
		if (!m_PatternPassedTime)
		{
			m_pModel->Change_AnimIndex_UntilNReturn(9, 11, 0,0.15, true);
		}
		else
		{
			if (!m_pModel->Get_IsUntillPlay())
			{
				m_bIsPatternFinished = true;
				m_PatternPassedTime = 0; m_PatternDelayTime = 0;
			}
		}

		break;
	case 3:
		if (!m_PatternPassedTime)
		{
			m_pModel->Change_AnimIndex_UntilNReturn(12, 13, 0,0.15, true);
		}
		else
		{
			if (!m_pModel->Get_IsUntillPlay())
			{
				m_bIsPatternFinished = true;
				m_PatternPassedTime = 0; m_PatternDelayTime = 0;
			}
		}

		break;

	case 4:
		if (!m_PatternPassedTime)
		{
			m_pModel->Change_AnimIndex_UntilTo(14, 15, 0.15, true);
		}

		break;


	default:
		break;
	}






	/*
	2	=> 0.5
	6	=>6.5
	17	=>0.3
	*/


	return _int();
}

void CGrunt::Add_Dmg_to_Monster(_float iDmgAmount)
{
	m_DmgPassedTime = MonsterDmgTime;
	m_fDmgAmount += iDmgAmount;
	m_fHP -= iDmgAmount;

}

HRESULT CGrunt::SetUp_Components()
{

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_Grunt), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	FAILED_CHECK(m_pModel->Change_AnimIndex(2));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_GruntSwpanMesh), TAG_COM(Com_SubModel), (CComponent**)&m_pSubModel));
	FAILED_CHECK(m_pModel->Change_AnimIndex(1));
	FAILED_CHECK(m_pModel->Change_AnimIndex(0));


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));
	
	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

//Pivot: -0.150000, 1.099999, 0.250000,
	ColliderDesc.vScale = _float3(5.f, 5.f, 5.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-0.150000f, 1.099999f, 0.250000f,1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));

//Pivot: -0.020000, 0.780000, 0.250000,
	//size : 1.240000, 1.859999, 1.000000,
	ColliderDesc.vScale = _float3(1.240000f, 1.859999f, 1.240000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0.930000f, 0,1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_AABB, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();



	//Pivot  : -1.394999f , -0.140000f , -0.515000f , 1
	//size  : 12.570192f , 0.100000f , 0.100000f  
	ColliderDesc.vScale = _float3(1.2570192f, 0.100000f, 0.100000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0,0,0, 1);
	//ColliderDesc.vPosition = _float4(-1.394999f, -0.140000f, -0.515000f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();
	m_tCollisionAttachPtr = m_pModel->Find_AttachMatrix_InHirarchyNode("Bone06");
	NULL_CHECK_RETURN(m_tCollisionAttachPtr.pDefaultPivotMat, E_FAIL);



	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = 0.5;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_SubTransform), (CComponent**)&m_pParticleTargetTransformCom, &tDesc));
	


	return S_OK;
}

HRESULT CGrunt::SetUp_ParticleDesc()
{

	m_tParticleDesc = PARTICLEDESC();

	m_tParticleDesc.eParticleTypeID = Particle_Straight;

	m_tParticleDesc.FollowingTarget = m_pParticleTargetTransformCom;

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

	//m_tParticleDesc.ParticleStartRandomPosMin = _float3(-0.05f, 0.7f, -0.05f);
	//m_tParticleDesc.ParticleStartRandomPosMax = _float3(0.05f, 0.75f, 0.05f);
	m_tParticleDesc.ParticleStartRandomPosMin = _float3(-0.05f, 0.00f, -0.05f);
	m_tParticleDesc.ParticleStartRandomPosMax = _float3(0.05f, 0.01f, 0.05f);

	m_tParticleDesc.DepthTestON = true;
	m_tParticleDesc.AlphaBlendON = true;

	m_tParticleDesc.m_fAlphaTestValue = 0.1f;
	m_tParticleDesc.m_iPassIndex = 3;

	return S_OK;
}

HRESULT CGrunt::DashPatternWander(_double fDeltaTime)
{
	if (!(Check_Movable_Terrain(m_pTransformCom, m_vLookDir.XMVector(), 0.1f)))
	{
		m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
	}
	else
	{
		m_pTransformCom->LookDir(m_vLookDir.XMVector() * 0.15f + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * 0.85f);
		m_pTransformCom->MovetoDir((m_vLookDir.XMVector()), fDeltaTime);

	}
	return S_OK;

	return S_OK;
}

HRESULT CGrunt::Adjust_AnimMovedTransform(_double fDeltatime)
{
	if (!m_bSpwanedAnimFinished) return S_FALSE;

	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	_double PlayRate = m_pModel->Get_PlayRate();

	if (iNowAnimIndex != m_iOldAnimIndex || PlayRate > 0.95)
		m_iAdjMovedIndex = 0;


	if (PlayRate <= 0.95)
	{
		switch (iNowAnimIndex)
		{
		case 4:
			if (m_iAdjMovedIndex == 0 && PlayRate > 0.01)
			{

				m_pParticleTargetTransformCom->Set_IsOwnerDead(true);

				CChainExplosion::THRONDESC tDesc;

				tDesc.MeshKinds = 0;
				tDesc.MoveDir = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);
				tDesc.vPosition = m_pParticleTargetTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), L"Prototype_ChainExplosion", &tDesc);

				m_iAdjMovedIndex++;
			}

			break;

		case 5:
			if (m_iAdjMovedIndex == 0 && PlayRate > 0.01)
			{

				m_pParticleTargetTransformCom->Set_IsOwnerDead(false);
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);
				m_iAdjMovedIndex++;

			}
			break;

		case 8:
			if (m_iAdjMovedIndex == 0 && PlayRate > 0.01)
			{

				m_pParticleTargetTransformCom->Set_IsOwnerDead(true);

				CChainExplosion::THRONDESC tDesc;

				tDesc.MeshKinds = 0;
				tDesc.MoveDir = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);
				tDesc.vPosition = m_pParticleTargetTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), L"Prototype_ChainExplosion", &tDesc);

				m_iAdjMovedIndex++;
			}


			break;

		case 15:

			if (m_iAdjMovedIndex == 0 && PlayRate > 0.01)
			{
				m_pParticleTargetTransformCom->Set_IsOwnerDead(false);
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);
				m_iAdjMovedIndex++;
			}


			break;
		case 21:

			if (m_iAdjMovedIndex == 0 && PlayRate > 0.878787)
			{
				m_pParticleTargetTransformCom->Set_IsOwnerDead(true);
				m_iAdjMovedIndex++;
			}


			break;


			
		}
	}
	else
	{
		switch (iNowAnimIndex)
		{
		case 15:
			{
				m_pModel->Change_AnimIndex_UntilTo(21, 22, 0.15, true);

			}
			break;

		case 22:
		{
			m_bIsPatternFinished = true;
			m_PatternPassedTime = 0;
			m_PatternDelayTime = 0;

			m_pModel->Change_AnimIndex(2, 0.15, true);

		}
		break;
		}
	}

	m_iOldAnimIndex = iNowAnimIndex;
	return S_OK;
}

CGrunt * CGrunt::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CGrunt*	pInstance = new CGrunt(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CGrunt");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGrunt::Clone(void * pArg)
{
	CGrunt*	pInstance = new CGrunt(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CGrunt");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGrunt::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pSubModel);

	
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pParticleTargetTransformCom);
	
}
