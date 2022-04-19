#include "stdafx.h"
#include "..\Public\Loader.h"

#include "Player.h"
#include "NormalBullet.h"
#include "GrenadeBullet.h"

//Editor
#include "ESCursor.h"
#include "Camera_Editor.h"
#include "WireTerrain.h"
#include "RendererEditSceneUI.h"

//MapObject
#include "Terrain.h"
#include "JumpPad.h"
#include "SteamPad.h"
#include "HiddenPad.h"
#include "TeethObj.h"
#include "RoseObj.h"

#include "StaticMapObject.h"

//Monster
#include "Executor.h"
#include "Grunt.h"
#include "WaspArrow.h"
#include "WaspInk.h"
#include "Eyepot.h"

//UI
#include "GamePlayUI.h"


_uint CALLBACK LoadingThread(void* _Prameter)
{
	THREADARG tThreadArg{};
	memcpy(&tThreadArg, _Prameter, sizeof(THREADARG));
	delete _Prameter;


	CLoader* pLoader = (CLoader*)(tThreadArg.pArg);


	switch (pLoader->Get_SceneID())
	{
	case SCENEID::SCENE_LOBY:
		pLoader->Load_Scene_Loby(tThreadArg.IsClientQuit, tThreadArg.CriSec);
		break;
	case SCENEID::SCENE_STAGESELECT:
		pLoader->Load_Scene_StageSelect(tThreadArg.IsClientQuit, tThreadArg.CriSec);
		break;
	case SCENEID::SCENE_STAGE1:
		pLoader->Load_Scene_Stage1(tThreadArg.IsClientQuit, tThreadArg.CriSec);
		break;

	case SCENEID::SCENE_EDIT:

		pLoader->Load_Scene_Edit(tThreadArg.IsClientQuit, tThreadArg.CriSec);
		break;

	default:
		MSGBOX("Failed to Create Scene");
		break;
	}

	return 0;
}


CLoader::CLoader()
{
}



HRESULT CLoader::Initialize(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, SCENEID eSceneID)
{
	m_eSceneID = eSceneID;
	m_bIsLoadingFinished = false;
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	g_pGameInstance->PlayThread(LoadingThread, this);

	return S_OK;
}

HRESULT CLoader::Load_Scene_Loby(_bool * _IsClientQuit, CRITICAL_SECTION * _CriSec)
{
	CGameInstance* pGameInstance = GetSingle(CGameInstance);

#pragma region PROTOTYPE_COMPONENT





#pragma endregion

#pragma  region PROTOTYPE_GAMEOBJECT



#pragma endregion

	EnterCriticalSection(_CriSec);
	m_iLoadingMaxCount = 1;
	m_iLoadingProgressCount = 0;
	LeaveCriticalSection(_CriSec);

	EnterCriticalSection(_CriSec);
	m_bIsLoadingFinished = true;
	LeaveCriticalSection(_CriSec);

	return S_OK;
}

HRESULT CLoader::Load_Scene_StageSelect(_bool * _IsClientQuit, CRITICAL_SECTION * _CriSec)
{

	EnterCriticalSection(_CriSec);
	m_iLoadingMaxCount = 1;
	m_iLoadingProgressCount = 0;
	LeaveCriticalSection(_CriSec);

	for (int i = 0; i < m_iLoadingMaxCount; ++i)
	{
		EnterCriticalSection(_CriSec);
		m_iLoadingProgressCount = i;
		LeaveCriticalSection(_CriSec);
	}

	EnterCriticalSection(_CriSec);
	m_bIsLoadingFinished = true;
	LeaveCriticalSection(_CriSec);
	m_bIsLoadingFinished = true;
	return S_OK;
}

HRESULT CLoader::Load_Scene_Stage1(_bool * _IsClientQuit, CRITICAL_SECTION * _CriSec)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


#pragma region PROTOTYPE_COMPONENT
	//Anim 모델 프로토타입
	_Matrix			TransformMatrix;
	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//TransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));


	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Player),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Alice", "Alice.FBX", TransformMatrix, 2)));

	//_Matrix			TransformMatrix;
	//TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));


	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_Executor),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Executor", "Executor.FBX", TransformMatrix)));
	//
	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_Grunt),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Grunt", "Grunt.FBX", TransformMatrix)));
	//
	//
	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_Eyepot),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Eyepot", "Eyepot.FBX", TransformMatrix)));
	//
	//
	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_WaspInk),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "WaspInk", "WaspInk.FBX", TransformMatrix)));
	//
	//
	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_WaspArrow),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "WaspArrow", "WaspArrow.FBX", TransformMatrix)));





	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	/* 테스트 알게락 */
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_AlgaeRock_Ledge),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "AlgaeRock", "AlgaeRock_Ledge.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_AlgaeRock_Pillar),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "AlgaeRock", "AlgaeRock_Pillar.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_AlgaeRock_Wall),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "AlgaeRock", "AlgaeRock_Wall.FBX", TransformMatrix)));


	/* 테스트 글로리그레스*/

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

	///* 테스트 켈프*/

	//FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Kelp_Single),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Kelp", "Kelp_Single.FBX", TransformMatrix)));

	//FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Kelp_Wall),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Kelp", "Kelp_Wall.FBX", TransformMatrix)));

	//FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Kelp_WallFar),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Kelp", "Kelp_WallFar.FBX", TransformMatrix)));

	//FAILED_CHECK(m_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_KelpWallBack),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Kelp", "KelpWallBack.FBX", TransformMatrix)));




	//////////Terrain
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_VIBuffer_Terrain),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, L"Height.bmp")));


	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Texture_Terrain),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"Terrain.txt")));



	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Texture_GamePlayScene),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"UI_GamePlay.txt")));



