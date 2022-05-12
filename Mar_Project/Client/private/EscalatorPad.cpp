#include "stdafx.h"
#include "..\public\EscalatorPad.h"
#include "Player.h"
#include "Terrain.h"



CEscalatorPad::CEscalatorPad(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CEscalatorPad::CEscalatorPad(const CEscalatorPad & rhs)
	: CMapObject(rhs)
{
}

HRESULT CEscalatorPad::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CEscalatorPad::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(ESCLATORDESC));
	}

	FAILED_CHECK(SetUp_Components());





	FAILED_CHECK(Set_TerrainTileKinds());

	return S_OK;
}

_int CEscalatorPad::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;


	FAILED_CHECK(Update_Escalating(fDeltaTime));





	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS),8);

	if (m_bIsOnScreen)
	{

		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

		g_pGameInstance->Add_CollisionGroup(CollisionType_MonsterWeapon, this, m_pColliderCom);

	}
	return _int();
}

_int CEscalatorPad::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;



	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CEscalatorPad::Render()
{
	if (__super::Render() < 0)
		return -1;

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG



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

_int CEscalatorPad::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

	return _int();
}

HRESULT CEscalatorPad::Set_TerrainTileKinds()
{
	CTerrain* pTerrain = (CTerrain*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));
	NULL_CHECK_RETURN(pTerrain, E_FAIL);



	_Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();


	_Matrix RectMatrix = XMMatrixIdentity();

	RectMatrix.r[0] = m_NevRectPoint[0].XMVector();
	RectMatrix.r[1] = m_NevRectPoint[1].XMVector();
	RectMatrix.r[2] = m_NevRectPoint[2].XMVector();
	RectMatrix.r[3] = m_NevRectPoint[3].XMVector();



	FAILED_CHECK(pTerrain->Chage_TileKindsNHeight(RectMatrix * WorldMatrix));



	return S_OK;
}

HRESULT CEscalatorPad::Reset_TerrainTileKindsMovableNHeightZero()
{
	CTerrain* pTerrain = (CTerrain*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));
	NULL_CHECK_RETURN(pTerrain, E_FAIL);


	_Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();


	_Matrix RectMatrix = XMMatrixIdentity();

	RectMatrix.r[0] = m_NevRectPoint[0].XMVector();
	RectMatrix.r[1] = m_NevRectPoint[1].XMVector();
	RectMatrix.r[2] = m_NevRectPoint[2].XMVector();
	RectMatrix.r[3] = m_NevRectPoint[3].XMVector();



	FAILED_CHECK(pTerrain->Chage_TileKindsMovableNZero(RectMatrix * WorldMatrix));


	return S_OK;
}

void CEscalatorPad::LetEscalatingToDest(_bool bBool)
{
	if (m_PassedTime > m_tDesc.MoveSpeed && bBool != m_bMoveToDest)
	{
		m_bMoveToDest = bBool;
		m_PassedTime = 0;
	}
}

void CEscalatorPad::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		//m_pPlayer->Set_LevitationTime((_float)g_fDeltaTime);
		_Vector Dir = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) - m_vOldPos.XMVector();
		_float Lenth = XMVectorGetX(XMVector3Length(Dir));

		m_pPlayerTransform->MovetoDir_bySpeed(Dir, Lenth, 1);



		if (m_pPlayerTransform->Get_MatrixState_Float3(CTransform::STATE_POS).y < m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).y + 2.5f)
		{
			_float3 PlayerPos = m_pPlayerTransform->Get_MatrixState_Float3(CTransform::STATE_POS);
			PlayerPos.y = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).y + 2.5f;

			m_pPlayerTransform->Set_MatrixState(CTransform::STATE_POS, PlayerPos);
		}

		//pConflictedCollider->Set_Conflicted();
		//((CPlayer*)(pConflictedObj))->Add_Dmg_to_Player(rand() % 2 + 3);

	}
	break;
	case Engine::CollisionType_Terrain:
		break;

	default:
		break;
	}


}

