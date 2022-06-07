#include "stdafx.h"
#include "..\public\DollMaker.h"
#include "Scythe.h"
#include "Terrain.h"
#include "HandyBoy.h"
#include "HandyGirl.h"
#include "DollMakerBullet.h"
#include "HyperVoice.h"
#include "StageBoss_SpwanBoss.h"
#include "Scene_Boss.h"



CDollMaker::CDollMaker(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonster(pDevice,pDeviceContext)
{
}

CDollMaker::CDollMaker(const CDollMaker & rhs)
	: CMonster(rhs)
{
}

HRESULT CDollMaker::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CDollMaker::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());


	if (pArg != nullptr)
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, * ((_float3*)pArg));

	m_fHP = m_fMaxHP = 56;


	FAILED_CHECK(SetUp_Weapon());

	ZeroMemory(m_bIsDmgAnimUpdated, sizeof(_bool) * 3);
	m_pTransformCom->LookAt(XMVectorSet(60, 10, 60,0));
	m_bIsPatternFinished = true;
	return S_OK;
}

_int CDollMaker::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)return -1;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);
	Update_DmgCalculate(fDeltaTime);


	if (g_pGameInstance->Get_DIKeyState(DIK_3) & DIS_Down)
		Add_Dmg_to_Monster(10);


	if (m_bDeadAnimStart)
	{
		CGameInstance* pInstance = g_pGameInstance;
		m_DeadPassedTime += fDeltaTime;

		if (m_DeadPassedTime < 6.5)
		{
			_float3 EasedPos;
			if (m_DeadPassedTime < 3.25f)
			{
				EasedPos = pInstance->Easing_Vector( TYPE_Linear, _float3(85, 10, 78), _float3(92.5f, 10.f, 74.f), (_float)m_DeadPassedTime, 3.25f);
			}
			else
			{
				EasedPos = pInstance->Easing_Vector(TYPE_Linear, _float3(92.5f, 10.f, 74.f), _float3(91, 10, 82), (_float)m_DeadPassedTime - 3.25f, 3.25f);
			}
			_float3 EasedAt = pInstance->Easing_Return_Vector(TYPE_Linear, TYPE_Linear, m_vDeadStartLookAt, _float3(98.5f, 10.f, 58.5f), (_float)m_DeadPassedTime, 6.5f);

			m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);
			m_pTransformCom->LookAt(EasedAt.XMVector());

			if (m_DeadPassedTime < 1) // 올라
			{
				if (!m_iDeadAnimChecker)
				{
					GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.5f, _float4(0.8f));
					m_iDeadAnimChecker++;
				}
				else
				{
					GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.1f, _float4(0.1f));

				}

				_float EasedHeight = pInstance->Easing(TYPE_SinInOut, 10 , 15 , (_float)m_DeadPassedTime, 1.f);
				EasedPos.y = EasedHeight;
				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);


			}
			else if (m_DeadPassedTime < 3.25f)// 반쯤 잠기게 내려가
			{
				if (m_DeadPassedTime < 3.1)
				{
					GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.1f, _float4(0.1f));
				}

				_float EasedHeight = pInstance->Easing(TYPE_SinInOut, 15, 5, (_float)m_DeadPassedTime - 1, 2.25f);
				EasedPos.y = EasedHeight;
				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);
			}
			else if (m_DeadPassedTime < 4.25f)//다보이게 올라가
			{
				if (m_iDeadAnimChecker == 1)
				{
					GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.5f, _float4(0.8f));
					m_iDeadAnimChecker++;
				}
				else
				{
					GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.1f, _float4(0.1f));

				}

				_float EasedHeight = pInstance->Easing(TYPE_SinInOut, 5, 15, (_float)m_DeadPassedTime - 3.25f, 1.f);
				EasedPos.y = EasedHeight;
				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);
			}
			else if (m_DeadPassedTime < 6.5f) // 완전히 내려가
			{
				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.1f, _float4(0.1f));


				_float EasedHeight = pInstance->Easing(TYPE_SinInOut, 15, 7, (_float)m_DeadPassedTime - 4.25f, 2.25f);
				EasedPos.y = EasedHeight;
				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);
			}



			m_bIsOnScreen = true;
			FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime, m_bIsOnScreen));
			FAILED_CHECK(Adjust_AnimMovedTransform(fDeltaTime));

		}
		else if (m_DeadPassedTime < 7.5)
		{
			GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.1f, _float4(0.1f *  (7.5f- (_float)m_DeadPassedTime)));


			if (m_iDeadAnimChecker == 2 && m_DeadPassedTime > 7.2f)
			{
				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeOut, 0.25f, _float4(1, 1, 1, 1));
				m_iDeadAnimChecker++;
			}

			_float3 EasedPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);

			_float EasedHeight = pInstance->Easing(TYPE_SinInOut, 7, 0, (_float)m_DeadPassedTime - 6.5f, 1.f);
			EasedPos.y = EasedHeight;
			m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);
		}
		else
		{
			Set_IsDead();
			g_pGameInstance->Get_NowScene()->Set_SceneChanging(SCENE_LOBY);
		}

	}
	else
	{


		{
			_Vector TargetAt = m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS);

			TargetAt = XMVectorSetY(TargetAt, m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).y);

			m_pTransformCom->LookAt(TargetAt);
		}



		if (m_PatternDelayTime > 0)
		{
			m_PatternDelayTime -= fDeltaTime;

			if (m_fHP < m_fMaxHP * 0.8f && m_pHanddyIndex == 0)
			{
				m_PatternDelayTime = 4;
				if (!m_bSummonHandy)
				{
					m_bSummonHandy = true;
					m_pModel->Change_AnimIndex_UntilNReturn_Must(9, 12, 0, 0.15, true);
				}
			}

			if (m_fHP < m_fMaxHP * 0.6f && m_pHanddyIndex == 1)
			{
				m_PatternDelayTime = 4;
				if (!m_bSummonHandy)
				{
					m_bSummonHandy = true;
					m_pModel->Change_AnimIndex_UntilNReturn_Must(14, 17, 0, 0.15, true);
				}
			}
		}
		else
		{
			_uint iChecker = 1;

			for (_uint i = 0; i < m_pHanddyIndex; i++)
				iChecker *= m_vecWeapon[i]->Object_Function();

			if (iChecker || !m_bIsPatternFinished)
				FAILED_CHECK(Update_Pattern(fDeltaTime));
		}


		m_bIsOnScreen = true;
		FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime, m_bIsOnScreen));
		FAILED_CHECK(Adjust_AnimMovedTransform(fDeltaTime));

		for (_uint i = 0; i < 7; i++)
		{
			_Matrix			TransformMatrix = XMLoadFloat4x4(m_ArrCollisionAttach[i].pUpdatedNodeMat) * XMLoadFloat4x4(m_ArrCollisionAttach[i].pDefaultPivotMat);
			TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
			TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
			TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);
			m_pColliderCom->Update_Transform(i, TransformMatrix * m_pTransformCom->Get_WorldMatrix());
		}

		_uint NowIndex = m_pModel->Get_NowAnimIndex();

		if ((NowIndex >= 1 && NowIndex <= 3) || (NowIndex >= 5 && NowIndex <= 6))
		{
			g_pGameInstance->Add_CollisionGroup(CollisionType_Monster, this, m_pColliderCom);

#ifdef _DEBUG
			FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
		}

	}
	if (!m_bDeadAnimStart)
	{
		for (_uint i = 0; i < m_pHanddyIndex; i++)
			m_vecWeapon[i]->Update(fDeltaTime);
	}

	return _int();
}

