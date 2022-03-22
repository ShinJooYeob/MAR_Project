#include "stdafx.h"
#include "..\public\MainApp.h"
#include "Scene_Loading.h"
#include "Camera_Main.h"

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

	//콜리전 내부 탐색중
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

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.0f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pGameInstance->Render_Scene();


	m_pGameInstance->Present();


	//콜리전 내부 비워주는중
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

		//Scene_Change에 디폴트로 false true 이걸 트루로 바꿔준다?
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

	////와이어 프레임
	////m_pGraphicDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	////잠시 조명연산을 꺼놓자
	//m_pGraphicDevice->SetRenderState(D3DRS_LIGHTING, false);

	//m_pGraphicDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//m_pGraphicDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	////텍스쳐의 사이즈가 확대되거나 축소될때 변경 된 사이즈에 발생한 공백을 선형 보간해서 채움

	//m_pGraphicDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	////텍스쳐를 인게임 내에서 수정하게 되면 과하게 선명해서 오히려 눈이 아픈 경우가 발생

	//m_pGraphicDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	//m_pGraphicDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	////UV좌표를 1을 넘어가게 작성할 경우 다시 1인 점을 0으로 치환하여 좌표를 찍음


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


	////렌더러 컴객체 프로토타입 생성
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Renderer), m_pComRenderer = CRenderer::Create(m_pGraphicDevice))))
	//	return E_FAIL;
	//Safe_AddRef(m_pComRenderer);

	////버퍼인덱스 프로토타입 생성
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_VIBuffer_Rect), CVIBuffer_Rect::Create(m_pGraphicDevice))))
	//	return E_FAIL;



	////Transform 프로토타입 생성
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Transform), CTransform::Create(m_pGraphicDevice))))
	//	return E_FAIL;

	///* 디폴트 텍스처 프로토타입 생성 */
	//CTexture::TEXTUREDESC TextureDesc{};
	//TextureDesc.eTextureType = CTexture::TYPE_DEFAULT;
	//TextureDesc.szTextFilePath = TEXT("Player.txt");

	//if (FAILED(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Texture_Default), CTexture::Create(m_pGraphicDevice,&TextureDesc))))
	//	return E_FAIL;

	////플레이어 텍스처 생성
	////Player Texture
	//TextureDesc.szTextFilePath = TEXT("Player.txt");

	//if (FAILED(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Texture_Player), CTexture::Create(m_pGraphicDevice, &TextureDesc))))
	//	return E_FAIL;


	//TextureDesc.szTextFilePath = TEXT("Mouse.txt");
	//TextureDesc.eTextureType = CTexture::TYPE_DEFAULT;

	//if (FAILED(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TEXT("Prototype_Component_Texture_Mouse"), CTexture::Create(m_pGraphicDevice, &TextureDesc))))
	//	return E_FAIL;

	////UI_Common 텍스처
	//TextureDesc.szTextFilePath = TEXT("UI.txt");
	//TextureDesc.eTextureType = CTexture::TYPE_DEFAULT;

	//if (FAILED(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Texture_UI), CTexture::Create(m_pGraphicDevice, &TextureDesc))))
	//	return E_FAIL;
	////블랭크 텍스처
	//TextureDesc.szTextFilePath = TEXT("Cam_Effect.txt");
	//
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Texture_Blank), CTexture::Create(m_pGraphicDevice, &TextureDesc))))
	//	return E_FAIL;

	////미니맵 텍스쳐
	//TextureDesc.szTextFilePath = TEXT("MiniMap.txt");
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Texture_MiniMap), CTexture::Create(m_pGraphicDevice, &TextureDesc))))
	//	return E_FAIL;


	//TextureDesc.eTextureType = CTexture::TYPE_CUBEMAP;
	//TextureDesc.szTextFilePath = TEXT("SkyBoxTexture.txt");
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TEXT("Prototype_Component_Texture_SkyBox"), CTexture::Create(m_pGraphicDevice, &TextureDesc))))
	//	return E_FAIL;



	//// 콜리전 프로토타입 생성
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TEXT("Prototype_Component_Collision"), m_pCollision = CCollision::Create(m_pGraphicDevice))))
	//	return E_FAIL;
	//Safe_AddRef(m_pCollision);

	//// 뷰포트용 콜리전 프로토타입 생성
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_CollisionView), m_pCollisionView = CCom_CollisionViewPort::Create(m_pGraphicDevice))))
	//	return E_FAIL;
	//Safe_AddRef(m_pCollisionView);

	////버퍼인덱스 큐브 프로토타입 생성
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pGraphicDevice))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TEXT("Prototype_Component_Inventory"), CInventory::Create(m_pGraphicDevice))))
	//	return E_FAIL;

	//// 셰이더 프로토타입 생성
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Shader_Test), CShader::Create(m_pGraphicDevice, TEXT("../Bin/ShaderFiles/Shader_Test.hlsl")))))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Shader_Cube), CShader::Create(m_pGraphicDevice, TEXT("../Bin/ShaderFiles/Shader_TerrainCube.hlsl")))))
	//	return E_FAIL;
	//// 총 컴포넌트 생성
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Gun), CCom_Gun::Create(m_pGraphicDevice))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Static_GameObject_Prototype()
{	//Camera_Main 프로토타입 생성
	//CCamera::CAMERADESC CameraDesc;
	//CameraDesc.vWorldRotAxis = _float3(0, 0, 0);
	//CameraDesc.vEye = _float3(0, 5.f, -10.f);
	//CameraDesc.vAt = _float3(0, 5.f, 0);
	//CameraDesc.vAxisY = _float3(0, 1, 0);

	//CameraDesc.fFovy = D3DXToRadian(60.0f);
	//CameraDesc.fAspect = _float(g_iWinCX) / g_iWinCY;
	//CameraDesc.fNear = 0.2f;
	//CameraDesc.fFar = 300.f;

	//CameraDesc.TransformDesc.fMovePerSec = 10.f;
	//CameraDesc.TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	//if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Camera_Main), CCamera_Main::Create(m_pGraphicDevice, &CameraDesc))))
	//	return E_FAIL;
	//
	//
	//if (FAILED(m_pGameInstance->Add_GameObject_Prototype(L"Prototype_Mouse_UI", CUI_Mouse::Create(m_pGraphicDevice, _float4(0, 0, 0, 0)))))
	//	return E_FAIL;

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

	if (FAILED(Free_SingletonMgr()))
	{
		__debugbreak();
		MSGBOX("Failed to Free Singleton");
	}

	Safe_Release(m_pSwapChain);
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);
}

