#include "stdafx.h"
#include "..\public\HiddenPad.h"
#include "Player.h"
#include "Terrain.h"



CHiddenPad::CHiddenPad(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CHiddenPad::CHiddenPad(const CHiddenPad & rhs)
	: CMapObject(rhs)
{
}

HRESULT CHiddenPad::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CHiddenPad::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		m_iKindsOfHiddenPad = _uint((*(_float4*)pArg).w);
	}
	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
	{
		_float3 Pos = *(_float3*)pArg;
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, Pos);
	}
	
	m_pTransformCom->Scaled_All(_float3(2, 2, 2));

	FAILED_CHECK(Set_TerrainTileKinds());

	return S_OK;
}

_int CHiddenPad::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;





	if (m_pPlayer->Get_SmallVisualTime())
		m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS),20);
	else
		m_bIsOnScreen = false;
	
	return _int();
}

_int CHiddenPad::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;



	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CHiddenPad::Render()
{
	if (__super::Render() < 0)
		return -1;






	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader_ApplyPivot(m_pShaderCom, "g_WorldMatrix"));


	FAILED_CHECK(__super::SetUp_ConstTable(m_pShaderCom));


	_float VisualValue = m_pPlayer->Get_SmallVisualTime();

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_fVisualValue", &(VisualValue), sizeof(_float)));

	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 4, i));
	}


	return _int();
}

_int CHiddenPad::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

	return _int();
}

HRESULT CHiddenPad::Set_TerrainTileKinds()
{
	CTerrain* pTerrain = (CTerrain*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));
	NULL_CHECK_RETURN(pTerrain, E_FAIL);



	_Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	_Matrix RectMatrix = XMMatrixIdentity();

	RectMatrix.r[0] = m_NevRectPoint[0].XMVector();
	RectMatrix.r[1] = m_NevRectPoint[1].XMVector();
	RectMatrix.r[2] = m_NevRectPoint[2].XMVector();
	RectMatrix.r[3] = m_NevRectPoint[3].XMVector();


	_Matrix WorldRectPoints = RectMatrix * WorldMatrix;

	FAILED_CHECK(pTerrain->Chage_TileKindsNHeight(WorldRectPoints));



	return S_OK;
}

HRESULT CHiddenPad::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));

	_uint iKindofMesh = min(Prototype_Mesh_GiantToys_Die + m_iKindsOfHiddenPad, Prototype_Mesh_GiantToys_DominoStair);
	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(COMPONENTPROTOTYPEID(iKindofMesh)), TAG_COM(Com_Model), (CComponent**)&m_pModel));


	ZeroMemory(m_NevRectPoint, sizeof(_float3) * 3);
	switch (m_iKindsOfHiddenPad)
	{
	case 0:
		m_NevRectPoint[0] = _float4(-0.779999f, 1.619999f, -0.839999f, 1);
		m_NevRectPoint[1] = _float4(0.779999f, 1.619999f, -0.839999f, 1);
		m_NevRectPoint[2] = _float4(0.779999f, 1.619999f, 0.779999f, 1);
		m_NevRectPoint[3] = _float4(-0.779999f, 1.619999f, 0.779999f, 1);

		break;

	case 1:
		m_NevRectPoint[0] = _float4(-0.6f, 0.3f, 0, 1);
		m_NevRectPoint[1] = _float4(0.6f, 0.3f, 0, 1);
		m_NevRectPoint[2] = _float4(0.6f, 0.3f, 2.34f, 1);
		m_NevRectPoint[3] = _float4(-0.6f, 0.3f, 2.34f, 1);
		break;

	case 2:
		m_NevRectPoint[0] = _float4(-0.6f, 0.3f, 0, 1);
		m_NevRectPoint[1] = _float4(0.6f, 0.3f, 0, 1);
		m_NevRectPoint[2] = _float4(0.6f, 0.3f, 2.34f, 1);
		m_NevRectPoint[3] = _float4(-0.6f, 0.3f, 2.34f, 1);
		break;

	case 3:
		m_NevRectPoint[0] = _float4(-1.2f, 0.48f, -0.6f, 1);
		m_NevRectPoint[1] = _float4(1.14f, 0.48f, -0.6f, 1);
		m_NevRectPoint[2] = _float4(1.14f, 3.84f, 8.52f, 1);
		m_NevRectPoint[3] = _float4(-1.2f, 3.84f, 8.52f, 1);
		break;

	default:
		break;
	}






	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);



	return S_OK;
}

CHiddenPad * CHiddenPad::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CHiddenPad*	pInstance = new CHiddenPad(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CHiddenPad");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHiddenPad::Clone(void * pArg)
{
	CHiddenPad*	pInstance = new CHiddenPad(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CHiddenPad");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHiddenPad::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pShaderCom);




}
