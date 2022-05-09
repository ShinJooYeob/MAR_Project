#include "stdafx.h"
#include "..\public\HandyBoy.h"
#include "Terrain.h"



CHandyBoy::CHandyBoy(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonster(pDevice, pDeviceContext)
{
}

CHandyBoy::CHandyBoy(const CHandyBoy & rhs)
	: CMonster(rhs)
{
}

HRESULT CHandyBoy::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CHandyBoy::Initialize_Clone(void * pArg)
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

	m_PoleGrabPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	m_PoleGrabLook = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_LOOK);

	return S_OK;
}

_int CHandyBoy::Update(_double fDeltaTime)
{

	if (__super::Update(fDeltaTime) < 0)return -1;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);
	Update_DmgCalculate(fDeltaTime);


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

	if (NowIndex >= 24 && NowIndex <= 26)
	{

	}
	else
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
				m_pModel->Change_AnimIndex_UntilNReturn_Must(23, 23, 0, 0.15, true);

			}

		}
		else if (m_PatternDelayTime < 0 || !m_bIsPatternFinished)
		{
			Update_Pattern(fDeltaTime);
		}
		else
		{
			m_PatternDelayTime -= fDeltaTime;

			if (NowIndex == 2)
				FAILED_CHECK(Update_WanderAround_PatterDelay(fDeltaTime, 3.f, 0.05f));

			if (m_PatternDelayTime < 0)
			{
				m_bIsDealTime = (rand() % 3)?false:true;

				if (m_bIsDealTime)
				{
					m_bIsPatternFinished = false;
					m_PatternPassedTime = 0;
					m_PatternDelayTime = 7;
					m_pModel->Change_AnimIndex_UntilNReturn_Must(21, 22, 22, 0.15, true);
				}
				
			}

		}
	}



	//g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS))
	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS),8);


	if ((NowIndex == 24))
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
		for (_uint i = 0; i < 8; i++)
		{
			_Matrix			TransformMatrix = XMLoadFloat4x4(m_ArrCollisionAttach[i].pUpdatedNodeMat) * XMLoadFloat4x4(m_ArrCollisionAttach[i].pDefaultPivotMat);
			TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
			TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
			TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);
			m_pColliderCom->Update_Transform(i, TransformMatrix * m_pTransformCom->Get_WorldMatrix());
		}


		if (NowIndex != 25)
			g_pGameInstance->Add_CollisionGroup(CollisionType_Monster, this, m_pColliderCom);

		if(!m_bIsPatternFinished && !m_bIsDealTime)
			g_pGameInstance->Add_CollisionGroup(CollisionType_MonsterWeapon, this, m_pColliderCom);

	}


	//_Matrix			TransformMatrix = XMLoadFloat4x4(m_ArrCollisionAttach[7].pUpdatedNodeMat) * XMLoadFloat4x4(m_ArrCollisionAttach[7].pDefaultPivotMat);
	//TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	//TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	//TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);
	//_Vector PlayerPos = XMVector3TransformCoord(XMVectorSet(-1.474999f, 0.740000f, -2.504998f, 1), TransformMatrix * m_pTransformCom->Get_WorldMatrix());
	//m_pPlayerTransfrom->Set_MatrixState(CTransform::STATE_POS, PlayerPos + XMVectorSet(0, -1, 0, 0));


	return _int();
}

_int CHandyBoy::LateUpdate(_double fDeltaTime)
{

	FAILED_CHECK(__super::Set_Monster_On_Terrain(m_pTransformCom, fDeltaTime));

	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));


	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CHandyBoy::Render()
{
	if (__super::Render() < 0)
		return -1;


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

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 0, i, "g_BoneMatrices"));
	}

	return _int();
}

_int CHandyBoy::LateRender()
{
	return _int();
}

