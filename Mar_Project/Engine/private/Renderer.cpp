#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "Camera.h"



CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

HRESULT CRenderer::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	//D3DVIEWPORT9		ViewPortDesc;
	//m_pGraphicDevice->GetViewport(&ViewPortDesc);
	//_int iWinCX = ViewPortDesc.Width;
	//_int iWinCY = ViewPortDesc.Height;
	//D3DXMatrixOrthoLH(&m_ProjMatrix, (_float)iWinCX, (_float)iWinCY, 0.0f, 1.f);




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

	//if (eRenderID == CRenderer::RENDER_BLEND)
	//	pGameObject->Compute_CamDistance((CTransform*)(pGameObject->Get_Component(TEXT("Com_Transform"))));

	m_RenderObjectList[eRenderID].emplace_back(pGameObject);
	
	Safe_AddRef(pGameObject);


	return S_OK;
}

HRESULT CRenderer::Render_RenderGroup()
{


	FAILED_CHECK(Render_Priority());

	FAILED_CHECK(Render_NonAlpha());

	FAILED_CHECK(Render_Alpha());

	FAILED_CHECK(Render_AfterObj());

	FAILED_CHECK(Render_UI());

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

HRESULT CRenderer::Render_Alpha()
{
	//m_RenderObjectList[RENDER_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
	//{
	//	return pSour->Get_CamDistance() > pDest->Get_CamDistance();
	//});

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

	//_float4x4 BeforeViewMatrix, BeforeProjectmat;
	//m_pGraphicDevice->GetTransform(D3DTS_VIEW, &BeforeViewMatrix);
	//m_pGraphicDevice->GetTransform(D3DTS_PROJECTION, &BeforeProjectmat);


	//_float4x4 ViewMatrix;
	//D3DXMatrixIdentity(&ViewMatrix);
	//m_pGraphicDevice->SetTransform(D3DTS_VIEW, &ViewMatrix);
	//m_pGraphicDevice->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);


	//m_pGraphicDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	//m_pGraphicDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	//m_RenderObjectList[RENDER_UI].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
	//{
	//	return pSour->Get_CamDistance() > pDest->Get_CamDistance();
	//});

	for (auto& RenderObject : m_RenderObjectList[RENDER_UI])
	{
		if (RenderObject != nullptr)
		{
			FAILED_CHECK(RenderObject->Render());
		}
		Safe_Release(RenderObject);
	}
	m_RenderObjectList[RENDER_UI].clear();


	//m_pGraphicDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	//m_pGraphicDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	//m_pGraphicDevice->SetTransform(D3DTS_VIEW, &BeforeViewMatrix);
	//m_pGraphicDevice->SetTransform(D3DTS_PROJECTION, &BeforeProjectmat);
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


	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for (auto& RenderObject : m_RenderObjectList[i])
		{
			Safe_Release(RenderObject);
		}
		m_RenderObjectList[i].clear();
	}
}
