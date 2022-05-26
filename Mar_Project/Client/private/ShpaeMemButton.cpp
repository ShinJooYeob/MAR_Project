#include "stdafx.h"
#include "..\public\ShpaeMemButton.h"
#include "Player.h"
#include "Terrain.h"
#include "ClockBomb.h"
#include "ShapeMemoryPad.h"



CShpaeMemButton::CShpaeMemButton(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CShpaeMemButton::CShpaeMemButton(const CShpaeMemButton & rhs)
	: CMapObject(rhs)
{
}

HRESULT CShpaeMemButton::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CShpaeMemButton::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(BUTTONDESC));
	}

	FAILED_CHECK(SetUp_Components());
	m_PassedTime = 16;


	return S_OK;
}

_int CShpaeMemButton::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	FAILED_CHECK(Update_ButtonAnim(fDeltaTime));



	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(
		m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) ,3);

	if (m_bIsOnScreen)
	{

		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());


		if (m_PassedTime >= 7)

		{
			g_pGameInstance->Add_CollisionGroup(CollisionType_DynaicObject, this, m_pColliderCom);

#ifdef _DEBUG
			FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
		}
	}
	else
	{
		if (m_PassedTime >= 7)
		{
			g_pGameInstance->Add_CollisionGroup(CollisionType_DynaicObject, this, m_pColliderCom);

#ifdef _DEBUG
			FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
		}

		
	}

	m_bChecker = false;

	return _int();
}

_int CShpaeMemButton::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;


	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CShpaeMemButton::Render()
{
	if (__super::Render() < 0)
		return -1;




	NULL_CHECK_RETURN(m_pModel, E_FAIL);
	NULL_CHECK_RETURN(m_pModel, E_FAIL);

	FAILED_CHECK(__super::SetUp_ConstTable(m_pShaderCom));


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	_uint NumMaterial = m_pSubModel->Get_NumMaterial();
	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pSubModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pSubModel->Render(m_pShaderCom, 1, i));
	}


	_float4x4		ShaderWorldMatrix;
	_float4x4		ButtonMat = m_pTransformCom->Get_WorldFloat4x4();
	ButtonMat._43 += m_ButtonHight;

	XMStoreFloat4x4(&ShaderWorldMatrix, ButtonMat.TransposeXMatrix());
	m_pShaderCom->Set_RawValue("g_WorldMatrix", &ShaderWorldMatrix, sizeof(_float4x4));

	NumMaterial = m_pModel->Get_NumMaterial();
	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 1, i));
	}



	return _int();
}

_int CShpaeMemButton::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

	return _int();
}




void CShpaeMemButton::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{


	case Engine::CollisionType_PlayerWeapon:
	{

		m_pPlayer->Set_ReturnPos(_float3(135.377f, 45.050f, 171.863f), _float3(166.329f, 39.12f, 171.863f));


 		if (!lstrcmp(pConflictedObj->Get_NameTag(), L"Layer_GranadeBullet"))
		{
			Set_ButtonIsUp(false);
		}
		else if (!lstrcmp(pConflictedObj->Get_NameTag(), L"Layer_NormalBullet"))
		{
			m_iHP--;
			if (m_iHP <= 0)
				Set_ButtonIsUp(false);
		}

	}
		break;

	default:
		break;
	}


}

void CShpaeMemButton::Set_ButtonIsUp(_bool bBool)
{
	if (m_PassedTime >= 7 && bBool != m_bIsUp)
	{
		m_bIsUp = bBool;
		m_PassedTime = 0;
		m_iHP = 5;
	}

}

HRESULT CShpaeMemButton::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));

	_Matrix WorldMatrix = XMMatrixRotationX(XMConvertToRadians(m_tDesc.vAngle.x))*
		XMMatrixRotationY(XMConvertToRadians(m_tDesc.vAngle.y))*
		XMMatrixRotationZ(XMConvertToRadians(m_tDesc.vAngle.z))*
		XMMatrixTranslation(m_tDesc.vPosition.x, m_tDesc.vPosition.y, m_tDesc.vPosition.z);

	m_pTransformCom->Set_Matrix(WorldMatrix);


	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_Button), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_Button_Frame), TAG_COM(Com_SubModel), (CComponent**)&m_pSubModel));


	ZeroMemory(m_NevRectPoint, sizeof(_float4) * 4);



	m_NevRectPoint[0] = _float4(0.8f, 0.6f, 0.8f, 1);
	m_NevRectPoint[1] = _float4(0.8f, 0.6f, -0.8f, 1);
	m_NevRectPoint[2] = _float4(-0.8f, 0.6f, -0.8f, 1);
	m_NevRectPoint[3] = _float4(-0.8f, 0.6f, 0.8f, 1);


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(5.f, 1.0f, 1.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.f, 0, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));


	ColliderDesc.vScale = _float3(2.8f,2.8f, 2.8f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0,0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_AABB, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();





	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);



	return S_OK;
}

HRESULT CShpaeMemButton::Update_ButtonAnim(_double fDeltaTime)
{
	if (!m_bIsUp)
	{
		if (m_PassedTime < 1)
		{
			m_PassedTime += fDeltaTime;
			m_ButtonHight = g_pGameInstance->Easing(TYPE_ExpoInOut, 0, 0.6f, (_float)m_PassedTime, 1);
			if (m_ButtonHight > 0.5)
				((CShapeMemoryPad*)m_tDesc.pTargetObject)->Let_ReturntoShape(true);
		}
		else if (m_PassedTime < 6)
		{
			m_PassedTime += fDeltaTime;
			((CShapeMemoryPad*)m_tDesc.pTargetObject)->Let_ReturntoShape(true);
		}
		else if (m_PassedTime < 7) {
			m_PassedTime += fDeltaTime;
			m_ButtonHight = g_pGameInstance->Easing(TYPE_ExpoInOut, 0.6f, 0, (_float)m_PassedTime - 6.f, 1);

			if (m_ButtonHight < 0.1)
				((CShapeMemoryPad*)m_tDesc.pTargetObject)->Let_ReturntoShape(false);

		}
		else
		{
			m_bIsUp = true;
		}
	}

	

	return S_OK;
}


CShpaeMemButton * CShpaeMemButton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CShpaeMemButton*	pInstance = new CShpaeMemButton(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CShpaeMemButton");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CShpaeMemButton::Clone(void * pArg)
{
	CShpaeMemButton*	pInstance = new CShpaeMemButton(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CShpaeMemButton");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShpaeMemButton::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pSubModel);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
