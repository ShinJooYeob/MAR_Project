#include "stdafx.h"
#include "..\public\MainApp.h"
#include "Scene_Loading.h"
#include "Camera_Main.h"
#include "Player.h"
#include "UIImage.h"
#include "SkyBox.h"
#include "Model.h"

#include "NormalBullet.h"
#include "GrenadeBullet.h"

#ifdef USE_IMGUI
#include "ImguiMgr.h"
#endif // USE_IMGUI
#include "UtilityMgr.h"

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

	if (m_bIsSlowed)
		Update_SlowMotion(fDeltaTime);


	g_fDeltaTime = fDeltaTime * m_SlowTimes;

	if (FAILED(m_pGameInstance->Update_Engine(fDeltaTime * m_SlowTimes)))
	{
		MSGBOX("Failed to Update_Engine ");
		return E_FAIL;
	}

	//�ݸ��� ���� Ž����
	//m_pCollision->Collision_Obsever(fDeltaTime);

	if (FAILED(m_pGameInstance->LateUpdate_Engine(fDeltaTime * m_SlowTimes)))
	{
		MSGBOX("Failed to LateUpdate_Engine ");
		return E_FAIL;

	}



	
	return 0;
}

_double CMainApp::Update_SlowMotion(_double fDeltaTime)
{
	m_fPassedTime += _float(fDeltaTime);

	if (m_fPassedTime < m_fTargetTime * 0.5f)
		 m_SlowTimes = m_pGameInstance->Easing(TYPE_QuarticOut, 1, m_fTargetSpeed, m_fPassedTime, m_fTargetTime *0.5f);
	else
		m_SlowTimes = m_pGameInstance->Easing(TYPE_QuarticIn, m_fTargetSpeed, 1, m_fPassedTime - m_fTargetTime *0.5f, m_fTargetTime *0.5f);

	if (m_fPassedTime > m_fTargetTime)
	{
		m_bIsSlowed = false;
		m_SlowTimes = 1;
	}

	return m_SlowTimes;
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

void CMainApp::SlowMotionStart(_float fTargetTime , _float TargetSpeed )
{
	if (m_bIsSlowed)
		return;

	m_fTargetSpeed = TargetSpeed;
	m_fTargetTime = fTargetTime;
	m_fPassedTime = 0;

	m_bIsSlowed = true;


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

	FAILED_CHECK(GetSingle(CUtilityMgr)->Initialize_UtilityMgr(m_pDevice, m_pDeviceContext, this));

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
	if (0 != GetSingle(CUtilityMgr)->DestroyInstance())
	{
		MSGBOX("Failed to Release CUtilityMgr");
		return E_FAIL;
	}

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

	FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("Shader_NonAnimModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements)));

	FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Shader_VAM),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("Shader_AnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements)));



	////�ؽ�ó ������Ÿ�� ����
	FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Texture_Player),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"Font2.txt" )));

	FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Texture_SkyBox),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"SkyBox.txt")));



	_Matrix			TransformMatrix;
	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//TransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	//�� ������Ÿ��
	FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Player),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Alice", "Alice.FBX", TransformMatrix)));
	//FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Player),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "ForkLift", "ForkLift.FBX", TransformMatrix)));



	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	/* �׽�Ʈ �˰Զ� */
	FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_AlgaeRock_Ledge),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "AlgaeRock", "AlgaeRock_Ledge.FBX", TransformMatrix)));

	FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_AlgaeRock_Pillar),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "AlgaeRock", "AlgaeRock_Pillar.FBX", TransformMatrix)));

	FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_AlgaeRock_Wall),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "AlgaeRock", "AlgaeRock_Wall.FBX", TransformMatrix)));


	/* �׽�Ʈ �۷θ��׷���*/

	//FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GloryGrowthA),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryGrowthA.FBX", TransformMatrix)));

	//FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GloryGrowthB),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryGrowthB.FBX", TransformMatrix)));

	//FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GloryGrowthC),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryGrowthC.FBX", TransformMatrix)));

	//FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GloryGrowthD),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryGrowthD.FBX", TransformMatrix)));
	//
	//FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GloryTree_Base),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryTree_Base.FBX", TransformMatrix)));
	//
	//FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GloryTree_MainB),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryTree_MainB.FBX", TransformMatrix)));

	//FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GloryTree_Main),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryTree_Main.FBX", TransformMatrix)));

	//FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GloryTree_Shell),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryTree_Shell.FBX", TransformMatrix)));

	///* �׽�Ʈ ����*/

	//FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Kelp_Single),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Kelp", "Kelp_Single.FBX", TransformMatrix)));

	//FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Kelp_Wall),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Kelp", "Kelp_Wall.FBX", TransformMatrix)));

	//FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Kelp_WallFar),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Kelp", "Kelp_WallFar.FBX", TransformMatrix)));

	//FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_KelpWallBack),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Kelp", "KelpWallBack.FBX", TransformMatrix)));




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

	FAILED_CHECK(m_pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Bullet_Normal), CNormalBullet::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(m_pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Bullet_Grenade), CGrenadeBullet::Create(m_pDevice, m_pDeviceContext)));


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

