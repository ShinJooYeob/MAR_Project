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




	return _int();
}

_int CUIImage::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	return _int();
}

_int CUIImage::Render()
{
	if (__super::Render( ) < 0)
		return -1;

	if (m_bIsDraw)
	{


		NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);

		if (m_bIsDrawByDesc)
		{
			FAILED_CHECK(Apply_UIDesc_To_Transform());

		}
		else
		{
			FAILED_CHECK(Apply_Rect_To_Transform());
		}

		FAILED_CHECK(Bind_Transform_OnShader(m_pShaderCom, "g_WorldMatrix"));

		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4)));
		
		if(m_PassIndex == 6)
			FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CutY", &m_fUICutY, sizeof(_float)));

			

		//FAILED_CHECK(m_pTextureCom->Bind_OnShader_AutoFrame(m_pShaderCom, "g_DiffuseTexture", g_fDeltaTime));
		if (m_bIsUntilTo)
		{
			m_fPassedTime += g_fDeltaTime;

			_uint iEasedIndex = 0;

			if (m_fPassedTime >= m_fTotalUntilTime)
			{
				iEasedIndex = m_iTextureLayerIndex = m_iTargetTextureLayerIndex;
				m_bIsUntilTo = false;
			}
			else
			{
				iEasedIndex = _uint(g_pGameInstance->Easing(TYPE_Linear, _float(m_iTextureLayerIndex), _float(m_iTargetTextureLayerIndex), _float(m_fPassedTime), _float(m_fTotalUntilTime)));
			}


			FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", iEasedIndex));


		}
		else
		{

			FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iTextureLayerIndex));
		}

		FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, m_PassIndex));

	}
	return _int();
}

_int CUIImage::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;


	return _int();
}

HRESULT CUIImage::Bind_Texture_OnShader(const char * szConstName, ID3D11ShaderResourceView * pSRV)
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	return m_pShaderCom->Set_Texture(szConstName, pSRV);
}


HRESULT CUIImage::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VT), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_VIBuffer_Rect), TAG_COM(Com_VIBuffer), (CComponent**)&m_pVIBufferCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Texture_DefaultUI), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));

	CTransform::TRANSFORMDESC tDesc = {};
	tDesc.fMovePerSec = 5;
	tDesc.fRotationPerSec = 1;
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
