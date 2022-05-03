#include "stdafx.h"
#include "..\public\Executor.h"
#include "Scythe.h"
#include "Terrain.h"



CExecutor::CExecutor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonster(pDevice,pDeviceContext)
{
}

CExecutor::CExecutor(const CExecutor & rhs)
	: CMonster(rhs)
{
}

HRESULT CExecutor::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CExecutor::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());


	if (pArg != nullptr)
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, *((_float3*)pArg));

	m_fHP = m_fMaxHP = 192;


	FAILED_CHECK(SetUp_Weapon());

	return S_OK;
}

_int CExecutor::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)return -1;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);

	if (g_pGameInstance->Get_DIKeyState(DIK_1)&DIS_Down)
		m_pModel->Change_AnimIndex(0);
	if (g_pGameInstance->Get_DIKeyState(DIK_2)&DIS_Down)
		m_pModel->Change_AnimIndex(1);
	if (g_pGameInstance->Get_DIKeyState(DIK_3)&DIS_Down)
		m_pModel->Change_AnimIndex(2);
	if (g_pGameInstance->Get_DIKeyState(DIK_4)&DIS_Down)
		m_pModel->Change_AnimIndex(8);
	if (g_pGameInstance->Get_DIKeyState(DIK_5)&DIS_Down)
		m_pModel->Change_AnimIndex(10);
	if (g_pGameInstance->Get_DIKeyState(DIK_6)&DIS_Down)
		m_pModel->Change_AnimIndex(11);
	if (g_pGameInstance->Get_DIKeyState(DIK_7)&DIS_Down)
		m_pModel->Change_AnimIndex(12);
	if (g_pGameInstance->Get_DIKeyState(DIK_8)&DIS_Down)
		m_pModel->Change_AnimIndex(13);
	if (g_pGameInstance->Get_DIKeyState(DIK_9)&DIS_Down)
		m_pModel->Change_AnimIndex(14);
	if (g_pGameInstance->Get_DIKeyState(DIK_0)&DIS_Down)
		m_pModel->Change_AnimIndex(16);
	//walk 2.8 / 0.05f

	//run 5.5 / 0.1
	


	if (!m_bIsPatternFinished || Distance_BetweenPlayer(m_pTransformCom) < 10)
	{
		Update_Pattern(fDeltaTime);
	}
	else
	{
		if (!m_pModel->Get_IsUntillPlay())
		{
			m_pModel->Change_AnimIndex((m_fHP < m_fMaxHP * 0.5f) ? 2 : 1);
			FAILED_CHECK(__super::Update_WanderAround(m_pTransformCom, fDeltaTime, (m_fHP < m_fMaxHP * 0.5f) ? 0.1f : 0.05f));
		}
	}
	







	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));
	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime , m_bIsOnScreen));

	if (m_bIsOnScreen)
	{

		_Matrix			TransformMatrix = XMLoadFloat4x4(m_tCollisionAttachPtr.pUpdatedNodeMat) * XMLoadFloat4x4(m_tCollisionAttachPtr.pDefaultPivotMat);

		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

		for (_uint i = 0 ; i < m_pColliderCom->Get_NumColliderBuffer() ; i++)
			m_pColliderCom->Update_Transform(i, TransformMatrix * m_pTransformCom->Get_WorldMatrix());

		

		g_pGameInstance->Add_CollisionGroup(CollisionType_Monster, this, m_pColliderCom);

		m_vecWeapon[0]->Update(fDeltaTime);
	}




	return _int();
}

_int CExecutor::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)return -1;

	FAILED_CHECK(Set_Monster_On_Terrain(m_pTransformCom, fDeltaTime));


	if (m_bIsOnScreen)
	{
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
		m_vecWeapon[0]->LateUpdate(fDeltaTime);
	}

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CExecutor::Render()
{
	if (__super::Render() < 0)		return -1;


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
	return _int();
}

_int CExecutor::LateRender()
{
	return _int();
}

_int CExecutor::Update_DmgCalculate(_double fDeltaTime)
{
	return _int();
}

