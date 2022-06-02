#include "stdafx.h"
#include "..\public\HandyGirl.h"
#include "Terrain.h"
#include "TransparencyBall.h"
#include "ScrechedDust.h"
#include "DustWind.h"
#include "CircleTornado.h"
#include "BreakedGazebo.h"
#include "HyperVoice.h"
#include "HandyGirlBullet.h"



CHandyGirl::CHandyGirl(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonster(pDevice, pDeviceContext)
{
}

CHandyGirl::CHandyGirl(const CHandyGirl & rhs)
	: CMonster(rhs)
{
}

HRESULT CHandyGirl::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CHandyGirl::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, *((_float3*)pArg));

	ZeroMemory(m_bIsDmgAnimUpdated, sizeof(_bool) * 3);
	m_fHP = m_fMaxHP = 32;

	_Vector TargetAt = m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS);
	TargetAt = XMVectorSetY(TargetAt, m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).y);
	m_pTransformCom->LookAt(TargetAt);
	__super::SetUp_WanderLook(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK));

	m_PoleGrabPos = _float3(67, 10, 78);
	m_PoleGrabLook = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_LOOK);


	FAILED_CHECK(Ready_ParticleDesc());
	return S_OK;
}

_int CHandyGirl::Update(_double fDeltaTime)
{

	if (__super::Update(fDeltaTime) < 0)return -1;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);
	Update_DmgCalculate(fDeltaTime);



	if (g_pGameInstance->Get_DIKeyState(DIK_2) & DIS_Down)
		Add_Dmg_to_Monster(m_fMaxHP - 1);

	//if (g_pGameInstance->Get_DIKeyState(DIK_1)&DIS_Down)
	//{
	//	Make_Hand_PoleGrab();
	//}
	//if (g_pGameInstance->Get_DIKeyState(DIK_2)&DIS_Down)
	//{
	//	Make_Hand_Free();
	//}
	//if (g_pGameInstance->Get_DIKeyState(DIK_3)&DIS_Down)
	//{
	//	Enter_Hand();
	//}



	_uint NowIndex = m_pModel->Get_NowAnimIndex();

	if(!(NowIndex >= 34 && NowIndex <= 36))
	{


		if (m_bIsDealTime)
		{
			m_PatternDelayTime -= fDeltaTime;

			if (m_PatternDelayTime < 0)
			{
				m_bIsPatternFinished = false;
				m_PatternPassedTime = 0;
				m_PatternDelayTime = 3;
				m_bIsDealTime = false;
				m_pModel->Change_AnimIndex_UntilNReturn_Must(33, 33, 0, 0.15, true);

			}

		}
		else if (m_PatternDelayTime < 0 || !m_bIsPatternFinished)
		{
			Update_Pattern(fDeltaTime);
		}
		else
		{
			m_PatternDelayTime -= fDeltaTime;

			if (NowIndex == 1)
			{
				FAILED_CHECK(Update_WanderAround_PatterDelay(fDeltaTime, 3.f, 0.05f));
			}
			if (m_PatternDelayTime < 0)
			{
				//m_bIsDealTime = (rand() % 4) ? false : true;
				m_bIsDealTime = false;

				if (m_bIsDealTime)
				{
					m_bIsPatternFinished = false;
					m_PatternPassedTime = 0;
					m_PatternDelayTime = 7;
					m_pModel->Change_AnimIndex_UntilNReturn_Must(30, 32, 32, 0.15, true);
				}
			}
		}

	}



	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS),8);


	if ((NowIndex == 34))
	{
		FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime*0.3f, m_bIsOnScreen));
	}
	else
	{
		FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime*1.f, m_bIsOnScreen));
	}
	FAILED_CHECK(Adjust_MovedTransform_byAnim(fDeltaTime));

	if (m_bIsOnScreen)
	{
		for (_uint i = 0; i < 9; i++)
		{
			_Matrix			TransformMatrix = XMLoadFloat4x4(m_ArrCollisionAttach[i].pUpdatedNodeMat) * XMLoadFloat4x4(m_ArrCollisionAttach[i].pDefaultPivotMat);
			TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
			TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
			TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);
			m_pColliderCom->Update_Transform(i, TransformMatrix * m_pTransformCom->Get_WorldMatrix());
		}


		if (NowIndex != 35)
		{
			g_pGameInstance->Add_CollisionGroup(CollisionType_Monster, this, m_pColliderCom);

#ifdef _DEBUG
			FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
		}

		if (!m_bIsPatternFinished && !m_bIsDealTime)
		{
			g_pGameInstance->Add_CollisionGroup(CollisionType_MonsterWeapon, this, m_pColliderCom);

#ifdef _DEBUG
			FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
		}

	}


	//_Matrix			TransformMatrix = XMLoadFloat4x4(m_ArrCollisionAttach[7].pUpdatedNodeMat) * XMLoadFloat4x4(m_ArrCollisionAttach[7].pDefaultPivotMat);
	//TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	//TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	//TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);
	//_Vector PlayerPos = XMVector3TransformCoord(XMVectorSet(1.634999f, 0.490000f, -2.074999f, 1), TransformMatrix * m_pTransformCom->Get_WorldMatrix());
	//m_pPlayerTransfrom->Set_MatrixState(CTransform::STATE_POS, PlayerPos + XMVectorSet(0,-1,0,0));

	return _int();
}

_int CHandyGirl::LateUpdate(_double fDeltaTime)
{

	FAILED_CHECK(__super::Set_Monster_On_Terrain(m_pTransformCom, fDeltaTime));

	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));


	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CHandyGirl::Render()
{
	if (__super::Render() < 0)
		return -1;


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

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 0, i, "g_BoneMatrices"));
	}

	return _int();
}

_int CHandyGirl::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	NULL_CHECK_RETURN(m_pModel, E_FAIL);

	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{
		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 10, i, "g_BoneMatrices"));

	}
	return _int();
}