HRESULT CEscalatorPad::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));


	m_pTransformCom->Set_MoveSpeed(m_tDesc.MoveSpeed);
	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_tDesc.vStartPos);


	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_HatterFloorSystem_OriginalShortA), TAG_COM(Com_Model), (CComponent**)&m_pModel));


	ZeroMemory(m_NevRectPoint, sizeof(_float4) * 4);

	m_NevRectPoint[0] = _float4(0, 2.5f, 0, 1);
	m_NevRectPoint[1] = _float4(0, 2.5f, -5.f, 1);
	m_NevRectPoint[2] = _float4(-5.f, 2.5f, -5.f, 1);
	m_NevRectPoint[3] = _float4(-5.f, 2.5f, 0, 1);

	/*
	//switch (m_iKindsOfHiddenPad)
	//{
	//case 0:
	//	m_NevRectPoint[0] = _float4(-0.779999f, 1.619999f, -0.839999f, 1);
	//	m_NevRectPoint[1] = _float4(0.779999f, 1.619999f, -0.839999f, 1);
	//	m_NevRectPoint[2] = _float4(0.779999f, 1.619999f, 0.779999f, 1);
	//	m_NevRectPoint[3] = _float4(-0.779999f, 1.619999f, 0.779999f, 1);

	//	break;

	//case 1:
	//	m_NevRectPoint[0] = _float4(-0.6f, 0.3f, 0, 1);
	//	m_NevRectPoint[1] = _float4(0.6f, 0.3f, 0, 1);
	//	m_NevRectPoint[2] = _float4(0.6f, 0.3f, 2.34f, 1);
	//	m_NevRectPoint[3] = _float4(-0.6f, 0.3f, 2.34f, 1);
	//	break;

	//case 2:
	//	m_NevRectPoint[0] = _float4(-0.6f, 0.3f, 0, 1);
	//	m_NevRectPoint[1] = _float4(0.6f, 0.3f, 0, 1);
	//	m_NevRectPoint[2] = _float4(0.6f, 0.3f, 2.34f, 1);
	//	m_NevRectPoint[3] = _float4(-0.6f, 0.3f, 2.34f, 1);
	//	break;

	//case 3:
	//	m_NevRectPoint[0] = _float4(-1.2f, 0.48f, -0.6f, 1);
	//	m_NevRectPoint[1] = _float4(1.14f, 0.48f, -0.6f, 1);
	//	m_NevRectPoint[2] = _float4(1.14f, 3.84f, 8.52f, 1);
	//	m_NevRectPoint[3] = _float4(-1.2f, 3.84f, 8.52f, 1);
	//	break;

	//default:
	//	break;
	//}

	*/



	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(8.f, 1.0f, 1.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-2.5f, 1.25f, -2.5f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));


	ColliderDesc.vScale = _float3(5.2f, 2.7f, 5.2f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-2.5f, 1.6f, -2.5f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_AABB, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();





	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);



	return S_OK;
}

HRESULT CEscalatorPad::Update_Escalating(_double fDeltaTime)
{

	if (m_PassedTime > m_tDesc.MoveSpeed)
		return S_FALSE;

	m_PassedTime += fDeltaTime;

	_float3 EasedPos = {};

	if (m_bMoveToDest)
		EasedPos = g_pGameInstance->Easing_Vector(TYPE_ExpoInOut, m_tDesc.vStartPos, m_tDesc.vDestPos, (_float)m_PassedTime, m_tDesc.MoveSpeed);
	else
		EasedPos = g_pGameInstance->Easing_Vector(TYPE_ExpoInOut, m_tDesc.vDestPos, m_tDesc.vStartPos, (_float)m_PassedTime, m_tDesc.MoveSpeed);

	FAILED_CHECK(Reset_TerrainTileKindsMovableNHeightZero());
	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);
	FAILED_CHECK(Set_TerrainTileKinds());

	return S_OK;
}

CEscalatorPad * CEscalatorPad::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEscalatorPad*	pInstance = new CEscalatorPad(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CEscalatorPad");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEscalatorPad::Clone(void * pArg)
{
	CEscalatorPad*	pInstance = new CEscalatorPad(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CEscalatorPad");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEscalatorPad::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);

	

}
