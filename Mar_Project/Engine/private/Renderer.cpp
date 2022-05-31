#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "Camera.h"

#include "RenderTargetMgr.h"
#include "LightMgr.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "SwordTrail.h"

#include "GameInstance.h"
#include "PipeLineMgr.h"
#include "Graphic_Device.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_pRenderTargetMgr(CRenderTargetMgr::GetInstance())
	, m_pLightMgr(CLightMgr::GetInstance())
	, m_pGraphicDevice(CGraphic_Device::GetInstance())
{
	Safe_AddRef(m_pRenderTargetMgr);
	Safe_AddRef(m_pLightMgr);
	Safe_AddRef(m_pGraphicDevice);

}
#define ShadowMapQuality 8

HRESULT CRenderer::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));



	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;

	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	/* For.Target_Diffuse */
	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget( TEXT("Target_Diffuse"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	/* For.Target_Normal */
	/* 노멀벡터의 경우, 정규화하기가 쉽다. */
	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget(TEXT("Target_Normal"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f)));

	/* For.Target_Depth */
	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget(TEXT("Target_Depth"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 0.f, 0.f, 1.f)));

	/* For.Target_Specular */
	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget(TEXT("Target_MtrlSpecularMap"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 1.f, 0.f)));
	
	/* For.Target_Shade */
	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget(TEXT("Target_Shade"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f)));

	/* For.Target_Specular */
	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget(TEXT("Target_Specular"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));

	/* For.Target_Shadow*/
	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget(TEXT("Target_Shadow"), (_uint)Viewport.Width * ShadowMapQuality, (_uint)Viewport.Height * ShadowMapQuality, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget(TEXT("Target_DownScaledBluredShadow"), (_uint)(Viewport.Width * 0.5f), (_uint)(Viewport.Height * 0.5f), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget(TEXT("Target_ReferenceShadow"), (_uint)(Viewport.Width * 0.5f), (_uint)(Viewport.Height * 0.5f), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget(TEXT("Target_UpScaledBluredShadow"), (_uint)Viewport.Width , (_uint)Viewport.Height , DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));

	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget(TEXT("Target_AfterDefferred"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget(TEXT("Target_ReferenceDefferred"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));

	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget(TEXT("Target_SubVeiwPortA"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget(TEXT("Target_SubVeiwPortB"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));



	/* For.MRT_Deferred : 객체들을 그릴때 바인드. */
	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse")));
	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal")));
	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth")));
	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_MtrlSpecularMap")));
	

	/* For.MRT_LightAcc : 빛을 그릴때 바인드 */
	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade")));
	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular")));

	/* For.MRT_LightAcc : 그림자 그릴때 바인드 */
	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_Shadow")));
	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_ShadowDownScaling"), TEXT("Target_DownScaledBluredShadow")));
	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_ShadowDownScaling"), TEXT("Target_ReferenceShadow")));

	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_ShadowUpScaling"), TEXT("Target_UpScaledBluredShadow")));

	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_ShadowBluringVerticle"), TEXT("Target_ReferenceShadow")));
	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_ShadowBluringHorizon"), TEXT("Target_DownScaledBluredShadow")));


	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_AfterDefferred"), TEXT("Target_AfterDefferred")));
	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_AfterDefferred"), TEXT("Target_ReferenceDefferred")));

	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_AfterPostProcessing"), TEXT("Target_AfterDefferred")));

	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_SubVeiwPortA"), TEXT("Target_SubVeiwPortA")));
	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_SubVeiwPortB"), TEXT("Target_SubVeiwPortB")));


	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext);
	NULL_CHECK_RETURN(m_pVIBuffer, E_FAIL);

	m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, TEXT("Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	NULL_CHECK_RETURN(m_pShader, E_FAIL);



#ifdef _DEBUG

	FAILED_CHECK(m_pRenderTargetMgr->Ready_DebugDesc(TEXT("Target_Diffuse"), 50, 50, 100, 100));
	FAILED_CHECK(m_pRenderTargetMgr->Ready_DebugDesc(TEXT("Target_Normal"), 50, 150, 100, 100));
	FAILED_CHECK(m_pRenderTargetMgr->Ready_DebugDesc(TEXT("Target_Depth"), 50, 250, 100, 100));
	FAILED_CHECK(m_pRenderTargetMgr->Ready_DebugDesc(TEXT("Target_MtrlSpecularMap"), 50, 350, 100, 100));

	FAILED_CHECK(m_pRenderTargetMgr->Ready_DebugDesc(TEXT("Target_Shade"), 150, 50, 100, 100));
	FAILED_CHECK(m_pRenderTargetMgr->Ready_DebugDesc(TEXT("Target_Specular"), 150, 150, 100, 100));

	FAILED_CHECK(m_pRenderTargetMgr->Ready_DebugDesc(TEXT("Target_UpScaledBluredShadow"), 50, 680, 100, 100));
	FAILED_CHECK(m_pRenderTargetMgr->Ready_DebugDesc(TEXT("Target_AfterDefferred"), 150, 680, 100, 100));

#endif


	XMStoreFloat4x4(&m_WVPmat.WorldMatrix, XMMatrixIdentity());

	m_WVPmat.WorldMatrix._11 = Viewport.Width;
	m_WVPmat.WorldMatrix._22 = Viewport.Height;
	m_WVPmat.WorldMatrix._33 = 1.f;
	memcpy(&m_WVPmat.WorldMatrix.m[3][0], &_float4(Viewport.Width * 0.5f - (Viewport.Width * 0.5f), -Viewport.Height * 0.5f + (Viewport.Height * 0.5f), 0.f, 1.f), sizeof(_float4));

	XMStoreFloat4x4(&m_WVPmat.WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WVPmat.WorldMatrix)));
	XMStoreFloat4x4(&m_WVPmat.ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_WVPmat.ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f)));

	//////////////////////////////////////////////////////////////////////////


	m_LightWVPmat.ViewMatrix = XMMatrixTranspose(XMMatrixLookAtLH(XMVectorSet(0,256,-128,1), XMVectorSet(128,0,128,1), XMVectorSet(0, 1, 0, 0)));
	m_LightWVPmat.ProjMatrix = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 1, 0.2f, 3000));


	ID3D11Texture2D*		pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = (_uint)Viewport.Width * ShadowMapQuality;
	TextureDesc.Height = (_uint)Viewport.Height * ShadowMapQuality;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	FAILED_CHECK(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture));
	FAILED_CHECK(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_LightDepthStencil));
	Safe_Release(pDepthStencilTexture);



	TextureDesc.Width = (_uint)(Viewport.Width * 0.5f);
	TextureDesc.Height = (_uint)(Viewport.Height * 0.5f);
	FAILED_CHECK(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture));
	FAILED_CHECK(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_DownScaledDepthStencil));
	Safe_Release(pDepthStencilTexture);
	




	return S_OK;
}

