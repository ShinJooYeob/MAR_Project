#include "stdafx.h"
#include "..\public\FadeEffect.h"
#include "FadeEffect.h"



CFadeEffect::CFadeEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI(pDevice,pDeviceContext)
{
}

CFadeEffect::CFadeEffect(const CFadeEffect & rhs)
	:CUI(rhs)
{

}

HRESULT CFadeEffect::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));

	return S_OK;
}

HRESULT CFadeEffect::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());


	m_UIRect.left = 0;
	m_UIRect.top = 0;
	m_UIRect.right = g_iWinCX;
	m_UIRect.bottom = g_iWinCY;


	m_vColor = _float4(0, 0, 0, 0.5f);
	m_fDepth = -FLT_MAX;

	m_bNeedToDraw = true;

	return S_OK;
}

_int CFadeEffect::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;




	return _int();
}

_int CFadeEffect::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;


	if (m_bNeedToDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	return _int();
}

_int CFadeEffect::Render()
{
	if (__super::Render( ) < 0)
		return -1;

	if (m_bIsDraw)
	{
		NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);
		FAILED_CHECK(Apply_Rect_To_Transform());

		FAILED_CHECK(Bind_Transform_OnShader(m_pShaderCom, "g_WorldMatrix"));
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4)));
		FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iTextureLayerIndex));
		FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, m_PassIndex));

	}
	return _int();
}

_int CFadeEffect::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;


	return _int();
}

_bool CFadeEffect::Start_FadeEffect(FadeID eFadeType, _double Duration, _float4 TargetColor)
{
	return _bool();
}


HRESULT CFadeEffect::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VT), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_VIBuffer_Rect), TAG_COM(Com_VIBuffer), (CComponent**)&m_pVIBufferCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Texture_ScreenEffectUI), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));

	CTransform::TRANSFORMDESC tDesc = {};
	tDesc.fMovePerSec = 5;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	m_pTransformCom->Set_TransformDesc(tDesc);

	return S_OK;
}

CFadeEffect * CFadeEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CFadeEffect*	pInstance = new CFadeEffect(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CFadeEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFadeEffect::Clone(void * pArg)
{
	CFadeEffect*	pInstance = new CFadeEffect(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CFadeEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFadeEffect::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);


}
