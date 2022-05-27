#include "stdafx.h"
#include "..\public\WaspInk.h"



CWaspInk::CWaspInk(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonster(pDevice,pDeviceContext)
{
}

CWaspInk::CWaspInk(const CWaspInk & rhs)
	: CMonster(rhs)
{
}

HRESULT CWaspInk::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));





	return S_OK;
}

HRESULT CWaspInk::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
	{
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, *((_float3*)pArg));
		_float Tempw = (*((_float4*)pArg)).w;
		m_SpwanKinds = (_uint)Tempw;
	}


	ZeroMemory(m_bIsDmgAnimUpdated, sizeof(_bool) * 3);
	m_fHP = m_fMaxHP = 32;
	m_pTransformCom->Set_MoveSpeed(4.8f);


	ZeroMemory(m_SpwanMovingPos, sizeof(_float3) * 5);

	
	switch (m_SpwanKinds)
	{
	case 0:
		m_SpwanMovingPos[4] = _float3(225.749f, 20.f, 65.196f);
		break;
	case 1:
		m_SpwanMovingPos[4] = _float3(211.348f, 20.f, 66.948f);
		break;
	case 2:
		m_SpwanMovingPos[4] = _float3(223.403f, 19.f, 46.583f);
		break;
	case 3:
		m_SpwanMovingPos[4] = _float3(213.09f, 20.f, 41.65f);
		break;
	default:
		break;
	}


	CUtilityMgr* pUtil = GetSingle(CUtilityMgr);

	m_SpwanMovingPos[0] = XMVectorSet(253.859f, 37.0f, 41.835f,0) + pUtil->RandomFloat3(-3,3).XMVector();
	m_SpwanMovingPos[1]= XMVectorSet(208.f, 26.f, 41.835f,0) + pUtil->RandomFloat3(-3,3).XMVector();
	m_SpwanMovingPos[2]= XMVectorSet(214.859f, 35.3f, 82.935f,0) + pUtil->RandomFloat3(-3,3).XMVector();
	m_SpwanMovingPos[3]= XMVectorSet(220.017f, 71.0f, 56.0f,0) + pUtil->RandomFloat3(-3,3).XMVector();

	m_SpwanAnimPassedTime = 0;


	/*
	253.859f,37.0f,41.835f

	208.f,26.f,41.835f

	214.859f,35.3f,82.935f

	220.017f,71.0f,56.0f

	//
	225.749f,20.f,65.196f
	211.348f,20.f,66.948f
	223.403f,19.f,46.583f
	213.09f, 20.f,41.65f
	//
	*/


	return S_OK;
}

