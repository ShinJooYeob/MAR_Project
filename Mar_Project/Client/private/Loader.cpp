#include "stdafx.h"
#include "..\Public\Loader.h"

#include "LobyAlice.h"
#include "LobyUI.h"

#include "Player.h"
#include "NormalBullet.h"
#include "GrenadeBullet.h"
#include "TornadoSwirl.h"
#include "Knife.h"
#include "Horse.h"
#include "Grinder.h"
#include "Teapot.h"
#include "Umbrella.h"

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
#include "BreakableObj.h"
#include "BreakablePiece.h"

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

	_Matrix			TransformMatrix;
	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_LOBY, TAG_CP(Prototype_Mesh_PlayerLoby),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Alice", "Alice.FBX", TransformMatrix, 1)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_LOBY, TAG_CP(Prototype_Texture_LobyScene),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"UI_Loby.txt")));


#pragma endregion

#pragma  region PROTOTYPE_GAMEOBJECT
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_UILoby), CLobyUI::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_PlayerLoby), CLobyAlice::Create(m_pDevice, m_pDeviceContext)));



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

	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Tornado1),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Tornado", "TornadoMesh.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Tornado2),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Tornado", "TornadoMesh02.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Tornado3),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Tornado", "TornadoMesh01.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Tornado4),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Tornado", "TornadoSwirl01.FBX", TransformMatrix)));








	//Pivot  :-0.61f, -0.01, -1.32f
	////////////////*무기*/
	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f)*
		XMMatrixRotationX(XMConvertToRadians(-83.7f))*
		XMMatrixRotationY(XMConvertToRadians(-83.1f))*
		XMMatrixRotationZ(XMConvertToRadians(-30))*
		XMMatrixTranslation(-0.61f, -0.01f, -1.32f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_KnifeUg),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Weapon/Knife", "KnifeUg.FBX", TransformMatrix)));

	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f)*
		XMMatrixRotationX(XMConvertToRadians(-20.479946f))*
		XMMatrixRotationY(XMConvertToRadians(211.469604f))*
		XMMatrixRotationZ(XMConvertToRadians(71.019928f))*
		XMMatrixTranslation(-0.61f, 0.02f, -1.32f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Horse),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Weapon/Horse", "Horse.FBX", TransformMatrix)));


	//_float3(-0.610000, 0.020000, -1.290000);
	//_float3(-27.030010, -10.069997, -201.929626);

	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f)*
		XMMatrixRotationX(XMConvertToRadians(-27.03f))*
		XMMatrixRotationY(XMConvertToRadians(-10.069997f))*
		XMMatrixRotationZ(XMConvertToRadians(-201.929626f))*
		XMMatrixTranslation(-0.61f, 0.0200f, -1.290f);

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Grinder),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Weapon/Grinder", "Grinder.FBX", TransformMatrix)));


	//-0.640000, 0.020000, -1.260000);
	//-193.979645, 9.009996, -129.319794);

	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f)*
		XMMatrixRotationX(XMConvertToRadians(-193.979645f))*
		XMMatrixRotationY(XMConvertToRadians(9.009996f))*
		XMMatrixRotationZ(XMConvertToRadians(-129.319794f))*
		XMMatrixTranslation(-0.640f, 0.02f, -1.26f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Teapot),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Weapon/Teapot", "Teapot.FBX", TransformMatrix)));



	//-0.584000, -0.032000, -1.333000,
	//	-20.713932, 205.632385, 68.354744,

	TransformMatrix = XMMatrixScaling(0.00015f, 0.00015f, 0.00015f)*
		XMMatrixRotationX(XMConvertToRadians(-20.713932f))*
		XMMatrixRotationY(XMConvertToRadians(205.632385f))*
		XMMatrixRotationZ(XMConvertToRadians(68.354744f))*
		XMMatrixTranslation(-0.584000f, -0.032000f, -1.333000f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Umbrella),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Weapon/Umbrella", "Umbrella.FBX", TransformMatrix)));
	


	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(90.0f));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_SkyBox),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "SkyBox", "SkyBox_0.FBX", TransformMatrix)));


	TransformMatrix = XMMatrixScaling(0.0003f, 0.0003f, 0.0003f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_OilBullet),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "OilBullet", "OilBullet.FBX", TransformMatrix)));

	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_TeaBullet),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "TeaBullet", "TeaBullet.FBX", TransformMatrix)));
	
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
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_HealthRose),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "HealthRose", "Health.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_Tooth),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Tooth", "Tooth.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_GoldenTooth),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "ToothGolden", "Tooth.FBX", TransformMatrix)));


	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_SteamPad),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "SteamPad", "SteamPad.FBX", TransformMatrix)));


	/*히든 패드*/
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GiantToys_Die),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GiantToys", "GiantToys_Die.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GiantToys_DominoC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GiantToys", "GiantToys_DominoC.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GiantToys_DominoE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GiantToys", "GiantToys_DominoE.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GiantToys_DominoStair),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GiantToys", "GiantToys_DominoStair.FBX", TransformMatrix)));

	/*BreakableObj & Piece*/

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_BreakableBox),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "BreakableBox", "HatterBox.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_BreakableBoxPiece1),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "BreakableBox", "HatterBoxA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_BreakableBoxPiece2),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "BreakableBox", "HatterBoxB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_BreakableBoxPiece3),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "BreakableBox", "HatterBoxC.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_BreakableBoxPiece4),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "BreakableBox", "HatterBoxD.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_BreakableBoxPiece5),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "BreakableBox", "HatterBoxG.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_BreakableBoxPiece6),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "BreakableBox", "HatterBoxK.FBX", TransformMatrix)));
	
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_BreakableBarrel),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "BreakableBarrel", "HatterBarrel.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_BreakableBarrelPiece1),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "BreakableBarrel", "HatterBarrelA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_BreakableBarrelPiece2),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "BreakableBarrel", "HatterBarrelB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_BreakableBarrelPiece3),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "BreakableBarrel", "HatterBarrelC.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_BreakableBarrelPiece4),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "BreakableBarrel", "HatterBarrelD.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_BreakableBarrelPiece5),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "BreakableBarrel", "HatterBarrelE.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GiftBasket),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GiftBasket", "GiftBasket.FBX", TransformMatrix)));


	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	/* 테스트 알게락 */
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_AlgaeRock_Ledge),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "AlgaeRock", "AlgaeRock_Ledge.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_AlgaeRock_Pillar),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "AlgaeRock", "AlgaeRock_Pillar.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_AlgaeRock_Wall),
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
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_PlayerTornado), CTornadoSwirl::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_WeaponKnife), CKnife::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_WeaponHorse), CHorse::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_WeaponGrinder), CGrinder::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_WeaponTeapot), CTeapot::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_WeaponUmbrella), CUmbrella::Create(m_pDevice, m_pDeviceContext)));
	//FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_WeaponClockBomb), CTornadoSwirl::Create(m_pDevice, m_pDeviceContext)));


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



	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_BreakableObj),
		CBreakableObj::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_BreakablePiece),
		CBreakablePiece::Create(m_pDevice, m_pDeviceContext)));
	

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
 