void CHandyGirl::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{
	case Engine::CollisionType_Player:

		if (m_pModel->Get_NowAnimIndex() == 16 || m_pModel->Get_NowAnimIndex() == 17)
		{
			if (!m_pPlayer->Get_IsAttached())
			{
				m_pPlayer->Set_IsAttached(true);
				m_bIsGrabed = true;
			}
		}

		else
		{
			pConflictedCollider->Set_Conflicted();
			((CPlayer*)(pConflictedObj))->Add_Dmg_to_Player(rand() % 2 + 1);
		}


		break;
	case Engine::CollisionType_PlayerWeapon:
		break;



	case Engine::CollisionType_Terrain:
		break;
	default:
		break;
	}

}

_int CHandyGirl::Update_DmgCalculate(_double fDeltaTime)
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

	if (m_fHP < 0)
	{
		Make_Hand_PoleGrab();
		m_fHP = 99999999.f;

	}

	return 0;
}

_int CHandyGirl::Update_Pattern(_double fDeltaTime)
{

	m_PatternPassedTime += fDeltaTime;
	if (m_bIsPatternFinished)
	{
		m_ePattern += 1;
		m_ePattern = 1;
		if (m_ePattern > 6) m_ePattern = 0;
		m_bIsPatternFinished = false;
		m_PatternPassedTime = 0;
	}


	switch (m_ePattern)
	{
	case 0:
		if (!m_PatternPassedTime)
			m_pModel->Change_AnimIndex_UntilNReturn_Must(2, 3, 0, 0.15, true);
		break;
	case 1:
		if (!m_PatternPassedTime)
			m_pModel->Change_AnimIndex_UntilNReturn_Must(4, 6, 0, 0.15, true);
		break;
	case 2:
		if (!m_PatternPassedTime)
			m_pModel->Change_AnimIndex_UntilNReturn_Must(7, 8, 0, 0.15, true);
		break;
	case 3:
		if (!m_PatternPassedTime)
			m_pModel->Change_AnimIndex_UntilNReturn_Must(9, 13, 0, 0.15, true);
		break;
	case 4:
		if (!m_PatternPassedTime)
			m_pModel->Change_AnimIndex_UntilNReturn_Must(14, 14, 0, 0.15, true);
		break;
	case 5:
		if (!m_PatternPassedTime)
			m_pModel->Change_AnimIndex_UntilNReturn_Must(15, 17, 0, 0.15, true);
		break;
	case 6:
		if (!m_PatternPassedTime)
			m_pModel->Change_AnimIndex_UntilNReturn_Must(22, 27, 0, 0.15, true);
		break;


	default:
		break;
	}






	return _int();
}

void CHandyGirl::Add_Dmg_to_Monster(_float iDmgAmount)
{
	m_DmgPassedTime = MonsterDmgTime;
	m_fDmgAmount += iDmgAmount;
	m_fHP -= iDmgAmount;

#ifdef _DEBUG

	wstring ttDebugLog = L"HadyGirl HP: " + to_wstring(m_fHP) + L"\n";;
	OutputDebugStringW(ttDebugLog.c_str());
	
#endif // _DEBUG


}

void CHandyGirl::Make_Hand_PoleGrab()
{

	m_PatternDelayTime = 0;
	m_bIsPatternFinished = false;
	m_PatternPassedTime = 0;
	m_bIsDealTime = true;

	GetSingle(CUtilityMgr)->Set_MonsterBlurOn(false);
	list<CGameObject*>* pBulletList = g_pGameInstance->Get_ObjectList_from_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet2));

	if (pBulletList != nullptr)
	{


		for (auto& pObj : *pBulletList)
		{
			((CScrechedDust*)(pObj))->Set_Spout();
		}

	}
	m_pModel->Change_AnimIndex_UntilNReturn_Must(34, 35, 35, 0.15, true);
}

void CHandyGirl::Make_Hand_Free()
{
	m_bIsDealTime = false;
	m_PatternDelayTime = 5;
	m_pTransformCom->Set_MoveSpeed(3.5f);
	m_pModel->Change_AnimIndex_ReturnTo_Must(36, 0, 0.15, true);
	m_fHP = m_fMaxHP;
}

void CHandyGirl::Enter_Hand()
{
	m_PatternDelayTime = 7;
	m_bIsPatternFinished = true;

	m_pModel->Change_AnimIndex_ReturnTo_Must(37, 0, 0.15, true);
	m_fHP = m_fMaxHP;
}

