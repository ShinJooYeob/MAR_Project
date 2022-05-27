#include "stdafx.h"
#include "..\public\ShrinkFlower.h"
#include "Player.h"




CShrinkFlower::CShrinkFlower(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CShrinkFlower::CShrinkFlower(const CShrinkFlower & rhs)
	: CMapObject(rhs)
{
}

HRESULT CShrinkFlower::Initialize_Prototype(void * pArg)
{
	__super::Initialize_Prototype(pArg);

	return S_OK;
}

HRESULT CShrinkFlower::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	FAILED_CHECK(SetUp_Components());


	if (pArg != nullptr)
	{
		_float3 Pos = *(_float3*)pArg;
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, Pos);
	}

	
	return S_OK;
}

_int CShrinkFlower::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	if (m_bIsDead) return 0;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);

	if (!m_bCanTrapped && m_TrappedCoolTime < 3)
	{
		m_TrappedCoolTime += fDeltaTime;
		if (m_TrappedCoolTime > 3) m_bCanTrapped = true;
	}
	

	if (m_pModel->Get_NowAnimIndex() == 2 && m_pModel->Get_PlayRate() > 0.95)
		m_pModel->Change_AnimIndex_UntilNReturn_Must(0, 2, 0, 0.15, true);





	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 2);
	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime, m_bIsOnScreen));



	for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
		m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

	if (m_bCanTrapped)
	{
		g_pGameInstance->Add_CollisionGroup(CollisionType_DynaicObject, this, m_pColliderCom);

#ifdef _DEBUG
		FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
	}

	return _int();
}

_int CShrinkFlower::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsDead) return 0;


	if (m_bIsOnScreen)	
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CShrinkFlower::Render()
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

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 4, i, "g_BoneMatrices"));
	}


	return _int();
}

_int CShrinkFlower::LightRender()
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

void CShrinkFlower::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		if (m_TrappedCoolTime > 0)
		{
			m_pModel->Change_AnimIndex_UntilNReturn_Must(3, 4, 4, 0.15, true);
			m_pPlayer->Set_PlayerPosition(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
			m_pPlayer->Set_TrappedFlower(true);
			m_TrappedCoolTime = 0;
		}
		else
		{
			if (m_pPlayer->Get_TrapppedFlower())
			{
				m_pPlayer->Set_PlayerPosition(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
				m_TrappedCoolTime = 0;

				static double Timer = 0;
				Timer += g_fDeltaTime;
				if (Timer > 0.3)
				{
					m_pPlayer->Heal_to_Player(1);
					Timer = 0;
				}

			}
			else
			{
				m_pModel->Change_AnimIndex_ReturnTo(5, 2, 0.15, true);
				m_bCanTrapped = false;
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

HRESULT CShrinkFlower::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_ShrinkFlower), TAG_COM(Com_Model), (CComponent**)&m_pModel));

	m_pModel->Change_AnimIndex(2);
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(2.f, 3.f, 2.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);	
	ColliderDesc.vPosition = _float4(0, 0.0f, 0.f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));

	


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));



	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);

	return S_OK;
}

CShrinkFlower * CShrinkFlower::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CShrinkFlower*	pInstance = new CShrinkFlower(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(nullptr)))
	{
		MSGBOX("Failed to Created CShrinkFlower");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CShrinkFlower::Clone(void * pArg)
{
	CShrinkFlower*	pInstance = new CShrinkFlower(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CShrinkFlower");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShrinkFlower::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);
	
}
