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


	m_vColor = _float4(0, 0, 0, 0.0f);
	m_fDepth = -FLT_MAX;

	m_bNeedToDraw = true;
	//m_PassIndex = 22;
	m_PassIndex = 7;
	
	m_iTextureLayerIndex = 0;

	return S_OK;
}

_int CFadeEffect::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	if (!m_bNeedToDraw || m_PassedTime > m_TotalTime) return 0;


	switch (m_eKindsFade)
	{
	case Client::CFadeEffect::FadeID_FadeIn:
	{
		m_PassedTime += fDeltaTime;

		_float EasedAlpha = g_pGameInstance->Easing(TYPE_SinOut, m_vTargetColor.w, 0, (_float)m_PassedTime, (_float)m_TotalTime);

		if (m_PassedTime > m_TotalTime)
		{
			EasedAlpha = 0;
			m_bNeedToDraw = false;
		}


		m_vColor.w = EasedAlpha;
	}
	break;
	case Client::CFadeEffect::FadeID_FadeOut:
	{
		m_PassedTime += fDeltaTime;

		_float EasedAlpha = g_pGameInstance->Easing(TYPE_Linear, 0, m_vTargetColor.w, (_float)m_PassedTime, (_float)m_TotalTime);

		if (m_PassedTime > m_TotalTime)
		{
			EasedAlpha = 1;
		}


		m_vColor.w = EasedAlpha;
	}
		break;
	case Client::CFadeEffect::FadeID_FadeInOut:
	{
		m_PassedTime += fDeltaTime;

		_float EasedAlpha = g_pGameInstance->Easing_Return(TYPE_QuadOut, TYPE_QuadIn, m_vTargetColor.w, 0, (_float)m_PassedTime, (_float)m_TotalTime);

		if (m_PassedTime > m_TotalTime)
		{
			EasedAlpha = 1;
		}

		m_vColor.w = EasedAlpha;
	}

		break;
	case Client::CFadeEffect::FadeID_FadeOutIn:
	{
		m_PassedTime += fDeltaTime;

		_float EasedAlpha = g_pGameInstance->Easing_Return(TYPE_QuadOut, TYPE_QuadIn, 0, m_vTargetColor.w, (_float)m_PassedTime, (_float)m_TotalTime);

		if (m_PassedTime > m_TotalTime)
		{
			m_bNeedToDraw = false;
			EasedAlpha = 0;
		}

		m_vColor.w = EasedAlpha;
	}
		break;

	case Client::CFadeEffect::FadeID_FlikeringIn:
	{
		m_PassedTime += fDeltaTime;

		_float EasedAlpha = g_pGameInstance->Easing(TYPE_BounceIn, 0, m_vTargetColor.w, (_float)m_PassedTime, (_float)m_TotalTime);
		m_vColor.w = 1;


		if (m_PassedTime > m_TotalTime*0.8f)
			m_vColor.w = g_pGameInstance->Easing(TYPE_SinOut, 1, 0, (_float)m_PassedTime - (_float)m_TotalTime*0.8f, (_float)m_TotalTime * 0.2f);

		if (m_PassedTime > m_TotalTime)
		{
			m_bNeedToDraw = false;
			EasedAlpha = 0;
			m_vColor.w = 0;
		}

		m_fAlphaTestValue = EasedAlpha;
	}
	break;
	default:
		break;
	}


	m_vColor.w = min(max(m_vColor.w, 0),1);


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
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_fAlphaTestValue", &m_fAlphaTestValue, sizeof(_float)));
		
		FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iTextureLayerIndex));
		FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, m_PassIndex));

	}
	return _int();
}

_int CFadeEffect::LightRender()
{



	return _int();
}

_bool CFadeEffect::Start_FadeEffect(FadeID eFadeType, _double Duration, _float4 TargetColor)
{
	if (m_PassedTime < m_TotalTime) return false;

	m_bNeedToDraw = true;
	m_eKindsFade = eFadeType;

	m_PassedTime = 0;
	m_TotalTime = Duration;
	m_vColor = m_vTargetColor = TargetColor;


	m_vTargetColor.x = min(max(m_vTargetColor.x, 0), 1);
	m_vTargetColor.y = min(max(m_vTargetColor.y, 0), 1);
	m_vTargetColor.z = min(max(m_vTargetColor.z, 0), 1);
	m_vTargetColor.w = min(max(m_vTargetColor.w, 0), 1);


	if (m_eKindsFade == CFadeEffect::FadeID_FlikeringIn)
	{
		m_PassIndex = 22;
		m_fAlphaTestValue = 0.f;		
		m_vColor.w = 1;

	}
	else
	{
		
		m_fAlphaTestValue = 0.1f;
		m_PassIndex = 7;
	}

	return true;
}

void CFadeEffect::Add_RenderGroup_ForSceneChanging()
{

	if (m_bNeedToDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

void CFadeEffect::Chage_TextureIndex(_uint iIndex)
{
	if (m_iTextureLayerIndex > 6) return;

	m_iTextureLayerIndex = iIndex;
}


HRESULT CFadeEffect::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VT), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_VIBuffer_Rect), TAG_COM(Com_VIBuffer), (CComponent**)&m_pVIBufferCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Texture_ScreenEffectUI), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));
	m_pTextureCom->Change_TextureLayer(L"ScreenEffect");
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
