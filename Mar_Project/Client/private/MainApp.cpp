#include "stdafx.h"
#include "..\public\MainApp.h"
#include "Scene_Loading.h"
#include "Camera_Main.h"
#include "Player.h"
#include "UIImage.h"
#include "SkyBox.h"

#ifdef USE_IMGUI
#include "ImguiMgr.h"
#endif // USE_IMGUI

CMainApp::CMainApp()
	:m_pGameInstance(GetSingle(CGameInstance))
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	CGraphic_Device::GRAPHICDESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(CGraphic_Device::GRAPHICDESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.eWinMode = CGraphic_Device::MODE_WIN;
	GraphicDesc.iWinCX = g_iWinCX;
	GraphicDesc.iWinCY = g_iWinCY;

	//ShowCursor(false);


	FAILED_CHECK(m_pGameInstance->Initialize_Engine(g_hInst, GraphicDesc, SCENEID::SCENE_END, &m_pDevice, &m_pDeviceContext,&m_pBackBufferRTV,&m_pDepthStencilView,&m_pSwapChain));


	FAILED_CHECK(Default_Setting());

	FAILED_CHECK(Ready_SingletonMgr());

	FAILED_CHECK(Ready_Static_Component_Prototype());

	FAILED_CHECK(Ready_Static_GameObject_Prototype());

	FAILED_CHECK(Scene_Change(SCENEID::SCENE_LOBY));

	return S_OK;
}

_int CMainApp::Update(_double fDeltaTime)
{
	if (m_pGameInstance == nullptr)
		return -1;

	g_fDeltaTime = fDeltaTime;

	if (FAILED(m_pGameInstance->Update_Engine(fDeltaTime)))
	{
		MSGBOX("Failed to Update_Engine ");
		return E_FAIL;
	}

	//�ݸ��� ���� Ž����
	//m_pCollision->Collision_Obsever(fDeltaTime);

	if (FAILED(m_pGameInstance->LateUpdate_Engine(fDeltaTime)))
	{
		MSGBOX("Failed to LateUpdate_Engine ");
		return E_FAIL;

	}



	
	return 0;
}

HRESULT CMainApp::Render()
{
	if (m_pGameInstance == nullptr)
	{
		__debugbreak();
		return E_FAIL;
	}

	FAILED_CHECK(m_pGameInstance->Clear_BackBuffer_View(_float4(0.5f, 0.5f, 0.5f, 1.f)));
	FAILED_CHECK(m_pGameInstance->Clear_DepthStencil_View());


	FAILED_CHECK(m_pComRenderer->Render_RenderGroup());
	FAILED_CHECK(m_pGameInstance->Render_Scene());


	FAILED_CHECK(m_pGameInstance->Present());


	//�ݸ��� ���� ����ִ���
	//m_pCollision->Collision_Obsever_Release(); 




	return S_OK;

}

HRESULT CMainApp::Scene_Change(SCENEID eSceneID)
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;



	switch (eSceneID)
	{
	case SCENEID::SCENE_LOBY:
	case SCENEID::SCENE_STAGESELECT:
	case SCENEID::SCENE_STAGE1:
	case SCENEID::SCENE_EDIT:

		//Scene_Change�� ����Ʈ�� false true �̰� Ʈ��� �ٲ��ش�?
		m_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice,m_pDeviceContext, eSceneID), SCENEID::SCENE_LOADING);

		break;

	default:
		return E_FAIL;
		break;
	}

	return S_OK;
}

HRESULT CMainApp::Default_Setting()
{
	if (m_pDevice == nullptr || m_pDeviceContext == nullptr)
	{
		__debugbreak();
		return E_FAIL;
	}

	////���̾� ������
	////m_pGraphicDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	////��� �������� ������
	//m_pGraphicDevice->SetRenderState(D3DRS_LIGHTING, false);

	//m_pGraphicDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//m_pGraphicDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	////�ؽ����� ����� Ȯ��ǰų� ��ҵɶ� ���� �� ����� �߻��� ������ ���� �����ؼ� ä��

	//m_pGraphicDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	////�ؽ��ĸ� �ΰ��� ������ �����ϰ� �Ǹ� ���ϰ� �����ؼ� ������ ���� ���� ��찡 �߻�

	//m_pGraphicDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	//m_pGraphicDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	////UV��ǥ�� 1�� �Ѿ�� �ۼ��� ��� �ٽ� 1�� ���� 0���� ġȯ�Ͽ� ��ǥ�� ����


	return S_OK;
}