HRESULT CRenderer::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));



	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderID, CGameObject * pGameObject)
{
	if (eRenderID >= RENDER_END ||
		pGameObject == nullptr)
	{
		__debugbreak();
		return E_FAIL;
	}

	if (eRenderID == CRenderer::RENDER_BLEND)
		pGameObject->Compute_RenderSortValue();

	m_RenderObjectList[eRenderID].emplace_back(pGameObject);
	
	Safe_AddRef(pGameObject);


	return S_OK;
}

HRESULT CRenderer::Add_TrailGroup(CSwordTrail * pComponent)
{
	m_TrailObjectList.push_back(pComponent);
	Safe_AddRef(pComponent);

	return S_OK;
}

HRESULT CRenderer::Add_DebugGroup(CComponent * pComponent)
{
	m_DebugObjectList.push_back(pComponent);
	Safe_AddRef(pComponent);

	return S_OK;
}

HRESULT CRenderer::Render_RenderGroup()
{
	FAILED_CHECK(m_pRenderTargetMgr->Clear_All_RenderTargetColor());


	FAILED_CHECK(Render_ShadowMap());

	FAILED_CHECK(m_pRenderTargetMgr->Begin(TEXT("MRT_Deferred")));
	FAILED_CHECK(Render_Priority());
	FAILED_CHECK(Render_NonAlpha());
	FAILED_CHECK(Render_PriorityAlpha());
	FAILED_CHECK(m_pRenderTargetMgr->End(TEXT("MRT_Deferred")));

	FAILED_CHECK(Render_BlurShadow());
	FAILED_CHECK(Render_Lights());

	FAILED_CHECK(m_pRenderTargetMgr->Begin(TEXT("MRT_AfterDefferred")));
	FAILED_CHECK(Render_DeferredTexture());
	FAILED_CHECK(m_pRenderTargetMgr->End(TEXT("MRT_AfterDefferred")));

	FAILED_CHECK(Render_PostProcessing());


	FAILED_CHECK(Render_NonAlpha_NoDeferrd());

	FAILED_CHECK(Render_Alpha());
	FAILED_CHECK(Render_AfterObj());


	FAILED_CHECK(Render_UI());


#ifdef _DEBUG
	static bool tt = false;
	if (GetSingle(CGameInstance)->Get_DIKeyState(DIK_M) &DIS_Down)
	{
		tt = !tt;
	}
	if (tt)
	{
		FAILED_CHECK(Render_Debug());
		FAILED_CHECK(m_pRenderTargetMgr->Render_DebugBuffer(TEXT("MRT_Deferred")));
		FAILED_CHECK(m_pRenderTargetMgr->Render_DebugBuffer(TEXT("MRT_LightAcc")));
		FAILED_CHECK(m_pRenderTargetMgr->Render_DebugBuffer(TEXT("MRT_ShadowUpScaling")));
		FAILED_CHECK(m_pRenderTargetMgr->Render_DebugBuffer(TEXT("MRT_AfterPostProcessing")));

		


		ID3D11ShaderResourceView* pSRV[8] = { nullptr };
		m_pDeviceContext->PSSetShaderResources(0, 8, pSRV);

	}

#endif

	return S_OK;
}


