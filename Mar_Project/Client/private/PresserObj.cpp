#include "stdafx.h"
#include "..\public\PresserObj.h"
#include "Player.h"
#include "Terrain.h"



CPresserObj::CPresserObj(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CPresserObj::CPresserObj(const CPresserObj & rhs)
	: CMapObject(rhs)
{
}

HRESULT CPresserObj::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CPresserObj::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(PRESSERDESC));
	}

	FAILED_CHECK(SetUp_Components());




	return S_OK;
}

_int CPresserObj::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;


	FAILED_CHECK(Update_Escalating(fDeltaTime));





	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS),13);

	if (m_bIsOnScreen)
	{

		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

		g_pGameInstance->Add_CollisionGroup(CollisionType_MonsterWeapon, this, m_pColliderCom);

#ifdef _DEBUG
		FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
	}
	return _int();
}

_int CPresserObj::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;



	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CPresserObj::Render()
{
	if (__super::Render() < 0)
		return -1;





	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));


	FAILED_CHECK(__super::SetUp_ConstTable(m_pShaderCom));


	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 1, i));
	}


	return _int();
}

_int CPresserObj::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;
	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));



	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{
		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 9, i));
	}

	return _int();

}




void CPresserObj::LetEscalatingToDest(_bool bBool)
{
	if (m_PassedTime > m_tDesc.MoveSpeed && bBool != m_bMoveToDest)
	{
		m_bMoveToDest = bBool;
		m_PassedTime = 0;
	}
}

void CPresserObj::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{

	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		m_pPlayer->Set_ReturnPos(_float3(102.431f, 26.70f, 130.580f), _float3(100.431f, 26.70f, 130.580f));
		m_pPlayer->Add_Dmg_to_Player(1000);

	}
	break;
	case Engine::CollisionType_Terrain:
		break;

	default:
		break;
	}


}

HRESULT CPresserObj::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));


	m_pTransformCom->Set_MoveSpeed(m_tDesc.MoveSpeed);

	_Matrix WorldMatrix = XMMatrixRotationX(XMConvertToRadians(180))*
		XMMatrixTranslation(m_tDesc.vStartPos.x, m_tDesc.vStartPos.y, m_tDesc.vStartPos.z);

	m_pTransformCom->Set_Matrix(WorldMatrix);

	//m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_tDesc.vStartPos);


	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_HatterBlocksE), TAG_COM(Com_Model), (CComponent**)&m_pModel));






	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(6.f, 1.0f, 1.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 10.f, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));


	ColliderDesc.vScale = _float3(4.0f, 1.0f, 4.0f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, -10.f, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_AABB, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();





	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);


	m_TargetTime = m_tDesc.MoveSpeed * GetSingle(CUtilityMgr)->RandomFloat(0.7f, 1.5f);
	return S_OK;
}

HRESULT CPresserObj::Update_Escalating(_double fDeltaTime)
{

	m_PassedTime += fDeltaTime;

	_float3 EasedPos = {};

	if (m_bMoveToDest)
	{

		EasedPos = g_pGameInstance->Easing_Vector(TYPE_ExpoInOut, m_tDesc.vStartPos, m_tDesc.vDestPos, (_float)m_PassedTime, (_float)m_TargetTime * 6.f);
		if (m_PassedTime > (_float)m_TargetTime * 6.f)
		{
			EasedPos = m_tDesc.vDestPos;
			m_PassedTime = 0;
			m_bMoveToDest = false;
		}
	}
	else
	{
		EasedPos = g_pGameInstance->Easing_Vector(TYPE_ExpoIn, m_tDesc.vDestPos, m_tDesc.vStartPos, (_float)m_PassedTime, (_float)m_TargetTime);

		if (m_PassedTime > (_float)m_TargetTime)
		{
			EasedPos = m_tDesc.vStartPos;
			m_PassedTime = 0;
			m_bMoveToDest = true;
			m_TargetTime = m_tDesc.MoveSpeed * GetSingle(CUtilityMgr)->RandomFloat(0.7f, 1.5f);
		}
	}

	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);

	return S_OK;
}

CPresserObj * CPresserObj::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CPresserObj*	pInstance = new CPresserObj(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CPresserObj");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPresserObj::Clone(void * pArg)
{
	CPresserObj*	pInstance = new CPresserObj(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CPresserObj");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPresserObj::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);

	

}