_int CDollMaker::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)return -1;


	if (m_bIsOnScreen)
	{
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

		if (!m_bDeadAnimStart)
		{
			for (_uint i = 0; i < m_pHanddyIndex; i++)
				m_vecWeapon[i]->LateUpdate(fDeltaTime);
		}
	}

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CDollMaker::Render()
{
	if (__super::Render() < 0)		return -1;


	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	CGameInstance* pInstance = GetSingle(CGameInstance);

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));


	_uint NumMaterial = m_pModel->Get_NumMaterial();




	for (_uint i = 0; i < NumMaterial; i++)
	{
		if (m_pHanddyIndex > 0 && i == 3)continue;
		if (m_pHanddyIndex > 1 && i == 4)continue;

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 1, i, "g_BoneMatrices"));
	}

	return _int();
	return _int();
}

_int CDollMaker::LightRender()
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

_int CDollMaker::Update_DmgCalculate(_double fDeltaTime)
{
	if (m_bDeadAnimStart) return 0;

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


	_uint NowAnimIndex = m_pModel->Get_NowAnimIndex();


	if (!m_bIsDmgAnimUpdated[0] && m_fDmgAmount > 3 && NowAnimIndex >= 1 && NowAnimIndex <=3 )
	{
		GetSingle(CUtilityMgr)->Set_MonsterBlurOn(false);

		m_pModel->Change_AnimIndex_ReturnTo_Must(19, 4, 0.15, true);
		m_bIsDmgAnimUpdated[0] = true;
	}
	if (!m_bIsDmgAnimUpdated[0] && m_fDmgAmount > 3 && NowAnimIndex >= 5 && NowAnimIndex <= 6)
	{
		GetSingle(CUtilityMgr)->Set_MonsterBlurOn(false);

		m_pModel->Change_AnimIndex_ReturnTo_Must(20, 8, 0.15, true);
		m_bIsDmgAnimUpdated[0] = true;
	}



	return 0;
}