HRESULT CRenderer::Clear_RenderGroup_forSceneChaging()
{
	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for (auto& RenderObject : m_RenderObjectList[i])
		{
			Safe_Release(RenderObject);
		}
		m_RenderObjectList[i].clear();
	}

	for (auto& DebugObject : m_DebugObjectList)
	{
		Safe_Release(DebugObject);
	}
	m_DebugObjectList.clear();

	for (auto& TrailObject : m_TrailObjectList)
	{
		Safe_Release(TrailObject);
	}
	m_TrailObjectList.clear();


	return S_OK;
}





HRESULT CRenderer::Render_Priority()
{
	for (auto& RenderObject : m_RenderObjectList[RENDER_PRIORITY])
	{
		if (RenderObject != nullptr)
		{
			FAILED_CHECK(RenderObject->Render());
		}
		Safe_Release(RenderObject);
	}
	m_RenderObjectList[RENDER_PRIORITY].clear();
	return S_OK;
}

HRESULT CRenderer::Render_ShadowMap()
{

	m_pDeviceContext->ClearDepthStencilView(m_LightDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);


	FAILED_CHECK(m_pRenderTargetMgr->Begin(TEXT("MRT_Shadow"), m_LightDepthStencil));

	_uint		iNumViewports = 1;
	D3D11_VIEWPORT		OldViewPortDesc;
	m_pDeviceContext->RSGetViewports(&iNumViewports, &OldViewPortDesc);

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)OldViewPortDesc.Width * ShadowMapQuality;
	ViewPortDesc.Height = (_float)OldViewPortDesc.Height * ShadowMapQuality;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(iNumViewports, &ViewPortDesc);

	CShader* ObjShader = nullptr;
	

	for (auto& RenderObject : m_RenderObjectList[RENDER_NONBLEND])
	{
		if (RenderObject != nullptr)
		{
			ObjShader = (CShader*)RenderObject->Get_Component(L"Com_Shader");
			ObjShader->Set_RawValue("g_LightViewMatrix", &m_LightWVPmat.ViewMatrix, sizeof(_float4x4));
			ObjShader->Set_RawValue("g_LightProjMatrix", &m_LightWVPmat.ProjMatrix, sizeof(_float4x4));

			RenderObject->LightRender();
		}

	}

	for (auto& RenderObject : m_RenderObjectList[RENDER_PRIORITYBLEND])
	{
		if (RenderObject != nullptr)
		{
			ObjShader = (CShader*)RenderObject->Get_Component(L"Com_Shader");
			ObjShader->Set_RawValue("g_LightViewMatrix", &m_LightWVPmat.ViewMatrix, sizeof(_float4x4));
			ObjShader->Set_RawValue("g_LightProjMatrix", &m_LightWVPmat.ProjMatrix, sizeof(_float4x4));

			RenderObject->LightRender();
		}

	}

	m_pDeviceContext->RSSetViewports(iNumViewports, &OldViewPortDesc);

	FAILED_CHECK(m_pRenderTargetMgr->End(TEXT("MRT_Shadow")));



	return S_OK;
}