_int CWaspInk::Update(_double fDeltaTime)
{


	if (__super::Update(fDeltaTime) < 0)return -1;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);


	if (g_pGameInstance->Get_DIKeyState(DIK_O)&DIS_Down)
		Add_Dmg_to_Monster(1000);


	//m_pTransformCom->Set_MoveSpeed(4.8f);
	if (m_bDeadAnimStart)
	{
		if (m_pModel->Get_PlayRate() > 0.5)
		{
			Set_IsDead();
			return 0;
		}
	}
	else if (m_bSpwanAnimFinished)
	{


		if (!m_bIsPatternFinished || Distance_BetweenPlayer(m_pTransformCom) < 15)
		{
			Update_Pattern(fDeltaTime);
		}
		else
		{
			if (!m_pModel->Get_IsUntillPlay() && !m_pModel->Get_IsHavetoBlockAnimChange())
			{
				m_pModel->Change_AnimIndex(2);
				FAILED_CHECK(__super::Update_WanderAround(m_pTransformCom, fDeltaTime, 0.2f));
			}
		}
	}
	else
	{
		m_SpwanAnimPassedTime += fDeltaTime;


		if (m_SpwanAnimPassedTime < 2)
		{
			_float3 EasedPos = g_pGameInstance->Easing_Vector(TYPE_ExpoInOut, m_SpwanMovingPos[0], m_SpwanMovingPos[1], (_float)m_SpwanAnimPassedTime, 2);
			m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);

			_Vector NewLook = XMVectorLerp(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK),
				XMVector3Normalize(m_SpwanMovingPos[1].XMVector() - m_SpwanMovingPos[0].XMVector()), 0.1f);

			m_pTransformCom->LookDir(NewLook);
		}
		else if (m_SpwanAnimPassedTime < 4)
		{

			_float3 EasedPos = g_pGameInstance->Easing_Vector(TYPE_ExpoInOut, m_SpwanMovingPos[1], m_SpwanMovingPos[2], (_float)m_SpwanAnimPassedTime - 2, 2);
			m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);

			_Vector NewLook = XMVectorLerp(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK),
				XMVector3Normalize(m_SpwanMovingPos[2].XMVector() - m_SpwanMovingPos[1].XMVector()), 0.1f);

			m_pTransformCom->LookDir(NewLook);

		}
		else if (m_SpwanAnimPassedTime < 6)
		{

			_float3 EasedPos = g_pGameInstance->Easing_Vector(TYPE_ExpoInOut, m_SpwanMovingPos[2], m_SpwanMovingPos[3], (_float)m_SpwanAnimPassedTime - 4, 2);
			m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);
			_Vector NewLook = XMVectorLerp(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK),
				XMVector3Normalize(m_SpwanMovingPos[3].XMVector() - m_SpwanMovingPos[2].XMVector()), 0.1f);

			m_pTransformCom->LookDir(NewLook);
		}
		else if (m_SpwanAnimPassedTime < 10)
		{
			_float3 EasedPos = g_pGameInstance->Easing_Vector(TYPE_ExpoIn, m_SpwanMovingPos[3], m_SpwanMovingPos[4], (_float)m_SpwanAnimPassedTime - 6, 4);
			m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);


			_Vector NewLook = XMVectorLerp(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK),
				XMVector3Normalize(m_SpwanMovingPos[4].XMVector() - m_SpwanMovingPos[3].XMVector()), 0.1f);

			m_pTransformCom->LookDir(NewLook);
		}
		else if(m_SpwanAnimPassedTime < 13)
		{
			if (!m_SpwanAnimCount)
			{
				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_SpwanMovingPos[4]);
				_float3 NewLook = GetSingle(CUtilityMgr)->RandomFloat3(-9999.f, 9999.f);
				NewLook.y = 0;
				m_pTransformCom->LookDir(NewLook.XMVector());
				__super::SetUp_WanderLook(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK));
				m_pModel->Change_AnimIndex(10);
				m_SpwanAnimCount++;
			}
		}
		else
		{
			m_bSpwanAnimFinished = true;

		}
	}


	Update_DmgCalculate(fDeltaTime);

	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));

	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime, m_bIsOnScreen));

	if (m_bIsOnScreen)
	{
		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());
	}

	if (m_bIsPatternFinished)
		g_pGameInstance->Add_CollisionGroup(CollisionType_Monster, this, m_pColliderCom);
	else
		g_pGameInstance->Add_CollisionGroup(CollisionType_MonsterWeapon, this, m_pColliderCom);
#ifdef _DEBUG
	FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG


	return _int();
}

_int CWaspInk::LateUpdate(_double fDeltaTime)
{
	if (m_bSpwanAnimFinished)
		FAILED_CHECK(__super::Set_Monster_On_Terrain(m_pTransformCom, fDeltaTime));


	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));


	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CWaspInk::Render()
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

		if (i % 2)
		{
			FAILED_CHECK(m_pModel->Render(m_pShaderCom, 4, i, "g_BoneMatrices"));
		}
		else
		{
			FAILED_CHECK(m_pModel->Render(m_pShaderCom, 0, i, "g_BoneMatrices"));

		}
	}
	return _int();
}

_int CWaspInk::LightRender()
{
	return _int();
}

_int CWaspInk::Update_DmgCalculate(_double fDeltaTime)
{
	//if (m_DmgPassedTime <= 0)
	//{
	//	if (m_fDmgAmount > 0)
	//	{
	//		ZeroMemory(m_bIsDmgAnimUpdated, sizeof(_bool) * 3);
	//		m_fDmgAmount = 0;
	//		m_DmgPassedTime = 0;
	//	}
	//	return 0;
	//}
	//m_DmgPassedTime -= fDeltaTime;

	if (m_fHP <= 0)
	{
		if (!m_bDeadAnimStart)
		{
			m_bDeadAnimStart = true;
			m_pModel->Change_AnimIndex_ReturnTo_Must(11, 11, 0.15, true);
		}
		return 0;
	}

	if (!m_bIsDmgAnimUpdated[0])
	{
		m_pModel->Change_AnimIndex_ReturnTo_Must(8 + rand()%2, 0, 0.15, true);
		m_bIsPatternFinished = true;
		m_PatternPassedTime = 0;
		m_pTransformCom->Set_MoveSpeed(4.8f);
		m_bIsDmgAnimUpdated[0] = true;

		m_PatternPassedTime = 0;
		m_bIsDmgAnimUpdated[2] = false;
	}

	return _int();
}