_int CDollMaker::Update_Pattern(_double fDeltaTime)
{

	m_PatternPassedTime += fDeltaTime;

	if (m_bIsPatternFinished)
	{
		m_ePattern += 1;
		if (m_ePattern > 1) m_ePattern = 0;
		m_bIsPatternFinished = false;
		m_PatternPassedTime = 0;
	}




	switch (m_ePattern)
	{
	case 0:
		if (!m_PatternPassedTime)		
			m_pModel->Change_AnimIndex_UntilNReturn_Must(1, 4, 0, 0.15, true);

		break;
	case 1:
		if (!m_PatternPassedTime)
			m_pModel->Change_AnimIndex_UntilNReturn_Must(5, 8, 0, 0.15, true);
		break;

	default:
		break;
	}


	


	return _int();
}

void CDollMaker::Add_Dmg_to_Monster(_float iDmgAmount)
{
	if (m_fHP <= 0) return;

	m_DmgPassedTime = MonsterDmgTime * 5.f;
	m_fDmgAmount += iDmgAmount;
	m_fHP -= iDmgAmount;


	if (m_fHP <= 0 && !m_bDeadAnimStart)
	{
		m_vDeadStartLookAt = m_pPlayerTransfrom->Get_MatrixState_Float3(CTransform::STATE_POS);
		m_vDeadStartLookAt.y = 10;


		 CStageBoss_SpwanBoss* pTrigger = (CStageBoss_SpwanBoss*)g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_TriggerCollider), 0);

		 NULL_CHECK_BREAK(pTrigger);

		 pTrigger->PlayCamAction();

		m_bDeadAnimStart = true;
		m_iDeadAnimChecker = 0;
		m_DeadPassedTime = 0;
		m_pModel->Change_AnimIndex(21, 0.15f, true);
	}
}

