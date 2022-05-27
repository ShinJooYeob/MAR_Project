#include "stdafx.h"
#include "..\public\ButtonPad.h"
#include "Player.h"
#include "Terrain.h"
#include "ClockBomb.h"
#include "EscalatorPad.h"



CButtonPad::CButtonPad(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CButtonPad::CButtonPad(const CButtonPad & rhs)
	: CMapObject(rhs)
{
}

HRESULT CButtonPad::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CButtonPad::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(BUTTONDESC));
	}

	FAILED_CHECK(SetUp_Components());



	return S_OK;
}

_int CButtonPad::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	if (!m_bVisuable) return 0;

	FAILED_CHECK(Update_ButtonAnim(fDeltaTime));



	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(
		m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) ,3);

	if (m_bIsOnScreen)
	{

		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

		g_pGameInstance->Add_CollisionGroup(CollisionType_DynaicObject, this, m_pColliderCom);
#ifdef _DEBUG
		FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
	}
	else
	{
		if (m_bChecker)
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

_int CButtonPad::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (!m_bVisuable) return 0;


	LetWorkButton(!m_bChecker);

	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CButtonPad::Render()
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
	ButtonMat._42 -= m_ButtonHight;

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

_int CButtonPad::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	return _int();
}




void CButtonPad::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{

		if (m_pPlayerTransform->Get_MatrixState_Float3(CTransform::STATE_POS).y < m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).y + 0.6f - m_ButtonHight)
		{
			switch (m_tDesc.eKindsOfObject)
			{
			case 0:
				m_pPlayer->Set_ReturnPos(_float3(75.830f, 23.670f, 77.509f), _float3(84.321f, 23.670f, 95.669f));
				break;
			default:
				break;
			}


			

			m_bChecker = true;


			_float3 PlayerPos = m_pPlayerTransform->Get_MatrixState_Float3(CTransform::STATE_POS);
			PlayerPos.y = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).y + 0.6f - m_ButtonHight;

			m_pPlayer->Set_NotLevitation();
			m_pPlayerTransform->Set_MatrixState(CTransform::STATE_POS, PlayerPos);
		}



	}
	break;
	case Engine::CollisionType_PlayerWeapon:
	{
 		if (!lstrcmp(pConflictedObj->Get_NameTag(), TAG_LAY(Layer_ClockBomb)))
		{
			CClockBomb* pObject = (CClockBomb*)pConflictedObj;

			CTransform* pTransform = (CTransform*)(pObject->Get_Component(TAG_COM(Com_Transform)));
			_float3		ObjectPos = pTransform->Get_MatrixState_Float3(CTransform::STATE_POS);
			if (pObject->Get_IsOn())
			{
				m_bChecker = true;

				ObjectPos.y = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).y + 0.6f - m_ButtonHight;
				pTransform->Set_MatrixState(CTransform::STATE_POS, ObjectPos);

			}
			else
			{
				if (pTransform->Get_MatrixState_Float3(CTransform::STATE_POS).y < m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).y + 0.6f - m_ButtonHight)
				{
					m_bChecker = true;


					pObject->Set_IsOn();

					ObjectPos.y = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).y + 0.6f - m_ButtonHight;
					pTransform->Set_MatrixState(CTransform::STATE_POS, ObjectPos);

				}
			}
		}

	}
		break;

	default:
		break;
	}


}

void CButtonPad::LetWorkButton(_bool bBool)
{
	if (m_PassedTime > 1 && bBool != m_bIsUp)
	{
		m_bIsUp = bBool;
		m_PassedTime = 0;
	}

}

HRESULT CButtonPad::SetUp_Components()
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



	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(4., 1.0f, 1.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.f, 0, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));


	ColliderDesc.vScale = _float3(2.8f,2.8f, 2.8f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, -0.475f, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_AABB, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();





	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);



	return S_OK;
}

HRESULT CButtonPad::Update_ButtonAnim(_double fDeltaTime)
{
	if (m_PassedTime > 1)
	{

		switch (m_tDesc.eKindsOfObject)
		{
		case 0:
		case 1:
			((CEscalatorPad*)m_tDesc.pTargetObject)->LetEscalatingToDest(!m_bIsUp);

			break;

		default:
			break;
		}

		return S_FALSE;
	}



	m_PassedTime += fDeltaTime;

	if (m_bIsUp)
	{
		m_ButtonHight = g_pGameInstance->Easing(TYPE_ExpoInOut, 0.3f, 0, (_float)m_PassedTime, 1);

		if (m_ButtonHight < 0.1)
		{

			switch (m_tDesc.eKindsOfObject)
			{
			case 0:
			case 1:
				((CEscalatorPad*)m_tDesc.pTargetObject)->LetEscalatingToDest(!m_bIsUp);

				break;

			default:
				break;
			}
		}
	}
	else
	{
		m_ButtonHight = g_pGameInstance->Easing(TYPE_ExpoInOut, 0, 0.3f, (_float)m_PassedTime, 1);

		if (m_ButtonHight > 0.2)
		{

			switch (m_tDesc.eKindsOfObject)
			{
			case 0:
			case 1:
				((CEscalatorPad*)m_tDesc.pTargetObject)->LetEscalatingToDest(!m_bIsUp);

				break;

			default:
				break;
			}
		}

	}

	return S_OK;
}


CButtonPad * CButtonPad::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CButtonPad*	pInstance = new CButtonPad(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CButtonPad");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CButtonPad::Clone(void * pArg)
{
	CButtonPad*	pInstance = new CButtonPad(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CButtonPad");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CButtonPad::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pSubModel);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
