#include "stdafx.h"
#include "..\public\RendererEditSceneUI.h"



CRendererEditSceneUI::CRendererEditSceneUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CRendererEditSceneUI::CRendererEditSceneUI(const CRendererEditSceneUI & rhs)
	:CGameObject(rhs)
{
}

HRESULT CRendererEditSceneUI::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CRendererEditSceneUI::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());

	return S_OK;
}

_int CRendererEditSceneUI::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	return _int();
}

_int CRendererEditSceneUI::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this));
	return 0;
}

_int CRendererEditSceneUI::Render()
{
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);


	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	CGameInstance* pInstance = GetSingle(CGameInstance);

	for (auto& EditUIDesc : *m_pVecEditUI)
	{
		if (EditUIDesc.SRV == nullptr)
		{
			__debugbreak();
			return E_FAIL;
		}

		if (EditUIDesc.bIsRect)
		{
			FAILED_CHECK(Apply_Rect_To_Transform(EditUIDesc.UIRect));
		}
		else
		{
			FAILED_CHECK(Apply_UIDesc_To_Transform(EditUIDesc.UIDesc));
		}

		if(EditUIDesc.fAngle != 0)
			m_pTransformCom->Rotation_CW(XMVectorSet(0, 0, 1, 0), XMConvertToRadians(EditUIDesc.fAngle));


		FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

		//m_pShaderCom->Set_Texture("g_DiffuseTexture", EditUIDesc.SRV);
		FAILED_CHECK(m_pShaderCom->Set_Texture("g_DiffuseTexture", EditUIDesc.SRV));

		FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, 0));
	}






	return _int();
}

_int CRendererEditSceneUI::LateRender()
{



	return _int();
}

void CRendererEditSceneUI::Set_VecEditUI(vector<EDITUI>* pVec)
{
	m_pVecEditUI = pVec;
}

HRESULT CRendererEditSceneUI::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VT), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_VIBuffer_Rect), TAG_COM(Com_VIBuffer), (CComponent**)&m_pVIBufferCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));

	return S_OK;
}




HRESULT CRendererEditSceneUI::Apply_UIDesc_To_Transform(UIDESC tDesc)
{
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);

	m_pTransformCom->Set_Matrix(XMMatrixIdentity());

	m_pTransformCom->Scaled(CTransform::STATE_RIGHT, tDesc.fCX);
	m_pTransformCom->Scaled(CTransform::STATE_UP, tDesc.fCY);

	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(tDesc.fX - g_iWinCX * 0.5f, -tDesc.fY + g_iWinCY * 0.5f, 0.f));

	return S_OK;
}

HRESULT CRendererEditSceneUI::Apply_Rect_To_Transform(FLOATRECT tRect)
{
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);

	m_pTransformCom->Set_Matrix(XMMatrixIdentity());


	m_pTransformCom->Scaled(CTransform::STATE_RIGHT, tRect.right - tRect.left);
	m_pTransformCom->Scaled(CTransform::STATE_UP, tRect.bottom - tRect.top);


	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS,
		_float3((tRect.right + tRect.left) * 0.5f - g_iWinCX * 0.5f,
			-(tRect.bottom + tRect.top) * 0.5f + g_iWinCY * 0.5f, 0.f));


	return S_OK;
}

CRendererEditSceneUI * CRendererEditSceneUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CRendererEditSceneUI*	pInstance = new CRendererEditSceneUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CRendererEditSceneUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRendererEditSceneUI::Clone(void * pArg)
{
	CRendererEditSceneUI*	pInstance = new CRendererEditSceneUI(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CGameObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRendererEditSceneUI::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
