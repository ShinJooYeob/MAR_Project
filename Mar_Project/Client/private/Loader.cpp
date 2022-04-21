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
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Alice", "Alice.FBX", TransformMatrix, 1)));

	//_Matrix			TransformMatrix;
	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//TransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));


	
	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_Grunt),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Grunt", "Grunt.FBX", TransformMatrix)));
	////
	//
	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_Eyepot),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Eyepot", "Eyepot.FBX", TransformMatrix)));
	//
	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_WaspInk),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "WaspInk", "WaspInk.FBX", TransformMatrix)));
	//
	//
	////
	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_Executor),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Executor", "Executor.FBX", TransformMatrix)));
	//
	//
	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_WaspArrow),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "WaspArrow", "WaspArrow.FBX", TransformMatrix)));


	/*Map Object*/
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_JumpPad),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "JumpPad", "JumpPad.FBX", TransformMatrix)));



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

	//
	/* 테스트 켈프*/

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


	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_EDIT, TAG_CP(Prototype_VIBuffer_Terrain_Edit),
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, 257)));
	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_EDIT, TAG_CP(Prototype_Texture_Terrain),
	//	CTexture::Create(m_pDevice, m_pDeviceContext, L"Terrain.txt")));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_EDIT, TAG_CP(Prototype_VIBuffer_Terrain),
		CVIBuffer_DynamicTerrain::Create(m_pDevice, m_pDeviceContext, 4, 4)));


	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_EDIT, TAG_CP(Prototype_Texture_EditScene),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"EditScene.txt")));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_EDIT, TAG_CP(Prototype_Texture_Edit_Terrain),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"EditTerrain.txt")));




	_Matrix			TransformMatrix;
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	/* 테스트 알게락 */

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_AlgaeRock_Ledge),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "AlgaeRock", "AlgaeRock_Ledge.FBX", TransformMatrix)));


	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_AlgaeRock_Pillar),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "AlgaeRock", "AlgaeRock_Pillar.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_AlgaeRock_Wall),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "AlgaeRock", "AlgaeRock_Wall.FBX", TransformMatrix)));


	/////////////////////* 테스트 글로리그레스*/

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GloryGrowthA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryGrowthA.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GloryGrowthB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryGrowthB.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GloryGrowthC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryGrowthC.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GloryGrowthD),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryGrowthD.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GloryTree_Base),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryTree_Base.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GloryTree_MainB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryTree_MainB.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GloryTree_Main),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryTree_Main.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GloryTree_Shell),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryTree_Shell.FBX", TransformMatrix)));


	////////////////////////////* 테스트 켈프*/

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Kelp_Single),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Kelp", "Kelp_Single.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Kelp_Wall),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Kelp", "Kelp_Wall.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Kelp_WallFar),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Kelp", "Kelp_WallFar.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_KelpWallBack),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Kelp", "KelpWallBack.FBX", TransformMatrix)));


	//////////////////////////*자이언트 토이*/
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GiantToys_Die),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GiantToys", "GiantToys_Die.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GiantToys_DominoC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GiantToys", "GiantToys_DominoC.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GiantToys_DominoE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GiantToys", "GiantToys_DominoE.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GiantToys_DominoStair),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GiantToys", "GiantToys_DominoStair.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GiantToys_Jacks),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GiantToys", "GiantToys_Jacks.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GiantToys_MarblesA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GiantToys", "GiantToys_MarblesA.FBX", TransformMatrix)));


	///////////////키홀 
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_KeyHole),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Keyhole", "Keyhole_DollHouse.FBX", TransformMatrix)));

	////////////////모아이 
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Moai_Gate),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Moai", "Moai_Gate.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Moai_Plat),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Moai", "Moai_Plat.FBX", TransformMatrix)));

	/////////////// Q배틀 타워
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_QBattleTower),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QBattleTower", "QBattleTower.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_QBattleTowerBaseDie),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QBattleTower", "QBattleTowerBaseDie.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QBattleTowerParticleA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QBattleTower", "QBattleTowerParticleA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QBattleTowerParticleB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QBattleTower", "QBattleTowerParticleB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QBattleTowerPieceA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QBattleTower", "QBattleTowerPieceA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QBattleTowerPieceB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QBattleTower", "QBattleTowerPieceB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QBattleTowerPieceC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QBattleTower", "QBattleTowerPieceC.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_SK_QBattleTowerTentacle_Giant),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QBattleTower", "SK_QBattleTowerTentacle_Giant.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_SK_QBattleTowerTentacle_Group),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QBattleTower", "SK_QBattleTowerTentacle_Group.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_SK_QBattleTowerTentacle_Thin),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QBattleTower", "SK_QBattleTowerTentacle_Thin.FBX", TransformMatrix)));

	///////////////////캐논
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_SK_QCannon),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QCannon", "SK_QCannon.FBX", TransformMatrix)));

	////////////////////////Q플레시
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QFleshFloorA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QFlesh", "QFleshFloorA.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QFleshFloorB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QFlesh", "QFleshFloorB.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QFleshFloorC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QFlesh", "QFleshFloorC.FBX", TransformMatrix)));

	TransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QfleshStairs),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QFlesh", "QfleshStairs.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	/////////////////자이언트 게이트
	TransformMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QGiantBreakableGate_Broken),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QGiantBreakable", "QGiantBreakableGate_Broken.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QGiantBreakableGate_Flesh),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QGiantBreakable", "QGiantBreakableGate_Flesh.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	///////////////아우터 월
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QOuterwall_WallB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QOuterWall", "QOuterwall_WallB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QOuterwall_WallD),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QOuterWall", "QOuterwall_WallD.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QOuterwall_WallE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QOuterWall", "QOuterwall_WallE.FBX", TransformMatrix)));


	//////////////QRocks
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QRocksB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QRocks", "QRocksB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QRocksC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QRocks", "QRocksC.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QRocksD),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QRocks", "QRocksD.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QRocksE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QRocks", "QRocksE.FBX", TransformMatrix)));


	/////////////////////우는소녀동상
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_TearsRock),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "TearsRock", "TearsRock.FBX", TransformMatrix)));


	////////////////////해초 암석
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_GiantUnderseaRock_Holey),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "UnderSeaRock", "GiantUnderseaRock_Holey.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_UnderseaRock_Ledge),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "UnderSeaRock", "UnderseaRock_Ledge.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_UnderseaRock_Pile),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "UnderSeaRock", "UnderseaRock_Pile.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_UnderseaRock_Pillar),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "UnderSeaRock", "UnderseaRock_Pillar.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_UnderseaRock_Plat),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "UnderSeaRock", "UnderseaRock_Plat.FBX", TransformMatrix)));



	//////////////////////바다 통나무
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_WaterLogA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "WaterLog", "WaterLogA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_WaterLogB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "WaterLog", "WaterLogB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_WaterLogC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "WaterLog", "WaterLogC.FBX", TransformMatrix)));


#pragma endregion



#pragma  region PROTOTYPE_GAMEOBJECT


	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_EditorCursor), CESCursor::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Camera_Editor), CCamera_Editor::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_WireTerrain), CWireTerrain::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(L"ProtoType_EditRendererUI", CRendererEditSceneUI::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_StaticMapObject),
		CStaticMapObject::Create(m_pDevice, m_pDeviceContext)));
#pragma endregion

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
 