HRESULT CRenderer::Render_NonAlpha()
{

	for (auto& RenderObject : m_RenderObjectList[RENDER_NONBLEND])
	{
		if (RenderObject != nullptr)
		{
			FAILED_CHECK(RenderObject->Render());
		}
		Safe_Release(RenderObject);
	}

	m_RenderObjectList[RENDER_NONBLEND].clear();
	return S_OK;
}

HRESULT CRenderer::Render_PriorityAlpha()
{
	for (auto& RenderObject : m_RenderObjectList[RENDER_PRIORITYBLEND])
	{
		if (RenderObject != nullptr)
		{
			FAILED_CHECK(RenderObject->Render());
		}
		Safe_Release(RenderObject);
	}

	m_RenderObjectList[RENDER_PRIORITYBLEND].clear();
	return S_OK;

	return S_OK;
}

HRESULT CRenderer::Render_BlurShadow()
{

	m_pDeviceContext->ClearDepthStencilView(m_DownScaledDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);


	_uint		iNumViewports = 1;
	D3D11_VIEWPORT		OldViewPortDesc;
	m_pDeviceContext->RSGetViewports(&iNumViewports, &OldViewPortDesc);

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)(OldViewPortDesc.Width * 0.5);
	ViewPortDesc.Height = (_float)(OldViewPortDesc.Height * 0.5f);
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(iNumViewports, &ViewPortDesc);


	FAILED_CHECK(m_pRenderTargetMgr->Begin(TEXT("MRT_ShadowDownScaling"), m_DownScaledDepthStencil));

	CPipeLineMgr*		pPipeLineMgr = GetSingle(CPipeLineMgr);

	_float4x4		ViewMatrixInv, ProjMatrixInv;

	XMStoreFloat4x4(&ViewMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLineMgr->Get_Transform_Matrix(PLM_VIEW))));
	XMStoreFloat4x4(&ProjMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLineMgr->Get_Transform_Matrix(PLM_PROJ))));


	FAILED_CHECK(m_pShader->Set_RawValue("g_ViewMatrixInv", &ViewMatrixInv, sizeof(_float4x4)));
	FAILED_CHECK(m_pShader->Set_RawValue("g_ProjMatrixInv", &ProjMatrixInv, sizeof(_float4x4)));


	FAILED_CHECK(m_pShader->Set_Texture("g_ShadowMapTexture", m_pRenderTargetMgr->Get_SRV(TEXT("Target_Shadow"))));


	FAILED_CHECK(m_pShader->Set_RawValue("g_WorldMatrix", &m_WVPmat.WorldMatrix, sizeof(_float4x4)));
	FAILED_CHECK(m_pShader->Set_RawValue("g_ViewMatrix", &m_WVPmat.ViewMatrix, sizeof(_float4x4)));
	FAILED_CHECK(m_pShader->Set_RawValue("g_ProjMatrix", &m_WVPmat.ProjMatrix, sizeof(_float4x4)));

	FAILED_CHECK(m_pShader->Set_RawValue("g_LightViewMatrix", &m_LightWVPmat.ViewMatrix, sizeof(_float4x4)));
	FAILED_CHECK(m_pShader->Set_RawValue("g_LightProjMatrix", &m_LightWVPmat.ProjMatrix, sizeof(_float4x4)));

	FAILED_CHECK(m_pVIBuffer->Render(m_pShader, 5));

	FAILED_CHECK(m_pRenderTargetMgr->End(TEXT("MRT_ShadowDownScaling")));

	




	FAILED_CHECK(m_pRenderTargetMgr->Begin(TEXT("MRT_ShadowBluringVerticle"), m_DownScaledDepthStencil));

	FAILED_CHECK(m_pShader->Set_Texture("g_ShadowMapTexture", m_pRenderTargetMgr->Get_SRV(TEXT("Target_DownScaledBluredShadow"))));
	FAILED_CHECK(m_pShader->Set_RawValue("fScreemWidth", &ViewPortDesc.Width, sizeof(_float)));
	FAILED_CHECK(m_pShader->Set_RawValue("fScreemHeight", &ViewPortDesc.Height, sizeof(_float)));

	FAILED_CHECK(m_pVIBuffer->Render(m_pShader, 7));

	FAILED_CHECK(m_pRenderTargetMgr->End(TEXT("MRT_ShadowBluringVerticle")));


	FAILED_CHECK(m_pRenderTargetMgr->Begin(TEXT("MRT_ShadowBluringHorizon"), m_DownScaledDepthStencil));

	FAILED_CHECK(m_pShader->Set_Texture("g_ShadowMapTexture", m_pRenderTargetMgr->Get_SRV(TEXT("Target_ReferenceShadow"))));


	FAILED_CHECK(m_pVIBuffer->Render(m_pShader, 8));

	m_pDeviceContext->RSSetViewports(iNumViewports, &OldViewPortDesc);
	FAILED_CHECK(m_pRenderTargetMgr->End(TEXT("MRT_ShadowBluringHorizon")));















	FAILED_CHECK(m_pRenderTargetMgr->Begin(TEXT("MRT_ShadowUpScaling")));
	FAILED_CHECK(m_pShader->Set_Texture("g_ShadowMapTexture", m_pRenderTargetMgr->Get_SRV(TEXT("Target_DownScaledBluredShadow"))));
	FAILED_CHECK(m_pVIBuffer->Render(m_pShader, 6));
	FAILED_CHECK(m_pRenderTargetMgr->End(TEXT("MRT_ShadowUpScaling")));
	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	FAILED_CHECK(m_pRenderTargetMgr->Begin(TEXT("MRT_LightAcc")));

	FAILED_CHECK(m_pLightMgr->Render(m_pShader, m_pVIBuffer, &m_WVPmat));

	FAILED_CHECK(m_pRenderTargetMgr->End(TEXT("MRT_LightAcc")));

	return S_OK;
}