_int CExecutor::Update_Pattern(_double fDeltaTime)
{

	m_PatternPassedTime += fDeltaTime;




	if (m_fHP < m_fMaxHP * 0.5f)
		//버닝 모드
	{
		if (m_bIsPatternFinished)
		{
			m_ePattern += 1;
			if (m_ePattern > 3) m_ePattern = 0;
			m_bIsPatternFinished = false;
			m_PatternPassedTime = 0;
			//m_pModel->Change_AnimIndex(2);
		}




	}
	else
		//일반 모드
	{
		if (m_bIsPatternFinished)
		{
			//m_ePattern += 1;
			//if (m_ePattern > 3) m_ePattern = 0;
			m_bIsPatternFinished = false;
			m_PatternPassedTime = 0;
		}


		switch (m_ePattern)
		{
		case 0:

		{

			if (!m_bIsJumping)
			{
				m_pModel->Change_AnimIndex_UntilTo(4, 6, 0.08, true);
				m_bIsJumping = true;
			}
			else if(m_pModel->Get_NowAnimIndex() == 5 && !m_bIsAddForceActived)
			{
				m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
				m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.15f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.85f));
				Add_Force(m_pTransformCom, m_vLookDir.XMVector() + XMVectorSet(0, 1, 0, 0), 50);
			}
		}

			break;
		case 1:
	


			break;
		case 2:

			break;
		case 3:
	
			break;


		default:
			break;
		}


	}


	


	return _int();
}

void CExecutor::Add_Dmg_to_Monster(_float iDmgAmount)
{
	m_DmgPassedTime = MonsterDmgTime;
	m_fDmgAmount += iDmgAmount;
	m_fHP -= iDmgAmount;

	if (m_fHP < m_fMaxHP * 0.5f) m_pTransformCom->Set_MoveSpeed(5.5f);
}

HRESULT CExecutor::SetUp_Components()
{

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_Executor), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	FAILED_CHECK(m_pModel->Change_AnimIndex(0));

	m_tCollisionAttachPtr = m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-Spine1");
	NULL_CHECK_RETURN(m_tCollisionAttachPtr.pDefaultPivotMat, E_FAIL);



	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));


	//Pivot  : -0.160000f , 0.000000f , -4.440007f , 1
	//size  : 7.600080f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(7.600080f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-0.160000f, 0.000000f, -4.440007f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));



	//Pivot  : 0.000000f , 0.310000f , -2.019999f , 1
	//size  : 2.039999f , 0.450001f , 1.180000f  
	//(x,z,y)
	//size  : 4.910018f , 1.699999f , 3.499998f  

	ColliderDesc.vScale = _float3(3.499998f, 2.699999f, 4.910018f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);	
	ColliderDesc.vPosition = _float4(-0.160000f, 0.000000f, -4.440007f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_OBB, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();



	CTransform::TRANSFORMDESC tDesc = {};
	tDesc.fMovePerSec = 2.8f;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));
	__super::SetUp_WanderLook(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK));


	return S_OK;
}

HRESULT CExecutor::SetUp_Weapon()
{

	CGameInstance* pInstance = GetSingle(CGameInstance);

	NULL_CHECK_RETURN(m_pModel, E_FAIL);
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);

	CMonsterWeapon::MONWEAPONDESC tWeaponDesc;
	tWeaponDesc.pModel = m_pModel;
	tWeaponDesc.pParantTransform = m_pTransformCom;
	tWeaponDesc.szHirarchyNodeName = "Bip01-Prop1";

	CMonsterWeapon* pWeapon = nullptr;
	pInstance->Add_GameObject_Out_of_Manager((CGameObject**)&pWeapon, m_eNowSceneNum, TAG_OP(Prototype_Scythe), &tWeaponDesc);
	NULL_CHECK_RETURN(pWeapon, E_FAIL);

	m_vecWeapon.push_back(pWeapon);


	return S_OK;
}

HRESULT CExecutor::Set_Monster_On_Terrain(CTransform * pTransform, _double fDeltaTime)
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

		if (m_bIsJumping && m_pModel->Get_NowAnimIndex() != 4)
		{
			m_bIsJumping = false;
			m_pModel->Change_AnimIndex_ReturnTo(7, 0, 0.15, true);
		}
	}

	return S_OK;
}

CExecutor * CExecutor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CExecutor*	pInstance = new CExecutor(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CExecutor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CExecutor::Clone(void * pArg)
{
	CExecutor*	pInstance = new CExecutor(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CExecutor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CExecutor::Free()
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