#pragma endregion

#pragma  region PROTOTYPE_GAMEOBJECT


	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Player), CPlayer::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Bullet_Normal), CNormalBullet::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Bullet_Grenade), CGrenadeBullet::Create(m_pDevice, m_pDeviceContext)));



	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_UIGamePlay),
		CGamePlayUI::Create(m_pDevice, m_pDeviceContext)));

	//동적 맵 오브젝트
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Terrain),
		CTerrain::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_JumpPad),
		CJumpPad::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_SteamPad),
		CSteamPad::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_HiddenPad),
		CHiddenPad::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_TeethObj),
		CTeethObj::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_RoseObj),
		CRoseObj::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_StaticMapObject),
		CStaticMapObject::Create(m_pDevice, m_pDeviceContext)));

	//몬스터
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Executor),
		CExecutor::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Grunt),
		CGrunt::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Eyepot),
		CEyepot::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_WaspInk),
		CWaspInk::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_WaspArrow),
		CWaspArrow::Create(m_pDevice, m_pDeviceContext)));

#pragma endregion
	RELEASE_INSTANCE(CGameInstance);


	EnterCriticalSection(_CriSec);
	m_iLoadingMaxCount = 1;
	m_iLoadingProgressCount = 0;
	LeaveCriticalSection(_CriSec);

	for (int i = 0; i < m_iLoadingMaxCount; ++i)
	{
		EnterCriticalSection(_CriSec);
		m_iLoadingProgressCount = i;
		LeaveCriticalSection(_CriSec);
	}

	EnterCriticalSection(_CriSec);
	m_bIsLoadingFinished = true;
	LeaveCriticalSection(_CriSec);
	m_bIsLoadingFinished = true;
	return S_OK;
}

HRESULT CLoader::Load_Scene_Edit(_bool * _IsClientQuit, CRITICAL_SECTION * _CriSec)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

#pragma region PROTOTYPE_COMPONENT
#pragma endregion

	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_EDIT, TAG_CP(Prototype_VIBuffer_Terrain_Edit),
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, 257)));
	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_EDIT, TAG_CP(Prototype_Texture_Terrain),
	//	CTexture::Create(m_pDevice, m_pDeviceContext, L"Terrain.txt")));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_EDIT, TAG_CP(Prototype_VIBuffer_Terrain),
		CVIBuffer_DynamicTerrain::Create(m_pDevice, m_pDeviceContext,  4, 4)));


	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_EDIT, TAG_CP(Prototype_Texture_EditScene),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"EditScene.txt")));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_EDIT, TAG_CP(Prototype_Texture_Edit_Terrain),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"EditTerrain.txt")));



#pragma  region PROTOTYPE_GAMEOBJECT
#pragma endregion

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_EditorCursor), CESCursor::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Camera_Editor), CCamera_Editor::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_WireTerrain), CWireTerrain::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(L"ProtoType_EditRendererUI", CRendererEditSceneUI::Create(m_pDevice, m_pDeviceContext)));


	RELEASE_INSTANCE(CGameInstance);
	//EnterCriticalSection(_CriSec);
	//m_iLoadingMaxCount = 1;
	//m_iLoadingProgressCount = 0;
	//LeaveCriticalSection(_CriSec);

	//for (int i = 0; i < m_iLoadingMaxCount; ++i)
	//{
	//	EnterCriticalSection(_CriSec);
	//	m_iLoadingProgressCount = i;
	//	LeaveCriticalSection(_CriSec);
	//}

	EnterCriticalSection(_CriSec);
	m_bIsLoadingFinished = true;
	LeaveCriticalSection(_CriSec);

	return S_OK;
}


CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, SCENEID eSceneID)
{
	CLoader* pTempLoader = new CLoader;

	if (FAILED(pTempLoader->Initialize(pDevice,pDeviceContext,eSceneID)))
	{
		Safe_Release(pTempLoader);
		MSGBOX("Failed to Create CLoader");
		return nullptr;
	}

	return pTempLoader;

}

void CLoader::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	
}