HRESULT CRenderer::Render_DeferredTexture()
{
	CPipeLineMgr*		pPipeLineMgr = GetSingle(CPipeLineMgr);

	_float4x4		ViewMatrixInv, ProjMatrixInv;

	XMStoreFloat4x4(&ViewMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLineMgr->Get_Transform_Matrix(PLM_VIEW))));
	XMStoreFloat4x4(&ProjMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLineMgr->Get_Transform_Matrix(PLM_PROJ))));


	FAILED_CHECK(m_pShader->Set_RawValue("g_ViewMatrixInv", &ViewMatrixInv, sizeof(_float4x4)));
	FAILED_CHECK(m_pShader->Set_RawValue("g_ProjMatrixInv", &ProjMatrixInv, sizeof(_float4x4)));

	FAILED_CHECK(m_pShader->Set_Texture("g_DiffuseTexture", m_pRenderTargetMgr->Get_SRV(TEXT("Target_Diffuse"))));
	FAILED_CHECK(m_pShader->Set_Texture("g_ShadeTexture", m_pRenderTargetMgr->Get_SRV(TEXT("Target_Shade"))));
	FAILED_CHECK(m_pShader->Set_Texture("g_SpecularTexture", m_pRenderTargetMgr->Get_SRV(TEXT("Target_Specular"))));
	FAILED_CHECK(m_pShader->Set_Texture("g_DepthTexture", m_pRenderTargetMgr->Get_SRV(TEXT("Target_Depth"))));

	FAILED_CHECK(m_pShader->Set_Texture("g_ShadowMapTexture", m_pRenderTargetMgr->Get_SRV(TEXT("Target_UpScaledBluredShadow"))));

	
	FAILED_CHECK(m_pShader->Set_RawValue("g_WorldMatrix", &m_WVPmat.WorldMatrix, sizeof(_float4x4)));
	FAILED_CHECK(m_pShader->Set_RawValue("g_ViewMatrix", &m_WVPmat.ViewMatrix, sizeof(_float4x4)));
	FAILED_CHECK(m_pShader->Set_RawValue("g_ProjMatrix", &m_WVPmat.ProjMatrix, sizeof(_float4x4)));

	//FAILED_CHECK(m_pShader->Set_RawValue("g_LightViewMatrix", &m_LightWVPmat.ViewMatrix, sizeof(_float4x4)));
	//FAILED_CHECK(m_pShader->Set_RawValue("g_LightProjMatrix", &m_LightWVPmat.ProjMatrix, sizeof(_float4x4)));

	FAILED_CHECK(m_pVIBuffer->Render(m_pShader, 3));

	return S_OK;
}