void CHandyBoy::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{

	switch (eConflictedObjCollisionType)
	{
	case Engine::CollisionType_Player:

		if (m_pModel->Get_NowAnimIndex() == 7)
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

_int CHandyBoy::Update_DmgCalculate(_double fDeltaTime)
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

_int CHandyBoy::Update_Pattern(_double fDeltaTime)
{

	m_PatternPassedTime += fDeltaTime;



	if (m_bIsPatternFinished)
	{
		m_ePattern += 1;
		if (m_ePattern > 3) m_ePattern = 0;
		m_PatternDelayTime = 0;
		m_bIsPatternFinished = false;
		m_PatternPassedTime = 0;
	}

	switch (m_ePattern)
	{
	case 0:

		if (!m_PatternPassedTime)
		{
			m_pModel->Change_AnimIndex_UntilNReturn_Must(4, 5, 0, 0.15, true);
		}
		break;


	case 1:

		if (!m_PatternPassedTime)
		{
			m_pModel->Change_AnimIndex_UntilNReturn_Must(6, 7, 0, 0.15, true);
		}

		break;

	case 2:

		if (!m_PatternPassedTime)
		{
			m_pModel->Change_AnimIndex_UntilNReturn_Must(11, 16, 0, 0.15, true);
		}

		break;

	case 3:

		if (!m_PatternPassedTime)
		{
			m_pModel->Change_AnimIndex_UntilNReturn_Must(17, 20, 0, 0.15, true);
		}

		break;

	default:
		break;
	}






	return _int();
}

void CHandyBoy::Add_Dmg_to_Monster(_float iDmgAmount)
{
	m_DmgPassedTime = MonsterDmgTime;
	m_fDmgAmount += iDmgAmount;
	m_fHP -= iDmgAmount;

	wstring ttDebugLog = L"HadyBoy HP: " + to_wstring(m_fHP) + L"\n";;
	OutputDebugStringW(ttDebugLog.c_str());
}

void CHandyBoy::Make_Hand_PoleGrab()
{
	m_PatternDelayTime = 0;
	m_bIsPatternFinished = false;
	m_PatternPassedTime = 0;
	m_bIsDealTime = true;

	m_pModel->Change_AnimIndex_UntilNReturn_Must(24, 25,25, 0.15, true);
}

void CHandyBoy::Make_Hand_Free()
{
	m_bIsDealTime = false;
	m_PatternDelayTime = 5;
	m_pTransformCom->Set_MoveSpeed(3.5f);
	m_pModel->Change_AnimIndex_ReturnTo_Must(26, 0, 0.15, true);
	m_fHP = m_fMaxHP;

}

void CHandyBoy::Enter_Hand()
{
	m_PatternDelayTime = 7;
	m_bIsPatternFinished = true;

	m_pModel->Change_AnimIndex_ReturnTo_Must(27, 0, 0, true);
}

HRESULT CHandyBoy::SetUp_Components()
{
	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = 3.5f;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_HandyBoy), TAG_COM(Com_Model), (CComponent**)&m_pModel));

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


	//Pivot  : -1.084999f , -0.120000f , -1.015000f , 1
	//size  : 1.539999f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(1.539999f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-1.084999f, -0.120000f, -1.015000f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[3] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-R-Hand");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[3].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer(1);



	//Pivot  : -0.255000f , -0.230000f , -0.895000f , 1
	//size  : 1.579999f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(1.579999f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-0.255000f, -0.230000f, -0.895000f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[4] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-R-Toe0");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[4].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer(1);


	//Pivot  : -0.075000f , -0.020000f , -3.974997f , 1
	//size  : 3.279998f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(1.719999f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.405000f, -0.150000f, -0.865000f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[5] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-L-Foot");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[5].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer(1);


	//Pivot  : 0.904999f , -0.130000f , -1.374999f , 1
	//size  : 1.470000f , 1.000000f , 1.000000f  

	ColliderDesc.vScale = _float3(1.470000f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.904999f, -0.130000f, -1.374999f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[6] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-L-Forearm");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[6].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer(1);


	//Pivot  : -1.474999f , 0.740000f , -2.504998f , 1
	//size  : 2.219999f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(2.219999f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-1.474999f, 0.740000f, -2.504998f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[7] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bone_thumb_03");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[7].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer(1);



	return S_OK;
}

