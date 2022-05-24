#include "stdafx.h"
#include "..\public\Stage3_GientTrigger.h"
#include "Player.h"
#include "Terrain.h"
#include "Camera_Main.h"



CStage3_GientTrigger::CStage3_GientTrigger(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CStage3_GientTrigger::CStage3_GientTrigger(const CStage3_GientTrigger & rhs)
	: CMapObject(rhs)
{
}



HRESULT CStage3_GientTrigger::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CStage3_GientTrigger::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	FAILED_CHECK(SetUp_Components());


	
	return S_OK;
}



_int CStage3_GientTrigger::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	CGameInstance* pInstance = g_pGameInstance;


	for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
		m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

	if (!m_bSpwanStart)
		pInstance->Add_CollisionGroup(CollisionType_DynaicObject, this, m_pColliderCom);



	return _int();
}

_int CStage3_GientTrigger::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

#ifdef _DEBUG

#endif // _DEBUG
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CStage3_GientTrigger::Render()
{
	if (__super::Render() < 0)
		return -1;


#ifdef _DEBUG
	if (!m_bSpwanStart)
		m_pColliderCom->Render();
#endif // _DEBUG

	//if (m_bOilDropRender)
	//{

	//	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	//	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));


	//	FAILED_CHECK(__super::SetUp_ConstTable(m_pShaderCom));


	//	_uint NumMaterial = m_pModel->Get_NumMaterial();

	//	for (_uint i = 0; i < NumMaterial; i++)
	//	{

	//		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
	//			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

	//		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 0, i));
	//	}


	//}


	return _int();
}

_int CStage3_GientTrigger::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

	return _int();
}




void CStage3_GientTrigger::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		if (!m_iSpwanPoint)
		{
			m_pPlayer->Eat_Protain();
			m_iSpwanPoint = 1;



			_Matrix WorldMatrix = XMMatrixScaling(1, 10.f, 20.f)*
				XMMatrixRotationY(XMConvertToRadians(180)) *
				XMMatrixTranslation(123.544f, 24.0f, 161.657f);
			m_pTransformCom->Set_Matrix(WorldMatrix);
		}
		else if(m_iSpwanPoint == 1)
		{
			m_pPlayer->Set_GettingBigger(false);
			m_iSpwanPoint = 2;
			Set_IsDead();
		}




	}
	break;

	default:
		break;
	}


}



HRESULT CStage3_GientTrigger::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	//FAILED_CHECK(Add_Component(m_eNowSceneNum, L"Grunt_StaticSwpanMesh", TAG_COM(Com_Model), (CComponent**)&m_pModel));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));


	/*
	Bigger
	143.686f,19.990f,120.693f
	0,32.6f,0

	smaller
	123.544f,24.0f,161.657f
	0,90,0

	XMMatrixScaling(1, 10.f, 20.f)*
	XMMatrixRotationY(XMConvertToRadians(90)) *
	XMMatrixTranslation(123.544f,24.0f,161.657f);
	m_pTransformCom->Set_Matrix(WorldMatrix);

	*/

	_Matrix WorldMatrix = XMMatrixScaling(1, 10.f, 7.f)*
		XMMatrixRotationY(XMConvertToRadians(-60)) *
		XMMatrixTranslation(143.686f, 20.f, 120.693f);

	m_pTransformCom->Set_Matrix(WorldMatrix);


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(30);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0.f, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));


	ColliderDesc.vScale = _float3(1.f, 1.f, 1);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_OBB, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();

	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));


	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);



	return S_OK;
}

CStage3_GientTrigger * CStage3_GientTrigger::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CStage3_GientTrigger*	pInstance = new CStage3_GientTrigger(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CStage3_GientTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CStage3_GientTrigger::Clone(void * pArg)
{
	CStage3_GientTrigger*	pInstance = new CStage3_GientTrigger(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CStage3_GientTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStage3_GientTrigger::Free()
{
	__super::Free();

	
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

		

}
