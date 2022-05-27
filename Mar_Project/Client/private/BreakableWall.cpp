#include "stdafx.h"
#include "Player.h"
#include "..\public\BreakableWall.h"



CBreakableWall::CBreakableWall(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice, pDeviceContext)
{
}

CBreakableWall::CBreakableWall(const CBreakableWall & rhs)
	: CMapObject(rhs)
{
}

HRESULT CBreakableWall::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CBreakableWall::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		m_iKindsOfMesh = _uint((*(_float4*)pArg).w);
	}

	FAILED_CHECK(SetUp_Components());


	_Matrix WorldMat = XMMatrixScaling(0.4f, 0.4f, 0.4f)
		*XMMatrixRotationX(XMConvertToRadians(0)) *XMMatrixRotationY(XMConvertToRadians(177.6f)) * XMMatrixRotationZ(XMConvertToRadians(0))
		* XMMatrixTranslation(125.013f, 25.860f, 163.490f);

	m_pTransformCom->Set_Matrix(WorldMat);



	m_szNameTag = TAG_LAY(Layer_Breakable);
	m_iPieceNum = 0;

	return S_OK;
}

_int CBreakableWall::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;



	if (m_iPieceNum == 1)
	{
		m_PassedTime += fDeltaTime;

		if (m_PassedTime > 2)
		{
			m_iPieceNum = 2;
		}
	}



	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS),20);
	

	if (m_iPieceNum != 2 && m_bIsOnScreen)
	{
		m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);


		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());


		g_pGameInstance->Add_CollisionGroup(CollisionType_DynaicObject, this, m_pColliderCom);
#ifdef _DEBUG
		FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
	}




	return _int();
}

_int CBreakableWall::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;



	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CBreakableWall::Render()
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

_int CBreakableWall::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	return _int();
}
void CBreakableWall::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{

		if (m_iPieceNum == 0 && m_pPlayer->Get_IsGiantAtteck())
		{
			CGameInstance* pInstance = g_pGameInstance;
			CUtilityMgr* pUtil = GetSingle(CUtilityMgr);
			_Vector vTranslation = XMVectorSetW(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS),0);

			for (_uint i = 0; i < 60; i++)
			{
				if (rand() % 2)
				{
					_float4 TargetPos = vTranslation + XMVectorSet(-6 + (i *0.2f), pUtil->RandomFloat(-3, 2), -2, _float(Prototype_Mesh_Gazebo_Piece01 + (rand() % 4) ));
					pInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_BreakablePiece), TAG_OP(Prototype_BreakablePiece), &TargetPos);

				}
				else
				{
					_float4 TargetPos = vTranslation + XMVectorSet(-6 + (i *0.2f), pUtil->RandomFloat(-3, 2), -2, _float(Prototype_QBattleTowerParticleA + (rand() % 2)));
					pInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_BreakablePiece), TAG_OP(Prototype_BreakablePiece), &TargetPos);

				}
			}

			Change_Component_by_NewAssign(m_eNowSceneNum, TAG_CP(Prototype_QGiantBreakableGate_Broken), TAG_COM(Com_Model));
			m_iPieceNum++;
		}

	}
	break;


	default:
		break;
	}

}


HRESULT CBreakableWall::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_QGiantBreakableGate_Flesh), TAG_COM(Com_Model), (CComponent**)&m_pModel));


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(60.f, 2.f, 2.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 1, 0, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));

	ColliderDesc.vScale = _float3(30, 30.f, 6.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0, 10.f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_OBB, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();

	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));
	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);
	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));
	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);



	return S_OK;
}

CBreakableWall * CBreakableWall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CBreakableWall*	pInstance = new CBreakableWall(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CBreakableWall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBreakableWall::Clone(void * pArg)
{
	CBreakableWall*	pInstance = new CBreakableWall(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CBreakableWall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBreakableWall::Free()
{
	__super::Free();


	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