HRESULT CHandyGirl::SetUp_Components()
{
	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = 3.5;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_HandyGirl), TAG_COM(Com_Model), (CComponent**)&m_pModel));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));
	
	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	//Pivot  : -0.025000f , -0.020000f , -3.594998f , 1
	//size  : 7.320073f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(7.320073f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-0.025000f, -0.020000f, -3.594998f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));

	m_ArrCollisionAttach[0] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bone_Head");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[0].pDefaultPivotMat, E_FAIL);


	//Pivot  : 0.035000f , 0.180000f , -2.784998f , 1
	//size  : 5.580033f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(5.580033f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.035000f, 0.180000f, -2.784998f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[1] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-Pelvis");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[1].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer(0);


	//Pivot  : -0.075000f , -0.020000f , -3.974997f , 1
	//size  : 3.279998f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(3.279998f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-0.075000f, -0.020000f, -3.974997f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[2] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bone_Head");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[2].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer(1);


	//Pivot  : 0.475000f , -0.510000f , -0.845000f , 1
	//size  : 1.589999f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(1.589999f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.475000f, -0.510000f, -0.845000f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[3] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-L-Foot");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[3].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer(1);



	//Pivot  : -0.515000f , -0.260000f , -0.855000f , 1
	//size  : 1.719999f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(1.719999f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-0.515000f, -0.260000f, -0.855000f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[4] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-R-Foot");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[4].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer(1);


	//Pivot  : -0.964999f , -0.530000f , -2.304999f , 1
	//size  : 1.090000f , 1.000000f , 1.000000f  

	ColliderDesc.vScale = _float3(1.090000f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-0.964999f, -0.530000f, -2.304999f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[5] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-TailNub");
	//m_ArrCollisionAttach[5] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bone_Ring");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[5].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer(1);


	//Pivot  : -1.514999f , -0.020000f , -1.414999f , 1
	//size  : 1.510000f , 1.000000f , 1.000000f  

	ColliderDesc.vScale = _float3(1.510000f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-1.514999f, -0.020000f, -1.414999f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[6] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-R-Forearm");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[6].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer(1);


	//Pivot  : 1.634999f , 0.490000f , -2.074999f , 1
	//size  : 1.689999f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(1.689999f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(1.634999f, 0.490000f, -2.074999f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[7] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-L-Hand");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[7].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer(1);



	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	//0.954999f, -0.080000f, 0.684999f, 1
	ColliderDesc.vPosition = _float4(0.439999f, -0.34f, -0.170001f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[8] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-L-Toe0");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[8].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer(1);



	__super::SetUp_WanderLook(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK));

	return S_OK;
}

HRESULT CHandyGirl::Ready_ParticleDesc()
{
	PARTICLEDESC  tParticleDesc = PARTICLEDESC();

	tParticleDesc.eParticleTypeID = Particle_Ball;

	tParticleDesc.FollowingTarget = nullptr;

	tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	tParticleDesc.szTextureLayerTag = L"Petal";
	tParticleDesc.iSimilarLayerNum = 3;

	tParticleDesc.TextureChageFrequency = 1;
	tParticleDesc.vTextureXYNum = _float2(8, 4);

	tParticleDesc.TotalParticleTime = 0.1f;
	tParticleDesc.EachParticleLifeTime = 0.34f;
	tParticleDesc.MaxParticleCount = 7;

	tParticleDesc.SizeChageFrequency = 0;
	tParticleDesc.ParticleSize = _float3(0.25f, 0.25f, 0.25f);
	tParticleDesc.ParticleSize2 = _float3(0.5f, 0.5f, 0.5f);

	tParticleDesc.ColorChageFrequency = 0;
	tParticleDesc.TargetColor = _float4(1.f, 1.f, 1.f, 1.f);
	tParticleDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 1.f);


	tParticleDesc.Particle_Power = 2;
	tParticleDesc.PowerRandomRange = _float2(0.8f, 1.5f);

	tParticleDesc.vUp = _float3(0, 1, 0);

	tParticleDesc.MaxBoundaryRadius = 6;

	tParticleDesc.m_bIsUI = false;
	tParticleDesc.m_bUIDepth = 0;

	tParticleDesc.ParticleStartRandomPosMin = _float3(-0.25f);
	tParticleDesc.ParticleStartRandomPosMax = _float3(0.25f);

	tParticleDesc.DepthTestON = true;
	tParticleDesc.AlphaBlendON = false;

	tParticleDesc.m_fAlphaTestValue = 0.1f;
	tParticleDesc.m_iPassIndex = 3;

	m_vecParticleDesc.push_back(tParticleDesc);


	////////////1//////////////////////////////////////////////////////////////
	tParticleDesc = PARTICLEDESC();

	tParticleDesc.eParticleTypeID = Particle_Spread;

	tParticleDesc.FollowingTarget = nullptr;

	tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	tParticleDesc.szTextureLayerTag = L"Dust4";
	tParticleDesc.iSimilarLayerNum = 1;

	tParticleDesc.TextureChageFrequency = 1;
	tParticleDesc.vTextureXYNum = _float2(5, 4);

	tParticleDesc.TotalParticleTime = 0.1f;
	tParticleDesc.EachParticleLifeTime = 0.34f;
	tParticleDesc.MaxParticleCount = 30;

	tParticleDesc.SizeChageFrequency = 1;
	tParticleDesc.ParticleSize = _float3(5.0);
	tParticleDesc.ParticleSize2 = _float3(2.f);

	tParticleDesc.ColorChageFrequency = 1;
	tParticleDesc.TargetColor = _float4(1.f, 0.443f, 1.f, 1.f);
	tParticleDesc.TargetColor2 = _float4(1.f, 0.743f, 1.f, 1.f);


	tParticleDesc.Particle_Power = 30;
	tParticleDesc.PowerRandomRange = _float2(0.8f, 1.2f);

	tParticleDesc.vUp = _float3(0, 1, 0);

	tParticleDesc.MaxBoundaryRadius = 10;

	tParticleDesc.m_bIsUI = false;
	tParticleDesc.m_bUIDepth = 0;

	tParticleDesc.ParticleStartRandomPosMin = _float3(0, 0.3f, 0);
	tParticleDesc.ParticleStartRandomPosMax = _float3(0, 0.3f, 0);

	tParticleDesc.DepthTestON = true;
	tParticleDesc.AlphaBlendON = true;

	tParticleDesc.m_fAlphaTestValue = 0.1f;
	tParticleDesc.m_iPassIndex = 10;

	m_vecParticleDesc.push_back(tParticleDesc);

	////////////2//////////////////////////////////////////////////////////////


	tParticleDesc.eParticleTypeID = Particle_Cone;

	tParticleDesc.FollowingTarget = nullptr;

	tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	tParticleDesc.szTextureLayerTag = L"Petal";
	tParticleDesc.iSimilarLayerNum = 3;

	tParticleDesc.TextureChageFrequency = 5;
	tParticleDesc.vTextureXYNum = _float2(8, 4);

	tParticleDesc.TotalParticleTime = 0.1f;
	tParticleDesc.EachParticleLifeTime = 1.5f;
	tParticleDesc.MaxParticleCount = 60;

	tParticleDesc.SizeChageFrequency = 0;
	tParticleDesc.ParticleSize = _float3(0.25f, 0.25f, 0.25f);
	tParticleDesc.ParticleSize2 = _float3(0.5f, 0.5f, 0.5f);

	tParticleDesc.ColorChageFrequency = 0;
	tParticleDesc.TargetColor = _float4(1.f, 1.f, 1.f, 1.f);
	tParticleDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 1.f);


	tParticleDesc.Particle_Power = 20;
	tParticleDesc.PowerRandomRange = _float2(0.2f, 1.5f);

	tParticleDesc.vUp = _float3(0, 1, 0);

	tParticleDesc.MaxBoundaryRadius = 10;

	tParticleDesc.m_bIsUI = false;
	tParticleDesc.m_bUIDepth = 0;

	tParticleDesc.ParticleStartRandomPosMin = _float3(-2, 0.3f, -2);
	tParticleDesc.ParticleStartRandomPosMax = _float3(2, 0.3f, 2);

	tParticleDesc.DepthTestON = true;
	tParticleDesc.AlphaBlendON = false;

	tParticleDesc.m_fAlphaTestValue = 0.1f;
	tParticleDesc.m_iPassIndex = 3;

	m_vecParticleDesc.push_back(tParticleDesc);

	//////////3////////////////////////////////////////////////////////////////
	tParticleDesc = PARTICLEDESC();

	tParticleDesc.eParticleTypeID = Particle_Ball;

	tParticleDesc.FollowingTarget = nullptr;

	tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	tParticleDesc.szTextureLayerTag = L"Petal";
	tParticleDesc.iSimilarLayerNum = 3;

	tParticleDesc.TextureChageFrequency = 2;
	tParticleDesc.vTextureXYNum = _float2(8, 4);

	tParticleDesc.TotalParticleTime = 0.1f;
	tParticleDesc.EachParticleLifeTime = 0.68f;
	tParticleDesc.MaxParticleCount = 14;

	tParticleDesc.SizeChageFrequency = 0;
	tParticleDesc.ParticleSize = _float3(0.25f, 0.25f, 0.25f);
	tParticleDesc.ParticleSize2 = _float3(0.5f, 0.5f, 0.5f);

	tParticleDesc.ColorChageFrequency = 0;
	tParticleDesc.TargetColor = _float4(1.f, 1.f, 1.f, 1.f);
	tParticleDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 1.f);


	tParticleDesc.Particle_Power = 4;
	tParticleDesc.PowerRandomRange = _float2(0.8f, 1.5f);

	tParticleDesc.vUp = _float3(0, 1, 0);

	tParticleDesc.MaxBoundaryRadius = 10;

	tParticleDesc.m_bIsUI = false;
	tParticleDesc.m_bUIDepth = 0;

	tParticleDesc.ParticleStartRandomPosMin = _float3(-0.25f);
	tParticleDesc.ParticleStartRandomPosMax = _float3(0.25f);

	tParticleDesc.DepthTestON = true;
	tParticleDesc.AlphaBlendON = false;

	tParticleDesc.m_fAlphaTestValue = 0.1f;
	tParticleDesc.m_iPassIndex = 3;

	m_vecParticleDesc.push_back(tParticleDesc);


	//////////4////////////////////////////////////////////////////////////////

	tParticleDesc = PARTICLEDESC();

	tParticleDesc.eParticleTypeID = Particle_Fixed_LookFree;

	tParticleDesc.FollowingTarget = nullptr;

	tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	tParticleDesc.szTextureLayerTag = L"HorseHit";
	tParticleDesc.iSimilarLayerNum = 2;

	tParticleDesc.TextureChageFrequency = 1;
	tParticleDesc.vTextureXYNum = _float2(3, 3);

	tParticleDesc.TotalParticleTime = 0.01f;
	tParticleDesc.EachParticleLifeTime = 0.34f;
	tParticleDesc.MaxParticleCount = 1;

	tParticleDesc.SizeChageFrequency = 1;
	tParticleDesc.ParticleSize = _float3(2.f);
	tParticleDesc.ParticleSize2 = _float3(8.5f);

	tParticleDesc.ColorChageFrequency = 1;
	tParticleDesc.TargetColor = _float4(0.8f, 0.8f, 0.5f, 0.3f);
	tParticleDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 0.8f);


	tParticleDesc.Particle_Power = 0;
	tParticleDesc.PowerRandomRange = _float2(0.5f, 1.5f);

	tParticleDesc.vUp = _float3(0, 0, 1);

	tParticleDesc.MaxBoundaryRadius = 20;

	tParticleDesc.m_bIsUI = false;
	tParticleDesc.m_bUIDepth = 0;

	tParticleDesc.ParticleStartRandomPosMin = _float3(-0, 0, -0);
	tParticleDesc.ParticleStartRandomPosMax = _float3(0, 0, 0);

	tParticleDesc.DepthTestON = true;
	tParticleDesc.AlphaBlendON = false;

	tParticleDesc.m_fAlphaTestValue = 0.1f;
	tParticleDesc.m_iPassIndex = 9;

	m_vecParticleDesc.push_back(tParticleDesc);
	return S_OK;
}

