#include "stdafx.h"
#include "..\public\RadialBlur.h"
#include "Player.h"



CRadialBlur::CRadialBlur(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI(pDevice,pDeviceContext)
{
}

CRadialBlur::CRadialBlur(const CRadialBlur & rhs)
	:CUI(rhs)
{

}

HRESULT CRadialBlur::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));

	return S_OK;
}

HRESULT CRadialBlur::Initialize_Clone(void * pArg)
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
	m_PassIndex = 12;
	m_iTextureLayerIndex = 0;

	return S_OK;
}

_int CRadialBlur::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;



	CPlayer* pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	if (!pPlayer || pPlayer->Get_DashPower() <= 0)
	{
		m_bNeedToDraw = false;
		return 0;
	}

	m_bNeedToDraw = true;
	m_DashPower = pPlayer->Get_DashPower() / PlayerMaxDashPower * 0.5f;
	

	return _int();
}

_int CRadialBlur::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;


	if (m_bNeedToDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_POSTPROCESSING, this);

	return _int();
}

_int CRadialBlur::Render()
{
	if (__super::Render() < 0)
		return -1;

	CGameInstance* pInstance = g_pGameInstance;

	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);
	FAILED_CHECK(Apply_Rect_To_Transform());

	FAILED_CHECK(Bind_Transform_OnShader(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	_float2 TempFloat2 = _float2(0.5f, 0.5f);
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vCenter", &TempFloat2, sizeof(_float2)));

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_fZoomPower", &m_DashPower, sizeof(_float)));
	m_DashPower = 0.25f;
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_fZoomSize", &m_DashPower, sizeof(_float)));

	
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

_int CRadialBlur::LightRender()
{



	return _int();
}



void CRadialBlur::Chage_TextureIndex(_uint iIndex)
{
	if (m_iTextureLayerIndex > 6) return;

	m_iTextureLayerIndex = iIndex;
}


HRESULT CRadialBlur::SetUp_Components()
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

CRadialBlur * CRadialBlur::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CRadialBlur*	pInstance = new CRadialBlur(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CRadialBlur");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRadialBlur::Clone(void * pArg)
{
	CRadialBlur*	pInstance = new CRadialBlur(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CRadialBlur");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRadialBlur::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);


}
