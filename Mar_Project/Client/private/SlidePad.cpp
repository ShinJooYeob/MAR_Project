#include "stdafx.h"
#include "..\public\SlidePad.h"
#include "Player.h"
#include "Terrain.h"



CSlidePad::CSlidePad(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CSlidePad::CSlidePad(const CSlidePad & rhs)
	: CMapObject(rhs)
{
}

HRESULT CSlidePad::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CSlidePad::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(SLIDEDESC));
	}

	FAILED_CHECK(SetUp_Components());


	FAILED_CHECK(Set_TerrainTileKinds());

	return S_OK;
}

_int CSlidePad::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;







	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(
		m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT)*10.65f		,13);

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

_int CSlidePad::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;



	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CSlidePad::Render()
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

_int CSlidePad::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	return _int();
}

HRESULT CSlidePad::Set_TerrainTileKinds()
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

HRESULT CSlidePad::Reset_TerrainTileKindsMovableNHeightZero()
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



void CSlidePad::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		if (m_tDesc.bStartCollider && iMyColliderIndex == m_iStartPointColliderIndex)
		{
			m_pPlayer->Set_ReturnPos({ 186.256f, 69.f, 86.939f }, {185.839508f,69.f,90.557f});
			m_pPlayer->Let_PlayerSliding(true);

		}
		else if (m_tDesc.bEndCollider &&iMyColliderIndex == m_iEndPointColliderIndex)
		{
			m_pPlayer->Set_ReturnPos({ 152.476f, 43.2f, 130.360f }, {150.38f,43.2f,133.09f});

			m_pPlayer->Let_PlayerSliding(false);

		}
		else//피격처리해주면됨
		{

			m_pPlayer->Add_Dmg_to_Player(1);
			pConflictedCollider->Set_Conflicted();

		}


	}
	break;
	case Engine::CollisionType_Terrain:
		break;

	default:
		break;
	}


}

HRESULT CSlidePad::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));

	_Matrix WorldMatrix = XMMatrixRotationX(XMConvertToRadians(m_tDesc.vAngle.x))*
		XMMatrixRotationY(XMConvertToRadians(m_tDesc.vAngle.y))*
		XMMatrixRotationZ(XMConvertToRadians(m_tDesc.vAngle.z))*
		XMMatrixTranslation(m_tDesc.vPosition.x, m_tDesc.vPosition.y, m_tDesc.vPosition.z);

	m_pTransformCom->Set_Matrix(WorldMatrix);


	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_Slide), TAG_COM(Com_Model), (CComponent**)&m_pModel));


	ZeroMemory(m_NevRectPoint, sizeof(_float4) * 4);



	m_NevRectPoint[0] = _float4(0, 0, 0, 1);
	m_NevRectPoint[1] = _float4(-21.3f, 0, 0, 1);
	m_NevRectPoint[2] = _float4(-21.3f, 0, 4.5f, 1);
	m_NevRectPoint[3] = _float4(0, 0, 4.5f, 1);


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(25.f, 1.0f, 1.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-10.65f, 0, 2.25f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));


	ColliderDesc.vScale = _float3(0.4f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-3.1f, 0.3f, 2.6f,1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();
	ColliderDesc.vPosition = _float4(-4.6f, 0.3f, 3.7f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();
	ColliderDesc.vPosition = _float4(-6.8f, 0.3f, 1.6f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();
	ColliderDesc.vPosition = _float4(-9.4f, 0.3f, 3.7f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();
	ColliderDesc.vPosition = _float4(-10.f, 0.3f, 1.4f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();
	ColliderDesc.vPosition = _float4(-10.6f, 0.3f, 2.9f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();
	ColliderDesc.vPosition = _float4(-14.6f, 0.3f, 2.f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();
	ColliderDesc.vPosition = _float4(-16.7f, 0.3f, 3.2f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();
	ColliderDesc.vPosition = _float4(-18.9f, 0.3f, 1.4f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();



	if (m_tDesc.bStartCollider)
	{
		ColliderDesc.vScale = _float3(2.f, 1.0f, 4.5f);
		ColliderDesc.vPosition = _float4(-1.f, 0.5f, 2.25f, 1);
		m_iStartPointColliderIndex = m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc);
		m_pColliderCom->Set_ParantBuffer();
	}

	if (m_tDesc.bEndCollider)
	{
		ColliderDesc.vScale = _float3(1.f, 1.0f, 4.5f);
		ColliderDesc.vPosition = _float4(-22.3f, 0.5f, 2.25f, 1);
		m_iEndPointColliderIndex = m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc);
		m_pColliderCom->Set_ParantBuffer();
	}






	//ColliderDesc.vScale = _float3(5.2f, 2.7f, 5.2f);
	//ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	//ColliderDesc.vPosition = _float4(-2.5f, 1.6f, -2.5f, 1);
	//FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_AABB, &ColliderDesc));
	//m_pColliderCom->Set_ParantBuffer();





	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);



	return S_OK;
}


CSlidePad * CSlidePad::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CSlidePad*	pInstance = new CSlidePad(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CSlidePad");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSlidePad::Clone(void * pArg)
{
	CSlidePad*	pInstance = new CSlidePad(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CSlidePad");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSlidePad::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