HRESULT CHandyGirl::Adjust_MovedTransform_byAnim(_double fDeltatime)
{
	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	_double PlayRate = m_pModel->Get_PlayRate();

	if (iNowAnimIndex != m_iOldAnimIndex || PlayRate > 0.95)
		m_iAdjMovedIndex = 0;

	//static float Power = 7.2f;
	//if (g_pGameInstance->Get_DIKeyState(DIK_UP)& DIS_Down)
	//{
	//	Power += 0.2f;
	//	string ttszLog = "//Power  : " + to_string(Power) + "\n";

	//	wstring ttDebugLog;
	//	ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

	//	OutputDebugStringW(ttDebugLog.c_str());

	//}
	//else if (g_pGameInstance->Get_DIKeyState(DIK_DOWN)& DIS_Down)
	//{
	//	Power -= 0.2f;
	//	string ttszLog = "//Power  : " + to_string(Power) + "\n";

	//	wstring ttDebugLog;
	//	ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

	//	OutputDebugStringW(ttDebugLog.c_str());
	//}


	if (PlayRate <= 0.95)
	{
		switch (iNowAnimIndex)
		{
		case 0:

			if (m_iAdjMovedIndex == 0 && PlayRate > 0.01)
			{
				m_PatternDelayTime = 5;
				m_bIsPatternFinished = true;
				m_pModel->Change_AnimIndex(1, 0.15, true);
			}

			break;

		case 2:
		{
			_Vector RandFloat = GetSingle(CUtilityMgr)->RandomFloat3(-0.3f, 0.3f).XMVector();
			XMVectorSetY(RandFloat, 0);
			_Vector PlayerPos = m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS);
			XMVectorSetY(PlayerPos, 0);
			_Vector ObjPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
			XMVectorSetY(ObjPos, 0);
			m_vLookDir = XMVector3Normalize(PlayerPos + RandFloat - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
			m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));
		}
			break;

		case 3:
		{
			if (m_iAdjMovedIndex == 0 && PlayRate > 0.266666)
			{
				CDustWind::DUSTWINDDESC tDesc;

				tDesc.vPosition = (m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));
				tDesc.ToTalLifeTime = 0.75f;
				tDesc.StartScale = _float3(2, 5, 2);
				tDesc.TargetScale = _float3(8, 1.f, 8);
				tDesc.eEasingType = TYPE_Linear;
				tDesc.vColor = _float4(1, 0.643f, 1, 1.f);

				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));


				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 1 && PlayRate > 0.32222)
			{
				CDustWind::DUSTWINDDESC tDesc;

				tDesc.vPosition = (m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));
				tDesc.ToTalLifeTime = 0.75f;
				tDesc.StartScale = _float3(2, 5, 2);
				tDesc.TargetScale = _float3(8, 1.f, 8);
				tDesc.eEasingType = TYPE_Linear;
				tDesc.vColor = _float4(1, 0.643f, 1, 1.f);

				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));
				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 2 && PlayRate > 0.3777777777)
			{
				CDustWind::DUSTWINDDESC tDesc;

				tDesc.vPosition = (m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));
				tDesc.ToTalLifeTime = 0.75f;
				tDesc.StartScale = _float3(2, 5, 2);
				tDesc.TargetScale = _float3(8, 1.f, 8);
				tDesc.eEasingType = TYPE_Linear;
				tDesc.vColor = _float4(1, 0.643f, 1, 1.f);

				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));
				m_iAdjMovedIndex++;
			}
		}
		break;

		case 5:
		{
			_Vector RandFloat = GetSingle(CUtilityMgr)->RandomFloat3(-0.3f, 0.3f).XMVector();
			XMVectorSetY(RandFloat, 0);
			_Vector PlayerPos = m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS);
			XMVectorSetY(PlayerPos, 0);
			_Vector ObjPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
			XMVectorSetY(ObjPos, 0);
			m_vLookDir = XMVector3Normalize(PlayerPos + RandFloat - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
			m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));
		}

		if (m_iAdjMovedIndex == 0 && PlayRate > 0.2727272727)
		{
			CHandyGirlBullet::HANDYGRILBULLETDESC tDesc;


			tDesc.MoveDir = m_vLookDir;
			tDesc.vPosition = m_pColliderCom->Get_ColliderPosition();

			_uint RandNum = rand() % 3 + 3;

			for (_uint i  = 0 ; i< RandNum; i++)
			{
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_HandyGirlBullet), &tDesc);
			}


			m_iAdjMovedIndex++;
		}

		else if (m_iAdjMovedIndex == 1 && PlayRate > 0.5636363636363)
		{
			CHandyGirlBullet::HANDYGRILBULLETDESC tDesc;

			tDesc.MoveDir = m_vLookDir;
			tDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
			_uint RandNum = rand() % 3 + 3;

			for (_uint i = 0; i < RandNum; i++)
			{
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_HandyGirlBullet), &tDesc);
			}
			m_iAdjMovedIndex++;
		}
		else if (m_iAdjMovedIndex == 2 && PlayRate > 0.9090909)
		{
			CHandyGirlBullet::HANDYGRILBULLETDESC tDesc;

			tDesc.MoveDir = m_vLookDir;
			tDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
			_uint RandNum = rand() % 3 + 3;

			for (_uint i = 0; i < RandNum; i++)
			{
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_HandyGirlBullet), &tDesc);
			}
			m_iAdjMovedIndex++;
		}

			break;

		case 7:
			if (PlayRate < 0.5f)
			{
				_Vector RandFloat = GetSingle(CUtilityMgr)->RandomFloat3(-0.3f, 0.3f).XMVector();
				XMVectorSetY(RandFloat, 0);
				_Vector PlayerPos = m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS);
				XMVectorSetY(PlayerPos, 0);
				_Vector ObjPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
				XMVectorSetY(ObjPos, 0);
				m_vLookDir = XMVector3Normalize(PlayerPos + RandFloat - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
				m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));
			}
			else
			{
				m_pTransformCom->MovetoDir_bySpeed(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 15.f, fDeltatime);

				CTerrain* pTerrain = (CTerrain*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));
				_bool bIsOn = false;
				_uint eNowTile = Tile_End;
				_float3 CaculatedPos = pTerrain->PutOnTerrain(&bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector(), nullptr, &eNowTile);


				if (eNowTile == Tile_None || bIsOn)
					m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);

			}

			break;
		case 8:

			if (m_iAdjMovedIndex == 0)
			{

				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.3f, _float4(0.8f));

				m_vecParticleDesc[2].FixedTarget = m_vecParticleDesc[1].FixedTarget = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[1]);
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[2]);

				CGameInstance* pInstance = g_pGameInstance;
				CBreakedGazebo::BREAKEDGAZBODESC tDesc;
				tDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);

				for (_uint i = 0; i < 20; i++)
				{
					tDesc.MeshKinds = rand() % 2 + Prototype_QBattleTowerParticleA;
					pInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_BreakedGazbo), &tDesc);
				}


				CCircleTornado::CIRCLETORNADODESC tTornadoDesc;
				tTornadoDesc.vLook = _float3(0.00000001f, 1.f, 0);
				tTornadoDesc.vPosition = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
				tTornadoDesc.fSize = 5.0f;
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tTornadoDesc);
				tTornadoDesc.vPosition = tTornadoDesc.vPosition.XMVector() + XMVectorSet(0, 0.5f, 0, 0);
				tTornadoDesc.fSize = 3.0f;
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tTornadoDesc);
				m_iAdjMovedIndex++;

			}

			break;
		case 9:
		{
			_Vector RandFloat = GetSingle(CUtilityMgr)->RandomFloat3(-0.3f, 0.3f).XMVector();
			XMVectorSetY(RandFloat, 0);
			_Vector PlayerPos = m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS);
			XMVectorSetY(PlayerPos, 0);
			_Vector ObjPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
			XMVectorSetY(ObjPos, 0);
			m_vLookDir = XMVector3Normalize(PlayerPos + RandFloat - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
			m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));


			static _double ScrechedDustSpwanTimer = 0;

			if (PlayRate < 0.12)
			{
				ScrechedDustSpwanTimer = 0;
			}
			else
			{
				ScrechedDustSpwanTimer -= fDeltatime;

				if (ScrechedDustSpwanTimer < 0)
				{
					m_vecParticleDesc[0].FixedTarget = m_pColliderCom->Get_ColliderPosition(8);
					GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[0]);
					ScrechedDustSpwanTimer = 0.1f;
				}
			}
		}
			break;

		case 10:
		{
			if (m_iAdjMovedIndex == 0)
			{
				CTransparencyBall::BALLMESHDESC tDesc;
				tDesc.vPosition = m_pColliderCom->Get_ColliderPosition(8);
				tDesc.fSize = 1;
				tDesc.HandyGirl = this;

				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), L"TransparencyBall", &tDesc);
				m_iAdjMovedIndex++;
			}
		}
		break;


		case 11:
		{
			static _double ScrechedDustSpwanTimer = 0;

			if (PlayRate < 0.21739130)
			{
				ScrechedDustSpwanTimer = 0;
			}

			else if (PlayRate < 0.9130434782608)
			{
				ScrechedDustSpwanTimer -= fDeltatime;

				if (ScrechedDustSpwanTimer < 0)
				{
					CScrechedDust::BREAKEDGAZBODESC tDesc;

					tDesc.vPosition = m_pColliderCom->Get_ColliderPosition(8).XMVector() + XMVectorSet(0,0.25f,0,0);

					tDesc.MoveDir = XMVector3Normalize(XMVectorSetY(tDesc.vPosition.XMVector(), 0) - XMVectorSetY(m_pColliderCom->Get_ColliderPosition(0).XMVector(), 0));
					
					g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet2), TAG_OP(Prototype_ScrechedDust), &tDesc);

					ScrechedDustSpwanTimer = 0.1f;
				}
			}
		}
		break;

		case 12:
		{

			if (m_iAdjMovedIndex== 0 && PlayRate > 0.47142857)
			{
				list<CGameObject*>* pBulletList = g_pGameInstance->Get_ObjectList_from_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet2));
				NULL_CHECK_BREAK(pBulletList);

				for (auto& pObj : *pBulletList)
				{
					((CScrechedDust*)(pObj))->Set_Spout();
				}
				m_iAdjMovedIndex++;
			}
		}
		break;

		case 14:
		{
			static _double HyperVoiceTimer = 0;


			if (m_iAdjMovedIndex == 0)
			{
				GetSingle(CUtilityMgr)->Set_MonsterBlurOn(true);
				m_iAdjMovedIndex++;
			}

			GetSingle(CUtilityMgr)->Set_MonsterBlurPos(m_pColliderCom->Get_ColliderPosition(0));

			if (PlayRate < 0.3636)
			{
				HyperVoiceTimer = 0;
				_Vector RandFloat = GetSingle(CUtilityMgr)->RandomFloat3(-0.3f, 0.3f).XMVector();
				XMVectorSetY(RandFloat, 0);
				_Vector PlayerPos = m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS);
				XMVectorSetY(PlayerPos, 0);
				_Vector ObjPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
				XMVectorSetY(ObjPos, 0);
				m_vLookDir = XMVector3Normalize(PlayerPos + RandFloat - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
				m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));
			}

			if (PlayRate > 0.409090909 && PlayRate < 0.727272727)
			{
				HyperVoiceTimer -= fDeltatime;

				if (HyperVoiceTimer < 0)
				{


					CHyperVoice::CIRCLETORNADODESC tDesc;

					tDesc.fSize = 0.5f;
					tDesc.Power = 15.f;
					tDesc.vLook = m_vLookDir;
					tDesc.vPosition = m_pColliderCom->Get_ColliderPosition(0);


					FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_HyperVoice), &tDesc));
					HyperVoiceTimer = 0.1f;


				}

			}

			if (m_iAdjMovedIndex == 1 && PlayRate > 0.727272727)
			{
				GetSingle(CUtilityMgr)->Set_MonsterBlurOn(false);
				m_iAdjMovedIndex++;
			}



		}
			break;


		case 15:
		{
			_Vector RandFloat = GetSingle(CUtilityMgr)->RandomFloat3(-0.3f, 0.3f).XMVector();
			XMVectorSetY(RandFloat, 0);
			_Vector PlayerPos = m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS);
			XMVectorSetY(PlayerPos, 0);
			_Vector ObjPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
			XMVectorSetY(ObjPos, 0);
			m_vLookDir = XMVector3Normalize(PlayerPos + RandFloat - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
			m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));
		}
			break;


		case 16:
		case 17:
		{
			static _double ParticleTimer = 0;

			if (PlayRate < 0.001)
				ParticleTimer = 0.1f;

			ParticleTimer -= fDeltatime;
			if (ParticleTimer < 0)
			{
				m_vecParticleDesc[3].FixedTarget = m_pColliderCom->Get_ColliderPosition(0);
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[3]);
				ParticleTimer = 0.1f;
			}

		}


			if (m_bIsGrabed)
			{
				m_pModel->Change_AnimIndex_UntilNReturn_Must(18, 21, 0, 0.15, true);
				m_pTransformCom->Set_MoveSpeed(3.5f);
			}

			else
			{
				m_pTransformCom->Set_MoveSpeed(15.5f);
				FAILED_CHECK(Update_WanderAround_PatterDelay(fDeltatime, 1.f, 0.15f));

				if (iNowAnimIndex == 17 && PlayRate > 0.90)
				{
					m_pTransformCom->Set_MoveSpeed(3.5f);
					m_pModel->Change_AnimIndex(0, 0.45, true);
				}
			}
			break;

		case 18:
		{

			_Matrix			TransformMatrix = XMLoadFloat4x4(m_ArrCollisionAttach[7].pUpdatedNodeMat) * XMLoadFloat4x4(m_ArrCollisionAttach[7].pDefaultPivotMat);
			TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
			TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
			TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);
			_Vector PlayerPos = XMVector3TransformCoord(XMVectorSet(1.634999f, 0.490000f, -2.074999f, 1), TransformMatrix * m_pTransformCom->Get_WorldMatrix());
			m_pPlayerTransfrom->Set_MatrixState(CTransform::STATE_POS, PlayerPos + XMVectorSet(0, -1, 0, 0));

		}
		break;
		case 19:
		{

			_Matrix			TransformMatrix = XMLoadFloat4x4(m_ArrCollisionAttach[7].pUpdatedNodeMat) * XMLoadFloat4x4(m_ArrCollisionAttach[7].pDefaultPivotMat);
			TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
			TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
			TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);
			_Vector PlayerPos = XMVector3TransformCoord(XMVectorSet(1.634999f, 0.490000f, -2.074999f, 1), TransformMatrix * m_pTransformCom->Get_WorldMatrix());
			m_pPlayerTransfrom->Set_MatrixState(CTransform::STATE_POS, PlayerPos + XMVectorSet(0, -1, 0, 0));

		}

		if (PlayRate > 0.01)
		{

			GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.1f, _float4(0.5f));
		}
			break;

		case 20:
			if (PlayRate < 0.346938)
			{
				_Matrix			TransformMatrix = XMLoadFloat4x4(m_ArrCollisionAttach[7].pUpdatedNodeMat) * XMLoadFloat4x4(m_ArrCollisionAttach[7].pDefaultPivotMat);
				TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
				TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
				TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);
				_Vector PlayerPos = XMVector3TransformCoord(XMVectorSet(1.634999f, 0.490000f, -2.074999f, 1), TransformMatrix * m_pTransformCom->Get_WorldMatrix());
				m_pPlayerTransfrom->Set_MatrixState(CTransform::STATE_POS, PlayerPos + XMVectorSet(0, -1, 0, 0));
			}
			else
			{
				if (m_bIsGrabed)
				{

					GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.25f, _float4(2.5f));
					m_bIsGrabed = false;
					m_pPlayer->Set_IsAttached(false);
				}

			}

			break;


		case 22:
		{
			_Vector RandFloat = GetSingle(CUtilityMgr)->RandomFloat3(-0.3f, 0.3f).XMVector();
			XMVectorSetY(RandFloat, 0);
			_Vector PlayerPos = m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS);
			XMVectorSetY(PlayerPos, 0);
			_Vector ObjPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
			XMVectorSetY(ObjPos, 0);
			m_vLookDir = XMVector3Normalize(PlayerPos + RandFloat - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
			m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));
		}
			break;

		case 23:
		case 24:
		{
			_Vector RandFloat = GetSingle(CUtilityMgr)->RandomFloat3(-0.3f, 0.3f).XMVector();
			XMVectorSetY(RandFloat, 0);
			_Vector PlayerPos = m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS);
			XMVectorSetY(PlayerPos, 0);
			_Vector ObjPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
			XMVectorSetY(ObjPos, 0);
			m_vLookDir = XMVector3Normalize(PlayerPos + RandFloat - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
			m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));

			m_pTransformCom->Set_MoveSpeed(5.5f);
			FAILED_CHECK(Update_WanderAround_PatterDelay(fDeltatime, 1.f, 0.15f));

		}

		if (m_iAdjMovedIndex == 0)
		{
			CUtilityMgr* pUtil = GetSingle(CUtilityMgr);

			pUtil->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.1f, _float4(0.8f));

			m_vecParticleDesc[4].FixedTarget =  m_pColliderCom->Get_ColliderPosition(3);
			m_vecParticleDesc[4].FixedTarget.y = 10.1f;

			pUtil->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[4]);


			pUtil->Start_InstanceParticle(m_eNowSceneNum, m_vecParticleDesc[4].FixedTarget, 2);
			pUtil->Start_InstanceParticle(m_eNowSceneNum, m_vecParticleDesc[4].FixedTarget, 2);

			CCircleTornado::CIRCLETORNADODESC tTornadoDesc;
			tTornadoDesc.vLook = _float3(0.00000001f, 1.f, 0);
			tTornadoDesc.vPosition = m_vecParticleDesc[4].FixedTarget;
			tTornadoDesc.vPosition.y -= 0.05f;
			tTornadoDesc.fSize = 1.5f;
			g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tTornadoDesc);
			tTornadoDesc.vPosition = tTornadoDesc.vPosition.XMVector() + XMVectorSet(0, 0.11f, 0, 0);
			tTornadoDesc.fSize = 0.8f;
			g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tTornadoDesc);

			m_iAdjMovedIndex++;
		}
		else if (m_iAdjMovedIndex == 1 && PlayRate > 0.5)
		{
			CUtilityMgr* pUtil = GetSingle(CUtilityMgr);

			pUtil->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.1f, _float4(0.8f));

			m_vecParticleDesc[4].FixedTarget = m_pColliderCom->Get_ColliderPosition(4);
			m_vecParticleDesc[4].FixedTarget.y = 10.1f;

			pUtil->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[4]);


			pUtil->Start_InstanceParticle(m_eNowSceneNum, m_vecParticleDesc[4].FixedTarget, 2);
			pUtil->Start_InstanceParticle(m_eNowSceneNum, m_vecParticleDesc[4].FixedTarget, 2);

			CCircleTornado::CIRCLETORNADODESC tTornadoDesc;
			tTornadoDesc.vLook = _float3(0.00000001f, 1.f, 0);
			tTornadoDesc.vPosition = m_vecParticleDesc[4].FixedTarget;
			tTornadoDesc.vPosition.y -= 0.05f;
			tTornadoDesc.fSize = 1.5f;
			g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tTornadoDesc);
			tTornadoDesc.vPosition = tTornadoDesc.vPosition.XMVector() + XMVectorSet(0, 0.11f, 0, 0);
			tTornadoDesc.fSize = 0.8f;
			g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tTornadoDesc);
			m_iAdjMovedIndex++;
		}

			break;

		case 25:
			m_pTransformCom->Set_MoveSpeed(3.5f);

			break;

		case 26:
		if (m_iAdjMovedIndex == 0 && PlayRate > 0.4666666666)
		{
			CUtilityMgr* pUtil = GetSingle(CUtilityMgr);

			pUtil->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.3f, _float4(2.5f));

			m_vecParticleDesc[4].FixedTarget = m_pColliderCom->Get_ColliderPosition(4);
			m_vecParticleDesc[4].FixedTarget.y = 10.1f;

			m_vecParticleDesc[2].FixedTarget = m_vecParticleDesc[4].FixedTarget;

			pUtil->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[2]);
			pUtil->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[4]);


			pUtil->Start_InstanceParticle(m_eNowSceneNum, m_vecParticleDesc[4].FixedTarget, 2);
			pUtil->Start_InstanceParticle(m_eNowSceneNum, m_vecParticleDesc[4].FixedTarget, 2);

			CCircleTornado::CIRCLETORNADODESC tTornadoDesc;
			tTornadoDesc.vLook = _float3(0.00000001f, 1.f, 0);
			tTornadoDesc.vPosition = m_vecParticleDesc[4].FixedTarget;
			tTornadoDesc.vPosition.y -= 0.05f;
			tTornadoDesc.fSize = 2.5f;
			g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tTornadoDesc);
			tTornadoDesc.vPosition = tTornadoDesc.vPosition.XMVector() + XMVectorSet(0, 0.11f, 0, 0);
			tTornadoDesc.fSize = 1.8f;
			g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tTornadoDesc);

			CGameInstance* pInstance = g_pGameInstance;
			CBreakedGazebo::BREAKEDGAZBODESC tDesc;
			tDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);

			for (_uint i = 0; i < 10; i++)
			{
				tDesc.MeshKinds = rand() % 2 + Prototype_QBattleTowerParticleA;
				pInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_BreakedGazbo), &tDesc);
			}


			m_iAdjMovedIndex++;
		}

		break;


			break;

		case 34:
		{
			static _float3 StartPos;
			static _float3 StartLook;

			if (m_iAdjMovedIndex == 0 && PlayRate > 0.000001)
			{
				StartPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
				StartLook = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_LOOK);


				m_iAdjMovedIndex++;
			}
			if (m_iAdjMovedIndex)
			{
				CGameInstance* pInstance = g_pGameInstance;

				_float3 EasedPos = pInstance->Easing_Vector(TYPE_Linear, StartPos, m_PoleGrabPos, (_float)PlayRate, 1.f);
				_Vector EasedLook = pInstance->Easing_Vector(TYPE_Linear, StartLook, m_PoleGrabLook, (_float)PlayRate, 1.f).XMVector();

				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);
				m_pTransformCom->LookDir(EasedLook);

			}

		}
			break;

		case 36:
		{
			static _float3 TargetPos = _float3(63,10,75);

			if (PlayRate > 0.000001)
			{
				CGameInstance* pInstance = g_pGameInstance;

				_float3 EasedPos = pInstance->Easing_Vector(TYPE_Linear, m_PoleGrabPos, TargetPos, (_float)PlayRate, 1.f);

				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);

			}

		}
		break;

		case 37:
		{

			if (PlayRate < 0.01)
			{
				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(0,0,0));
				m_iAdjMovedIndex++;
			}
			else
			{
				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(63, 10, 75));

				
				m_iAdjMovedIndex++;

			}

		}
		break;
		default:
			break;
		}

	}


	m_iOldAnimIndex = iNowAnimIndex;
	return S_OK;
}