HRESULT CRenderer::Render_PostProcessing()
{


	for (auto& RenderObject : m_RenderObjectList[RENDER_POSTPROCESSING])
	{
		if (RenderObject != nullptr)
		{
			FAILED_CHECK(RenderObject->Render());
		}
		Safe_Release(RenderObject);
	}

	m_RenderObjectList[RENDER_POSTPROCESSING].clear();





	_uint iNumViews = 1;
	ID3D11RenderTargetView*		pRenderTargets[8] = { nullptr };
	pRenderTargets[0] = m_pGraphicDevice->Get_BackBufferRTV();
	m_pDeviceContext->OMSetRenderTargets(iNumViews, pRenderTargets, m_pGraphicDevice->Get_OriginalDSV());

	CPipeLineMgr*		pPipeLineMgr = GetSingle(CPipeLineMgr);

	_float4x4		ViewMatrixInv, ProjMatrixInv;

	XMStoreFloat4x4(&ViewMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLineMgr->Get_Transform_Matrix(PLM_VIEW))));
	XMStoreFloat4x4(&ProjMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLineMgr->Get_Transform_Matrix(PLM_PROJ))));

	m_pShader->Set_Texture("g_TargetTexture", m_pRenderTargetMgr->Get_SRV(L"Target_AfterDefferred"));

	FAILED_CHECK(m_pShader->Set_RawValue("g_WorldMatrix", &m_WVPmat.WorldMatrix, sizeof(_float4x4)));
	FAILED_CHECK(m_pShader->Set_RawValue("g_ViewMatrix", &m_WVPmat.ViewMatrix, sizeof(_float4x4)));
	FAILED_CHECK(m_pShader->Set_RawValue("g_ProjMatrix", &m_WVPmat.ProjMatrix, sizeof(_float4x4)));

	FAILED_CHECK(m_pVIBuffer->Render(m_pShader, 0));



	return S_OK;
}