HRESULT CDollMaker::SetUp_Components()
{

	CTransform::TRANSFORMDESC tDesc = {};
	tDesc.fMovePerSec = 2.8f;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));
	__super::SetUp_WanderLook(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK));


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_DollMaker), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	FAILED_CHECK(m_pModel->Change_AnimIndex(0));




	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	///* For.Com_AABB */
	//ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));


	//Pivot  : 0.030000f , -10.630148f , -10.410143f , 1
	//size  : 6.080047f , 3.000000f , 3.000000f  
	ColliderDesc.vScale = _float3(6.080047f, 3.000000f, 3.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.030000f, -10.630148f, -10.410143f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[0] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bone_Tough_Long_04");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[0].pDefaultPivotMat, E_FAIL);



	ColliderDesc.vScale = _float3(1.3f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.030000f, -10.630148f, -10.410143f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[1] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bone_Tough_Long_04");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[1].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer();


	//Pivot  :
	//size  : 1.000000f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(1.3f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-0.070000f, -9.190115f, -10.170137f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[2] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bone_Tough_Long_02");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[2].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer();


	//Pivot  : -0.050000f , -9.880131f , -10.230139f , 1
	//size  : 0.910000f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(1.3f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-0.050000f, -9.880131f, -10.230139f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[3] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bone_Tough_Long_03");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[3].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer();



	//Pivot  : 0.030000f , -11.570169f , -10.410143f , 1
	//size  : 1.000000f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(1.3f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);	
	ColliderDesc.vPosition = _float4(0.030000f, -11.570169f, -10.410143f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[4] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bone_Tough_Long_05");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[4].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer();


	//Pivot  : -0.010000f , -12.640194f , -10.480145f , 1
	//size  : 1.000000f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(1.3f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-0.010000f, -12.640194f, -10.480145f, 1);

	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[5] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bone_Tough_Long_06");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[5].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer();


	//Pivot  : -0.010000f , -13.590216f , -10.570147f , 1
	//size  : 1.000000f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(1.3f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-0.010000f, -13.590216f, -10.570147f, 1);

	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[6] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bone_Tough_Long_07");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[6].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer();





	////Pivot  : 0.000000f , 0.310000f , -2.019999f , 1
	////size  : 2.039999f , 0.450001f , 1.180000f  
	////(x,z,y)
	////size  : 4.910018f , 1.699999f , 3.499998f  

	//ColliderDesc.vScale = _float3(3.499998f, 2.699999f, 4.910018f);
	//ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);	
	//ColliderDesc.vPosition = _float4(-0.160000f, 0.000000f, -4.440007f, 1);
	//FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_OBB, &ColliderDesc));
	//m_pColliderCom->Set_ParantBuffer();




	return S_OK;
}

HRESULT CDollMaker::SetUp_Weapon()
{

	CGameInstance* pInstance = GetSingle(CGameInstance);

	NULL_CHECK_RETURN(m_pModel, E_FAIL);
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);



	CMonsterWeapon* pWeapon = nullptr;
	FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)&pWeapon, m_eNowSceneNum, TAG_OP(Prototype_HandyBoy), &_float3(71, 10, 60)));
	NULL_CHECK_RETURN(pWeapon, E_FAIL);
	m_vecWeapon.push_back(pWeapon);

	pWeapon = nullptr;
	FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)&pWeapon, m_eNowSceneNum, TAG_OP(Prototype_HandyGirl), &_float3(63, 10, 75)));
	NULL_CHECK_RETURN(pWeapon, E_FAIL);
	m_vecWeapon.push_back(pWeapon);



	return S_OK;
}

HRESULT CDollMaker::Set_Monster_On_Terrain(CTransform * pTransform, _double fDeltaTime)
{
	CGameInstance* pInstance = GetSingle(CGameInstance);

	m_LevitationTime += fDeltaTime;
	_float fGravity = _float((m_LevitationTime) * (m_LevitationTime)* -29.4f);

	pTransform->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);


	CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

	_bool bIsOn = false;
	_uint eTileKinds = Tile_End;

	_float3 CaculatedPos = pTerrain->PutOnTerrain(&bIsOn, pTransform->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector(),nullptr,&eTileKinds);

	if (bIsOn)
	{
		m_LevitationTime = 0;
		pTransform->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);

		if (m_bIsJumping && m_pModel->Get_NowAnimIndex() != 4 && m_pModel->Get_NowAnimIndex() != 5)
		{
			m_bIsJumping = false;
			m_PatternDelayTime = 3;
			m_pModel->Change_AnimIndex_ReturnTo_Must(7, 0, 0, true);
		}
	}

	return S_OK;
}

