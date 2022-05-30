#include "stdafx.h"
#include "..\public\illusion.h"
#include "Player.h"



Cillusion::Cillusion(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI(pDevice,pDeviceContext)
{
}

Cillusion::Cillusion(const Cillusion & rhs)
	:CUI(rhs)
{

}

HRESULT Cillusion::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));

	return S_OK;
}

HRESULT Cillusion::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());


	m_UIRect.left = -g_iWinCX;
	m_UIRect.top = -g_iWinCY;
	m_UIRect.right = g_iWinCX*2.f;
	m_UIRect.bottom = g_iWinCY*2.f;

	//m_UIRect.left = 0;
	//m_UIRect.top = 0;
	//m_UIRect.right = g_iWinCX;
	//m_UIRect.bottom = g_iWinCY;

	m_vColor = _float4(0, 0, 0, 0.0f);
	m_fDepth = -FLT_MAX;

	m_bNeedToDraw = true;
	m_PassIndex = 12;
	m_iTextureLayerIndex = 0;

	return S_OK;
}

_int Cillusion::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;



	CPlayer* pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	if (!pPlayer || pPlayer->Get_SmallVisualTime() <= 0 )
	{
		m_bNeedToDraw = false;
		return 0;
	}
	m_bNeedToDraw = true;
	m_vColor = _float4(1,1,1, pPlayer->Get_SmallVisualTime() * 0.5f);
	m_fAngle += 30 * (_float)fDeltaTime;

	//m_vColor = _float4(1, 1, 1, 1.0f);
	return _int();
}

_int Cillusion::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;


	if (m_bNeedToDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	return _int();
}

_int Cillusion::Render()
{
	if (__super::Render() < 0)
		return -1;


	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);
	FAILED_CHECK(Apply_Rect_To_Transform());
	
	FAILED_CHECK(Bind_Transform_OnShader(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4)));
	FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iTextureLayerIndex));
	FAILED_CHECK(m_pShaderCom->Set_Texture("g_BackBufferTexture", g_pGameInstance->Get_SRV(L"Target_AfterDefferred")));
	FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, m_PassIndex));

	//NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);
	//FAILED_CHECK(Apply_Rect_To_Transform());

	//FAILED_CHECK(Bind_Transform_OnShader(m_pShaderCom, "g_WorldMatrix"));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4)));
	//FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_NoiseTexture", m_iTextureLayerIndex));
	//FAILED_CHECK(m_pShaderCom->Set_Texture("g_BlurTargetTexture", g_pGameInstance->Get_SRV(L"Target_AfterDefferred")));
	//FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, 13));

	return _int();
}

_int Cillusion::LightRender()
{



	return _int();
}



void Cillusion::Chage_TextureIndex(_uint iIndex)
{
	if (m_iTextureLayerIndex > 6) return;

	m_iTextureLayerIndex = iIndex;
}


HRESULT Cillusion::SetUp_Components()
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

Cillusion * Cillusion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	Cillusion*	pInstance = new Cillusion(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created Cillusion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * Cillusion::Clone(void * pArg)
{
	Cillusion*	pInstance = new Cillusion(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created Cillusion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Cillusion::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);


}