HRESULT CHandyGirl::Update_WanderAround_PatterDelay(_double fDeltatime, _float RandRange, _float TurningMixRate)
{
	_uint iWanderCount = 0;

	while (!(Check_Movable_Terrain(m_pTransformCom, m_vLookDir.XMVector(), 0.001f)))
	{

		if (!iWanderCount)
		{

			_Vector RandFloat = GetSingle(CUtilityMgr)->RandomFloat3(-RandRange, RandRange).XMVector();
			XMVectorSetY(RandFloat, 0);


			_Vector PlayerPos = m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS);
			XMVectorSetY(PlayerPos, 0);

			_Vector ObjPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
			XMVectorSetY(ObjPos, 0);

			m_vLookDir = XMVector3Normalize(PlayerPos + RandFloat - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
			iWanderCount++;
		}
		else
		{
			_float RandFloat = GetSingle(CUtilityMgr)->RandomFloat(-0.5, 0.5);
			_Vector NewLook = m_vLookDir.XMVector() * -1;
			//_Vector NewLook = pTransform->Get_MatrixState(CTransform::STATE_LOOK) * -1;
			if (RandFloat < 0)
			{
				_Vector Left = m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT)* -1;

				NewLook = XMVector3Normalize(NewLook* (1 + RandFloat) + (Left * -RandFloat));
			}
			else
			{
				_Vector vRight = m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT);

				NewLook = XMVector3Normalize(NewLook* (1 - RandFloat) + (vRight  * RandFloat));
			}

			m_vLookDir = NewLook;

		}



		if (iWanderCount > 5) break;
		iWanderCount++;
	}

	m_pTransformCom->LookDir(m_vLookDir.XMVector()*(TurningMixRate)+m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (1 - TurningMixRate));
	m_pTransformCom->MovetoDir(m_vLookDir.XMVector(), fDeltatime);



	return S_OK;
}

CHandyGirl * CHandyGirl::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CHandyGirl*	pInstance = new CHandyGirl(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CHandyGirl");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHandyGirl::Clone(void * pArg)
{
	CHandyGirl*	pInstance = new CHandyGirl(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CHandyGirl");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHandyGirl::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);

}
