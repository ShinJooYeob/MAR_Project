#include "stdafx.h"
#include "..\public\RadialBlurMonster.h"



CRadialBlurMonster::CRadialBlurMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI(pDevice,pDeviceContext)
{
}

CRadialBlurMonster::CRadialBlurMonster(const CRadialBlurMonster & rhs)
	:CUI(rhs)
{

}

HRESULT CRadialBlurMonster::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));

	return S_OK;
}

HRESULT CRadialBlurMonster::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());


	m_UIRect.left = 0;
	m_UIRect.top = 0;
	m_UIRect.right = g_iWinCX;
	m_UIRect.bottom = g_iWinCY;

	m_vColor = _float4(0, 0, 0, 0.0f);
	m_fDepth = -FLT_MAX;

	m_bNeedToDraw = false;
	m_PassIndex = 12;
	m_iTextureLayerIndex = 0;

	return S_OK;
}

_int CRadialBlurMonster::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	if (m_PassedTime < 1)
	{
		if (m_bIsOn)
		{
			m_PassedTime += fDeltaTime;

			m_ZoomSize = g_pGameInstance->Easing(TYPE_Linear, 1, 0.25f,(_float)m_PassedTime, 1);
			m_ZoomPower = g_pGameInstance->Easing(TYPE_Linear, 0, 0.7f,(_float)m_PassedTime, 1);

		}
		else
		{

			m_PassedTime += fDeltaTime;

			m_ZoomSize = g_pGameInstance->Easing(TYPE_Linear, 0.25f, 1.f, (_float)m_PassedTime, 1);
			m_ZoomPower = g_pGameInstance->Easing(TYPE_Linear, 0.7f, 0.f, (_float)m_PassedTime, 1);

			if (m_PassedTime >= 1)
			{
				m_bNeedToDraw = false;
			}
		}

	}


	return _int();
}

_int CRadialBlurMonster::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;


	if (m_bNeedToDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_POSTPROCESSING, this);


	return _int();
}

_int CRadialBlurMonster::Render()
{
	if (__super::Render() < 0)
		return -1;

	CGameInstance* pInstance = g_pGameInstance;

	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);
	FAILED_CHECK(Apply_Rect_To_Transform());

	FAILED_CHECK(Bind_Transform_OnShader(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vCenter", &m_TargetPos, sizeof(_float2)));

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_fZoomPower", &m_ZoomPower, sizeof(_float)));

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_fZoomSize", &m_ZoomSize, sizeof(_float)));

	
	FAILED_CHECK(pInstance->Begin_MTV(L"MRT_SubVeiwPortA"));
	FAILED_CHECK(m_pShaderCom->Set_Texture("g_BlurTargetTexture", pInstance->Get_SRV(L"Target_ReferenceDefferred")));
	FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, 16));
	FAILED_CHECK(pInstance->End_MTV(L"MRT_SubVeiwPortA"));

	FAILED_CHECK(pInstance->Begin_MTV(TEXT("MRT_AfterPostProcessing")));

	FAILED_CHECK(m_pShaderCom->Set_Texture("g_DiffuseTexture", pInstance->Get_SRV(L"Target_SubVeiwPortA")));
	FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, 15));

	FAILED_CHECK(pInstance->End_MTV(TEXT("MRT_AfterPostProcessing")));

	return _int();
}

_int CRadialBlurMonster::LightRender()
{



	return _int();
}



void CRadialBlurMonster::Chage_TextureIndex(_uint iIndex)
{
	if (m_iTextureLayerIndex > 6) return;

	m_iTextureLayerIndex = iIndex;
}

void CRadialBlurMonster::Set_TargetPos(_float3 vPos)
{
	CGameInstance* pInstace = g_pGameInstance;

	_float3 ProjSpacePos = XMVector3TransformCoord(vPos.Multiply_Matrix_AsPosVector(pInstace->Get_Transform_Matrix(PLM_VIEW)), pInstace->Get_Transform_Matrix(PLM_PROJ));

	m_TargetPos = _float2(ProjSpacePos.x * 0.5f + 0.5f, ProjSpacePos.y * -0.5f + 0.5f);

}

void CRadialBlurMonster::Set_IsOn(_bool bBool)
{
	if (m_bIsOn == bBool)return;

	m_bIsOn = bBool;
	m_bNeedToDraw = true;
	m_PassedTime = 0;

}


HRESULT CRadialBlurMonster::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VT), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_VIBuffer_Rect), TAG_COM(Com_VIBuffer), (CComponent**)&m_pVIBufferCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Texture_ScreenEffectUI), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));
	m_pTextureCom->Change_TextureLayer(L"BlurEffect");
	CTransform::TRANSFORMDESC tDesc = {};
	tDesc.fMovePerSec = 5;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	m_pTransformCom->Set_TransformDesc(tDesc);

	return S_OK;
}

CRadialBlurMonster * CRadialBlurMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CRadialBlurMonster*	pInstance = new CRadialBlurMonster(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CRadialBlurMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRadialBlurMonster::Clone(void * pArg)
{
	CRadialBlurMonster*	pInstance = new CRadialBlurMonster(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CRadialBlurMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRadialBlurMonster::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);


}
