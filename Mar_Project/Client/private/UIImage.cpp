#include "stdafx.h"
#include "..\public\UIImage.h"



CUIImage::CUIImage(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI(pDevice,pDeviceContext)
{
}

CUIImage::CUIImage(const CUIImage & rhs)
	:CUI(rhs)
{

}

HRESULT CUIImage::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));

	return S_OK;
}

HRESULT CUIImage::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());


	m_UIRect.left = 30;
	m_UIRect.top = 30;
	m_UIRect.right = 400;
	m_UIRect.bottom = 700;



	return S_OK;
}

_int CUIImage::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	if (g_pGameInstance->Get_DIKeyState(DIK_RETURN)&DIS_Down)
	{
		static int test = 0;
		test++;

		if (test > 2)
			test = 0;

		switch (test)
		{
		case 0:
			FAILED_CHECK(m_pTextureCom->Change_TextureLayer(L"lower"));
			break;
		case 1:
			FAILED_CHECK(m_pTextureCom->Change_TextureLayer(L"upper"));
			break;
		case 2:
			FAILED_CHECK(m_pTextureCom->Change_TextureLayer(L"number"));
			break;
		default:
			break;
		}
	}

	return _int();
}

_int CUIImage::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;
	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this));

	return _int();
}

_int CUIImage::Render()
{
	if (__super::Render( ) < 0)
		return -1;

	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);


	FAILED_CHECK(Apply_Rect_To_Transform());
	FAILED_CHECK(Bind_Transform_OnShader(m_pShaderCom, "g_WorldMatrix"));

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	FAILED_CHECK(m_pTextureCom->Bind_OnShader_AutoFrame(m_pShaderCom, "g_DiffuseTexture", g_fDeltaTime));

	FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, 0));

	return _int();
}

_int CUIImage::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;


	return _int();
}


HRESULT CUIImage::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VT), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_VIBuffer_Rect), TAG_COM(Com_VIBuffer), (CComponent**)&m_pVIBufferCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Texture_Player), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));

	CTransform::TRANSFORMDESC tDesc = {};
	tDesc.fMovePerSec = 5;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	m_pTransformCom->Set_TransformDesc(tDesc);

	return S_OK;
}

CUIImage * CUIImage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CUIImage*	pInstance = new CUIImage(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CUIImage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUIImage::Clone(void * pArg)
{
	CUIImage*	pInstance = new CUIImage(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CUIImage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIImage::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);

}