HRESULT CHandyBoy::Adjust_MovedTransform_byAnim(_double fDeltatime)
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
				m_PatternDelayTime = 3;
				m_bIsPatternFinished = true;
				m_pModel->Change_AnimIndex_UntilTo(1, 2, 0.15, true);
			}

			break;
		case 6:

			_Vector RandFloat = GetSingle(CUtilityMgr)->RandomFloat3(-0.3f, 0.3f).XMVector();
			XMVectorSetY(RandFloat, 0);
			_Vector PlayerPos = m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS);
			XMVectorSetY(PlayerPos, 0);
			_Vector ObjPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
			XMVectorSetY(ObjPos, 0);
			m_vLookDir = XMVector3Normalize(PlayerPos + RandFloat - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
			m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f)+m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));

			break;

		case 7:
			if (PlayRate > 0.01 && PlayRate < 0.90)
			{
				if (m_bIsGrabed)
				{
					m_pModel->Change_AnimIndex_UntilNReturn_Must(8, 10, 0, 0.15, true);
					m_pTransformCom->Set_MoveSpeed(3.5f);
				}

				else
				{
					m_pTransformCom->Set_MoveSpeed(15.5f);
					FAILED_CHECK(Update_WanderAround_PatterDelay(fDeltatime, 1.f, 0.15f));
				}


			}
			else if (!m_bIsGrabed && PlayRate > 0.90)
			{
				m_pTransformCom->Set_MoveSpeed(3.5f);
				m_pModel->Change_AnimIndex(0, 0.15, true);

			}
			break;

		case 8:
		case 9:
		{

			_Matrix			TransformMatrix = XMLoadFloat4x4(m_ArrCollisionAttach[7].pUpdatedNodeMat) * XMLoadFloat4x4(m_ArrCollisionAttach[7].pDefaultPivotMat);
			TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
			TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
			TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);
			_Vector PlayerPos = XMVector3TransformCoord(XMVectorSet(-1.474999f, 0.740000f, -2.504998f, 1), TransformMatrix * m_pTransformCom->Get_WorldMatrix());
			m_pPlayerTransfrom->Set_MatrixState(CTransform::STATE_POS, PlayerPos + XMVectorSet(0, -1, 0, 0));

		}
			break;


		case 10:

			if (m_bIsGrabed  )
			{
				if (PlayRate < 0.346938)
				{
					_Matrix			TransformMatrix = XMLoadFloat4x4(m_ArrCollisionAttach[7].pUpdatedNodeMat) * XMLoadFloat4x4(m_ArrCollisionAttach[7].pDefaultPivotMat);
					TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
					TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
					TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);
					_Vector PlayerPos = XMVector3TransformCoord(XMVectorSet(-1.474999f, 0.740000f, -2.504998f, 1), TransformMatrix * m_pTransformCom->Get_WorldMatrix());
					m_pPlayerTransfrom->Set_MatrixState(CTransform::STATE_POS, PlayerPos + XMVectorSet(0, -1, 0, 0));
				}
				else
				{
					if (m_bIsGrabed)
					{
						m_bIsGrabed = false;
						m_pPlayer->Set_IsAttached(false);
					}


				}
			}



			break;

		case 11:

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


	
		case 12:
			if (PlayRate > 0.01 && PlayRate < 0.875)
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

		case 14:
		case 15:
			if (PlayRate > 0.01 && PlayRate < 0.95)
			{
				m_pTransformCom->Set_MoveSpeed(20.5f);
				FAILED_CHECK(Update_WanderAround_PatterDelay(fDeltatime, 1.f, 0.15f));
			}
			break;
		case 16:
			m_pTransformCom->Set_MoveSpeed(3.5f);
			break;



		case 17:
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

		case 18:
			if (PlayRate > 0.01)
			{


				m_pTransformCom->MovetoDir_bySpeed(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), -7.f, fDeltatime);

				CTerrain* pTerrain = (CTerrain*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));
				_bool bIsOn = false;
				_uint eNowTile = Tile_End;
				_float3 CaculatedPos = pTerrain->PutOnTerrain(&bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector(), nullptr, &eNowTile);


				if (eNowTile == Tile_None || bIsOn)
					m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);

			}
			break;


		case 24:
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

				_float3 EasedPos = pInstance->Easing_Vector(TYPE_Linear, StartPos, m_PoleGrabPos ,(_float)PlayRate, 1.f);
				_Vector EasedLook = pInstance->Easing_Vector(TYPE_Linear, StartLook, m_PoleGrabLook, (_float)PlayRate, 1.f).XMVector();

				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);
				m_pTransformCom->LookDir(EasedLook);

			}

		}
			break;


		default:
			break;
		}

	}


	return S_OK;
}

HRESULT CHandyBoy::Update_WanderAround_PatterDelay(_double fDeltatime, _float RandRange, _float TurningMixRate)
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

			m_vLookDir = XMVector3Normalize(PlayerPos + RandFloat- m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
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

CHandyBoy * CHandyBoy::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CHandyBoy*	pInstance = new CHandyBoy(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CHandyBoy");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHandyBoy::Clone(void * pArg)
{
	CHandyBoy*	pInstance = new CHandyBoy(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CHandyBoy");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHandyBoy::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);

}
