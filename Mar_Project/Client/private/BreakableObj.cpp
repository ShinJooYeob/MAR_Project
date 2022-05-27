#include "stdafx.h"
#include "..\public\BreakableObj.h"



CBreakableObj::CBreakableObj(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice, pDeviceContext)
{
}

CBreakableObj::CBreakableObj(const CBreakableObj & rhs)
	: CMapObject(rhs)
{
}

HRESULT CBreakableObj::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CBreakableObj::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		m_iKindsOfMesh = _uint((*(_float4*)pArg).w);

		switch (m_iKindsOfMesh)
		{
		case Prototype_Mesh_BreakableBox:
			m_iPieceNum = 6;
			break;

		case Prototype_Mesh_BreakableBarrel:
			m_iPieceNum = 5;
			break;

		case Prototype_Mesh_GiftBasket:
			m_iPieceNum = 0;
			break;

		default:
			m_iPieceNum = 0;
			break;
		}

	}

	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
	{
		_float3 Pos = *(_float3*)pArg;
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, Pos);
	}


	m_szNameTag = TAG_LAY(Layer_Breakable);

	return S_OK;
}

_int CBreakableObj::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;


	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));

	if (m_bIsOnScreen)
	{
		m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);


		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());


		g_pGameInstance->Add_CollisionGroup(CollisionType_Monster, this, m_pColliderCom);
#ifdef _DEBUG
		FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
	}



	//if (m_fHP <= 0 || g_pGameInstance->Get_DIKeyState(DIK_O) & DIS_Down)
	//{
	//	CGameInstance* pGameInstance = g_pGameInstance;
	//	_float3 vPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
	//	_uint iNumTeethCount = rand() % 3 + 2;


	//	if (m_iKindsOfMesh == Prototype_Mesh_GiftBasket)
	//	{
	//		_uint IsRoseNum = rand() % 2 + 1;

	//		for (_uint i = 0; i < IsRoseNum; i++)
	//		{
	//			FAILED_CHECK(pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_RoseObj),
	//				TAG_OP(Prototype_RoseObj), &_float3(vPos)));
	//		}
	//	}
	//	else
	//	{

	//		for (_uint i = 0; i < iNumTeethCount; i++)
	//		{
	//			_uint IsGold = rand() % 5;
	//			IsGold = (IsGold) ? 0 : 1;

	//			FAILED_CHECK(pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_TeethObj),
	//				TAG_OP(Prototype_TeethObj), &_float4(vPos, _float(IsGold))));
	//		}

	//	}

	//	//////////////////////乔胶 积己

	//	for (_uint i = m_iKindsOfMesh + 1; i < m_iKindsOfMesh + 1 + m_iPieceNum; i ++)
	//	{
	//		FAILED_CHECK(pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_BreakablePiece),
	//			TAG_OP(Prototype_BreakablePiece), &_float4(vPos, _float(i))));
	//	}



	//	

	//	Set_IsDead();
	//	return 0;
	//}



	return _int();
}

_int CBreakableObj::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;



	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CBreakableObj::Render()
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

_int CBreakableObj::LightRender()
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

HRESULT CBreakableObj::Add_Dmg_To_BreakableObj(_uint iDmg)
{
	m_fHP -= iDmg;


	if (m_fHP <= 0)
	{
		CGameInstance* pGameInstance = g_pGameInstance;
		_float3 vPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
		_uint iNumTeethCount = rand() % 3 + 2;


		if (m_iKindsOfMesh == Prototype_Mesh_GiftBasket)
		{
			_uint IsRoseNum = rand() % 2 + 1;

			for (_uint i = 0; i < IsRoseNum; i++)
			{
				FAILED_CHECK(pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_RoseObj),
					TAG_OP(Prototype_RoseObj), &_float3(vPos)));
			}
		}
		else
		{

			for (_uint i = 0; i < iNumTeethCount; i++)
			{
				_uint IsGold = rand() % 5;
				IsGold = (IsGold) ? 0 : 1;

				FAILED_CHECK(pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_TeethObj),
					TAG_OP(Prototype_TeethObj), &_float4(vPos, _float(IsGold))));
			}

		}

		//////////////////////乔胶 积己

		for (_uint i = m_iKindsOfMesh + 1; i < m_iKindsOfMesh + 1 + m_iPieceNum; i++)
		{
			FAILED_CHECK(pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_BreakablePiece),
				TAG_OP(Prototype_BreakablePiece), &_float4(vPos, _float(i))));
		}





		Set_IsDead();

		return S_OK;

	}
	return S_OK;
}

HRESULT CBreakableObj::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(COMPONENTPROTOTYPEID(m_iKindsOfMesh)), TAG_COM(Com_Model), (CComponent**)&m_pModel));


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 1, 0, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));


	return S_OK;
}

CBreakableObj * CBreakableObj::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CBreakableObj*	pInstance = new CBreakableObj(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CBreakableObj");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBreakableObj::Clone(void * pArg)
{
	CBreakableObj*	pInstance = new CBreakableObj(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CBreakableObj");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBreakableObj::Free()
{
	__super::Free();


	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