HRESULT CMainApp::Ready_SingletonMgr()
{

#ifdef USE_IMGUI
	FAILED_CHECK(GETIMGUI->Initialize_ImguiMgr(m_pDevice, m_pDeviceContext, m_pBackBufferRTV, m_pDepthStencilView, m_pSwapChain));
#endif // USE_IMGUI



	return S_OK;
}

HRESULT CMainApp::Free_SingletonMgr()
{

#ifdef USE_IMGUI

	if (0 != GetSingle(CImguiMgr)->DestroyInstance())
	{
		MSGBOX("Failed to Release CImguiMgr");
		return E_FAIL;
	}

#endif // USE_IMGUI

	return S_OK;
}

HRESULT CMainApp::Ready_Static_Component_Prototype()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;


	//������ �İ�ü ������Ÿ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Renderer), 
		m_pComRenderer = CRenderer::Create(m_pDevice,m_pDeviceContext)));
	Safe_AddRef(m_pComRenderer);


	FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Transform),
		CTransform::Create(m_pDevice, m_pDeviceContext)));

	////�����ε��� ������Ÿ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_VIBuffer_Rect),
		CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_VIBuffer_Cube),
		CVIBuffer_Cube::Create(m_pDevice, m_pDeviceContext)));

	////���̴�
	FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Shader_VT),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements)));

	FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Shader_VCT),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("Shader_VtxCubeTex.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements)));

	FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Shader_VNT),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("Shader_VtxNorTex.hlsl"), VTXNORMTEX_DECLARATION::Elements, VTXNORMTEX_DECLARATION::iNumElements)));


	////�ؽ�ó ������Ÿ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Texture_Player),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"Font2.txt" )));

	FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Texture_SkyBox),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"SkyBox.txt")));




	return S_OK;
}

HRESULT CMainApp::Ready_Static_GameObject_Prototype()
{
	///////////////////Camera_Main ������Ÿ�� ����
	CCamera::CAMERADESC CameraDesc;
	CameraDesc.vWorldRotAxis = _float3(0, 0, 0);
	CameraDesc.vEye = _float3(0, 5.f, -10.f);
	CameraDesc.vAt = _float3(0, 0.f, 0);
	CameraDesc.vAxisY = _float3(0, 1, 0);

	CameraDesc.fFovy = XMConvertToRadians(60.f);
	CameraDesc.fAspect = _float(g_iWinCX) / g_iWinCY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.f;

	CameraDesc.iWinCX = g_iWinCX;
	CameraDesc.iWinCY = g_iWinCY;

	CameraDesc.TransformDesc.fMovePerSec = 5.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(60.0f);
	CameraDesc.TransformDesc.fScalingPerSec = 1.f;



	FAILED_CHECK(m_pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Camera_Main), CCamera_Main::Create(m_pDevice, m_pDeviceContext, &CameraDesc)));
	//
	//
	FAILED_CHECK(m_pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Player), CPlayer::Create(m_pDevice, m_pDeviceContext)));


	FAILED_CHECK(m_pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_UIImage), CUIImage::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(m_pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_SkyBox), CSkyBox::Create(m_pDevice, m_pDeviceContext)));

	//FAILED_CHECK(m_pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Terrain), CSkyBox::Create(m_pDevice, m_pDeviceContext)));


	//if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_UI_Common), CUI_Common::Create(m_pGraphicDevice, _float4(0, 0, 0, 0)))))
	//	return E_FAIL;
	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp;

	if (FAILED(pInstance->Initialize()))
	{
		MSGBOX("Failed to Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	m_pGameInstance->Get_NowScene()->Free();

	if (FAILED(Free_SingletonMgr()))
	{
		__debugbreak();
		MSGBOX("Failed to Free Singleton");
	}

	Safe_Release(m_pComRenderer);

	Safe_Release(m_pSwapChain);
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);
}

