#include "stdafx.h"
#include "..\public\HiddenPad.h"
#include "Player.h"



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
	

	return S_OK;
}

_int CHiddenPad::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;


	if (m_pPlayer->Get_SmallVisualTime())
		m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));
	else
		m_bIsOnScreen = false;
		
	return _int();
}

_int CHiddenPad::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	//if (!m_bIsPlayerCloser) return _int();


		if (m_bIsOnScreen)
			FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
		m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	
	return _int();
}

_int CHiddenPad::Render()
{
	if (__super::Render() < 0)
		return -1;

	//NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);


	//FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	//CGameInstance* pInstance = g_pGameInstance;

	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));

	//_float VisualValue = m_pPlayer->Get_SmallVisualTime();

	//FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture",2));


	//FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, 2));

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

HRESULT CHiddenPad::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));


	_uint iKindofMesh = min(Prototype_Mesh_GiantToys_Die + m_iKindsOfHiddenPad, Prototype_Mesh_GiantToys_DominoStair);
	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(COMPONENTPROTOTYPEID(iKindofMesh)), TAG_COM(Com_Model), (CComponent**)&m_pModel));


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));



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