_int CWaspInk::Update_Pattern(_double fDeltaTime)
{
	m_PatternPassedTime += fDeltaTime;

	if (m_PatternPassedTime > 2.5 &&(!m_bIsPatternFinished || Distance_BetweenPlayer(m_pTransformCom) < 10))
	{
		if (m_bIsPatternFinished)
		{
			m_bIsPatternFinished = false;
			m_pModel->Change_AnimIndex_ReturnTo(7,2, 0.15, true);
			m_bIsDmgAnimUpdated[2] = false;
		}


		if (!m_bIsDmgAnimUpdated[2] && m_pModel->Get_PlayRate() > 0.43)
		{
			_Vector NewLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) 
				- m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));

			Add_Force(m_pTransformCom, NewLookDir, 80);
			m_bIsDmgAnimUpdated[2] = true;
		}

		if (!m_pModel->Get_IsHavetoBlockAnimChange())
		{
			m_bIsPatternFinished = true;
			m_bIsDmgAnimUpdated[2] = false;
			m_PatternPassedTime = 0;
		}
	}
	else
	{
		//if (!m_pModel->Get_IsHavetoBlockAnimChange())
			NearPatternWander(fDeltaTime);
	}
	return _int();
}

void CWaspInk::Add_Dmg_to_Monster(_float iDmgAmount)
{
	if (m_bIsPatternFinished)
	{
		m_DmgPassedTime = MonsterDmgTime;
		//m_fDmgAmount += iDmgAmount;
		m_fHP -= iDmgAmount;

		ZeroMemory(m_bIsDmgAnimUpdated, sizeof(_bool) * 2);
		m_fDmgAmount = 0;
		m_DmgPassedTime = 0;
	}
}

void CWaspInk::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		if (!m_bIsPatternFinished)
		{
			if (!lstrcmp(pConflictedObj->Get_NameTag(), L"Alice"))
			{
				pConflictedCollider->Set_Conflicted();
				((CPlayer*)(pConflictedObj))->Add_Dmg_to_Player(rand() % 2 + 3);
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

HRESULT CWaspInk::SetUp_Components()
{

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_WaspInk), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	FAILED_CHECK(m_pModel->Change_AnimIndex(0));


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	//Pivot  : 0.000000f , 0.800000f , 0.000000f , 1
	ColliderDesc.vScale = _float3(3);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.000000f, 1.5f, 0.000000f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));


	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = 4.8f;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));

	__super::SetUp_WanderLook(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK));

	return S_OK;
}

HRESULT CWaspInk::NearPatternWander(_double fDeltaTime)
{
	_uint iCheckCount = 0;

	while (!(Check_Movable_Terrain(m_pTransformCom, m_vLookDir.XMVector(), 0.1f)))
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
		if (iCheckCount > 5) break;
		iCheckCount++;
	}

	_Vector MosterPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
	_Vector PlayerPos = m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS);

	_Vector NewLookDir = XMVector3Normalize(XMVectorSetY(PlayerPos - MosterPos,0));

	 _float fCosTheta= XMVectorGetX(XMVector3Dot(NewLookDir,XMVector3Normalize(XMVectorSetY(m_vLookDir.XMVector(), 0))));

	 _float fDegree = XMConvertToDegrees(acosf(fCosTheta));

	 if ((m_vLookDir.z * XMVectorGetX(NewLookDir) - m_vLookDir.x * XMVectorGetZ(NewLookDir)) > 0)
		 fDegree = 360 - fDegree;


	 if (fDegree < 45 || fDegree >= 315)
	 {
		 m_pModel->Change_AnimIndex(2);
	 }
	 else if (fDegree >= 45 && fDegree < 135) 
	 {
		 m_pModel->Change_AnimIndex(5);
	 }
	 else if (fDegree >= 135 && fDegree < 225)
	 {
		 m_pModel->Change_AnimIndex(3);
	 }
	 else if (fDegree >= 225 && fDegree < 315)
	 {
		 m_pModel->Change_AnimIndex(4);
	 }

	m_pTransformCom->LookDir(NewLookDir*(0.2f)+ m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.8f));
	m_pTransformCom->MovetoDir(m_vLookDir.XMVector(), fDeltaTime);



	return S_OK;
}

CWaspInk * CWaspInk::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CWaspInk*	pInstance = new CWaspInk(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CWaspInk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWaspInk::Clone(void * pArg)
{
	CWaspInk*	pInstance = new CWaspInk(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CWaspInk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWaspInk::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);

}
