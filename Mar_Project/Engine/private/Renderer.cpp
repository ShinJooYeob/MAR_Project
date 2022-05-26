#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "Camera.h"

#include "RenderTargetMgr.h"
#include "LightMgr.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"


CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_pRenderTargetMgr(CRenderTargetMgr::GetInstance())
	, m_pLightMgr(CLightMgr::GetInstance())
{
	Safe_AddRef(m_pRenderTargetMgr);
	Safe_AddRef(m_pLightMgr);

}

HRESULT CRenderer::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));



	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;

	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	/* For.Target_Diffuse */
	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget( TEXT("Target_Diffuse"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	/* For.Target_Normal */
	/* ��ֺ����� ���, ����ȭ�ϱⰡ ����. */
	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget(TEXT("Target_Normal"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f)));

	/* For.Target_Depth */
	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget(TEXT("Target_Depth"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f)));

	/* For.Target_Shade */
	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget(TEXT("Target_Shade"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f)));

	/* For.Target_Specular */
	FAILED_CHECK(m_pRenderTargetMgr->Add_RenderTarget(TEXT("Target_Specular"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));

	/* For.MRT_Deferred : ��ü���� �׸��� ���ε�. */
	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse")));
	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal")));
	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth")));


	/* For.MRT_LightAcc : ���� �׸��� ���ε� */
	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade")));
	FAILED_CHECK(m_pRenderTargetMgr->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular")));


	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext);
	NULL_CHECK_RETURN(m_pVIBuffer, E_FAIL);

	m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, TEXT("Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	NULL_CHECK_RETURN(m_pShader, E_FAIL);



#ifdef _DEBUG

	FAILED_CHECK(m_pRenderTargetMgr->Ready_DebugDesc(TEXT("Target_Diffuse"), 100.f, 100.f, 200.f, 200.f));
	FAILED_CHECK(m_pRenderTargetMgr->Ready_DebugDesc(TEXT("Target_Normal"), 100.f, 300.f, 200.f, 200.f));
	FAILED_CHECK(m_pRenderTargetMgr->Ready_DebugDesc(TEXT("Target_Depth"), 100.f, 500.f, 200.f, 200.f));
		
	FAILED_CHECK(m_pRenderTargetMgr->Ready_DebugDesc(TEXT("Target_Shade"), 300.f, 100.f, 200.f, 200.f));
	FAILED_CHECK(m_pRenderTargetMgr->Ready_DebugDesc(TEXT("Target_Specular"), 300.f, 300.0f, 200.f, 200.f));
#endif


	XMStoreFloat4x4(&m_WVPmat.WorldMatrix, XMMatrixIdentity());

	m_WVPmat.WorldMatrix._11 = Viewport.Width;
	m_WVPmat.WorldMatrix._22 = Viewport.Height;
	m_WVPmat.WorldMatrix._33 = 1.f;
	memcpy(&m_WVPmat.WorldMatrix.m[3][0], &_float4(Viewport.Width * 0.5f - (Viewport.Width * 0.5f), -Viewport.Height * 0.5f + (Viewport.Height * 0.5f), 0.f, 1.f), sizeof(_float4));

	XMStoreFloat4x4(&m_WVPmat.WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WVPmat.WorldMatrix)));
	XMStoreFloat4x4(&m_WVPmat.ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_WVPmat.ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f)));

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

HRESULT CRenderer::Add_DebugGroup(CComponent * pComponent)
{
	m_DebugObjectList.push_back(pComponent);
	Safe_AddRef(pComponent);

	return S_OK;
}

HRESULT CRenderer::Render_RenderGroup()
{


	FAILED_CHECK(Render_Priority());


	FAILED_CHECK(m_pRenderTargetMgr->Begin(TEXT("MRT_Deferred")));
	FAILED_CHECK(Render_NonAlpha());
	FAILED_CHECK(Render_PriorityAlpha());
	FAILED_CHECK(m_pRenderTargetMgr->End(TEXT("MRT_Deferred")));

	FAILED_CHECK(Render_Lights());
	FAILED_CHECK(Render_DeferredTexture());

	FAILED_CHECK(Render_Alpha());
	FAILED_CHECK(Render_AfterObj());


	FAILED_CHECK(Render_UI());


#ifdef _DEBUG
	FAILED_CHECK(Render_Debug());
	FAILED_CHECK(m_pRenderTargetMgr->Render_DebugBuffer(TEXT("MRT_Deferred")));
	FAILED_CHECK(m_pRenderTargetMgr->Render_DebugBuffer(TEXT("MRT_LightAcc")));
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

HRESULT CRenderer::Render_Lights()
{
	FAILED_CHECK(m_pRenderTargetMgr->Begin(TEXT("MRT_LightAcc")));

	m_pLightMgr->Render(m_pShader, m_pVIBuffer, &m_WVPmat);

	FAILED_CHECK(m_pRenderTargetMgr->End(TEXT("MRT_LightAcc")));

	return S_OK;
}

HRESULT CRenderer::Render_DeferredTexture()
{

	FAILED_CHECK(m_pShader->Set_Texture("g_DiffuseTexture", m_pRenderTargetMgr->Get_SRV(TEXT("Target_Diffuse"))));
	FAILED_CHECK(m_pShader->Set_Texture("g_ShadeTexture", m_pRenderTargetMgr->Get_SRV(TEXT("Target_Shade"))));
	FAILED_CHECK(m_pShader->Set_Texture("g_SpecularTexture", m_pRenderTargetMgr->Get_SRV(TEXT("Target_Specular"))));


	FAILED_CHECK(m_pShader->Set_RawValue("g_WorldMatrix", &m_WVPmat.WorldMatrix, sizeof(_float4x4)));
	FAILED_CHECK(m_pShader->Set_RawValue("g_ViewMatrix", &m_WVPmat.ViewMatrix, sizeof(_float4x4)));
	FAILED_CHECK(m_pShader->Set_RawValue("g_ProjMatrix", &m_WVPmat.ProjMatrix, sizeof(_float4x4)));


	m_pVIBuffer->Render(m_pShader, 3);

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


	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);


	Safe_Release(m_pRenderTargetMgr);
	Safe_Release(m_pLightMgr);

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

	
}