HRESULT CDollMaker::Adjust_AnimMovedTransform(_double fDeltatime)
{
	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	_double PlayRate = m_pModel->Get_PlayRate();

	if (iNowAnimIndex != m_iOldAnimIndex || PlayRate > 0.95)
		m_iAdjMovedIndex = 0;


	if (PlayRate <= 0.95)
	{
		switch (iNowAnimIndex)
		{
		case 0:
			break;
	

		case 2:
		case 3:
		{
			static _double AmilazeTimer = 0;

			if (PlayRate < 0.0001f)
			{
				AmilazeTimer = 0;
			}
			else
			{
				AmilazeTimer -= fDeltatime;
				if (AmilazeTimer < 0)
				{
					CDollMakerBullet::HANDYGRILBULLETDESC tDesc;

					tDesc.vPosition = m_pColliderCom->Get_ColliderPosition(2).XMVector();

					_Vector TargetAt = m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS);

					tDesc.vMoveDir = XMVector3Normalize(XMVectorSetY(TargetAt, 0) - XMVectorSetY(tDesc.vPosition.XMVector(), 0) );



					g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DollMakerBullet), &tDesc);

					AmilazeTimer = GetSingle(CUtilityMgr)->RandomFloat(0.64f, 1.f);
				}

			}
		}
			break;

		case 6:

			if (m_iAdjMovedIndex == 0 && PlayRate > 0.45)
			{
				GetSingle(CUtilityMgr)->Set_MonsterBlurOn(true);
				m_iAdjMovedIndex++;
			}

			GetSingle(CUtilityMgr)->Set_MonsterBlurPos(m_pColliderCom->Get_ColliderPosition(2));




			break;

		case 7:
		{
			GetSingle(CUtilityMgr)->Set_MonsterBlurPos(m_pColliderCom->Get_ColliderPosition(2));


			if (m_iAdjMovedIndex == 0 && PlayRate > 0.3)
			{

				CHyperVoice::CIRCLETORNADODESC tDesc;

				tDesc.fSize = 2.5f;
				tDesc.Power = 25.f;
				tDesc.vPosition = m_pColliderCom->Get_ColliderPosition(2);

				_Vector TargetAt = m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS);
				tDesc.vLook = XMVector3Normalize((TargetAt) - (tDesc.vPosition.XMVector()));




				FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_HyperVoice), &tDesc));


				GetSingle(CUtilityMgr)->Set_MonsterBlurOn(false);
				m_iAdjMovedIndex++;
			}
		}
			break;

		case 10:
			if (PlayRate > 0.85 && m_pHanddyIndex == 0)
			{
				m_pHanddyIndex = 1;
				((CHandyBoy*)(m_vecWeapon[0]))->Enter_Hand();
				m_bSummonHandy = false;
			}
			break;

		case 15:
			if (PlayRate > 0.85 && m_pHanddyIndex == 1)
			{
				m_pHanddyIndex = 2;
				((CHandyGirl*)(m_vecWeapon[1]))->Enter_Hand();
				m_bSummonHandy = false;

			}
			break;

		default:
			break;
		}
	}
	else
	{
		switch (iNowAnimIndex)
		{
		case 4:
			m_pModel->Change_AnimIndex(0, 0.15f, true);
			m_PatternDelayTime = 4;
			m_bIsPatternFinished = true;

			if (m_pHanddyIndex > 0) ((CHandyBoy*)m_vecWeapon[0])->Make_Hand_Free();
			if (m_pHanddyIndex > 1) ((CHandyGirl*)m_vecWeapon[1])->Make_Hand_Free();

			break;

		case 8:
			m_pModel->Change_AnimIndex(0, 0.15f, true);
			m_PatternDelayTime = 4;
			m_bIsPatternFinished = true;

			if (m_pHanddyIndex > 0) ((CHandyBoy*)m_vecWeapon[0])->Make_Hand_Free();
			if (m_pHanddyIndex > 1) ((CHandyGirl*)m_vecWeapon[1])->Make_Hand_Free();

			break;

		default:
			break;
		}

	}





	m_iOldAnimIndex = iNowAnimIndex;
	return S_OK;
}

CDollMaker * CDollMaker::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CDollMaker*	pInstance = new CDollMaker(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CDollMaker");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDollMaker::Clone(void * pArg)
{
	CDollMaker*	pInstance = new CDollMaker(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CDollMaker");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDollMaker::Free()
{
	__super::Free();


	for (auto& pWeapon : m_vecWeapon)
		Safe_Release(pWeapon);
	

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);

}