HRESULT CRenderer::Render_NonAlpha_NoDeferrd()
{
	for (auto& RenderObject : m_RenderObjectList[RENDER_NONBLEND_NOLIGHT])
	{
		if (RenderObject != nullptr)
		{
			FAILED_CHECK(RenderObject->Render());
		}
		Safe_Release(RenderObject);
	}

	m_RenderObjectList[RENDER_NONBLEND_NOLIGHT].clear();


	for (auto& pComponent : m_TrailObjectList)
	{
		if (nullptr != pComponent)
			pComponent->Render();

		Safe_Release(pComponent);
	}

	m_TrailObjectList.clear();

	return S_OK;

}

HRESULT CRenderer::Render_Alpha()
{
	m_RenderObjectList[RENDER_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
	{
		return pSour->Get_RenderSortValue() > pDest->Get_RenderSortValue();
	});

	for (auto& RenderObject : m_RenderObjectList[RENDER_BLEND])
	{
		if (RenderObject != nullptr)
		{
			FAILED_CHECK(RenderObject->Render());
		}
		Safe_Release(RenderObject);
	}
	m_RenderObjectList[RENDER_BLEND].clear();
	return S_OK;
}

HRESULT CRenderer::Render_AfterObj()
{
	for (auto& RenderObject : m_RenderObjectList[RENDER_AFTEROBJ])
	{
		if (RenderObject != nullptr)
		{
			FAILED_CHECK(RenderObject->Render());
		}
		Safe_Release(RenderObject);
	}
	m_RenderObjectList[RENDER_AFTEROBJ].clear();

	return S_OK;
}


HRESULT CRenderer::Render_UI()
{
	if (m_pDevice == nullptr || m_pDeviceContext ==nullptr)
		return E_FAIL;


	m_RenderObjectList[RENDER_UI].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
	{
		return pSour->Get_RenderSortValue() > pDest->Get_RenderSortValue();
	});
	for (auto& RenderObject : m_RenderObjectList[RENDER_UI])
	{
		if (RenderObject != nullptr)
		{
			FAILED_CHECK(RenderObject->Render());
		}
		Safe_Release(RenderObject);
	}
	m_RenderObjectList[RENDER_UI].clear();


	return S_OK;
}

HRESULT CRenderer::Render_Debug()
{
	FAILED_CHECK(m_pShader->Apply(4));

	for (auto& pComponent : m_DebugObjectList)
	{
		if (nullptr != pComponent)
			pComponent->Render();

		Safe_Release(pComponent);
	}

	m_DebugObjectList.clear();

	return S_OK;
}

CRenderer * CRenderer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CRenderer* pInstance = new CRenderer(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg))) 
	{
		MSGBOX("Failed to Creating CRenderer");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CComponent * CRenderer::Clone(void * pArg)
{
	this->AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_DownScaledDepthStencil);
	Safe_Release(m_LightDepthStencil);

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);


	Safe_Release(m_pRenderTargetMgr);
	Safe_Release(m_pLightMgr);
	Safe_Release(m_pGraphicDevice);

	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for (auto& RenderObject : m_RenderObjectList[i])
		{
			Safe_Release(RenderObject);
		}
		m_RenderObjectList[i].clear();
	}

	for (auto& DebugObject : m_DebugObjectList)
	{
		Safe_Release(DebugObject);
	}
	m_DebugObjectList.clear();

	for (auto& TrailObject : m_TrailObjectList)
	{
		Safe_Release(TrailObject);
	}
	m_TrailObjectList.clear();

}
