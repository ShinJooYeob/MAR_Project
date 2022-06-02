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
#include "ClockBomb.h"
#include "Protein.h"
#include "CircleTornado.h"

#include "Instance_Particle.h"

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
#include "EscalatorPad.h"
#include "SlidePad.h"
#include "ButtonPad.h"
#include "ShapeMemoryPad.h"
#include "ShpaeMemButton.h"
#include "PresserObj.h"
#include "ShrinkFlower.h"
#include "VentObj.h"
#include "MovableColum.h"
#include "MovableColumBtn.h"
#include "BreakableWall.h"

#include "SceneChageTriger.h"
#include "Stage1_SpwanGrunt.h"
#include "Stage1_SpwanWasp.h"
#include "Stage2_SpwanEyePot.h"
#include "Stage3_SpwanExecutor.h"
#include "Stage3_GientTrigger.h"
#include "StageBoss_SpwanBoss.h"

#include "StaticMapObject.h"

//Monster
#include "DustWind.h"

#include "Executor.h"
#include "EntireCard.h"
#include "CardPeice.h"
#include "BreakedGazebo.h"
#include "ExecutorThron.h"
#include "DustWindCurvedMove.h"
#include "ScrechedBlock.h"
#include "ScrechedDust.h"
#include "TransparencyBall.h"
#include "HyperVoice.h"
#include "HandyGirlBullet.h"

#include "Grunt.h"
#include "ChainExplosion.h"
#include "ThrowOilBullet.h"


#include "WaspArrow.h"
#include "WaspInk.h"
#include "Eyepot.h"
#include "DollMaker.h"

#include "Scythe.h"
#include "HandyBoy.h"
#include "HandyGirl.h"

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
	case SCENEID::SCENE_STAGE2:
		pLoader->Load_Scene_Stage2(tThreadArg.IsClientQuit, tThreadArg.CriSec);
		break;
	case SCENEID::SCENE_STAGE3:
		pLoader->Load_Scene_Stage3(tThreadArg.IsClientQuit, tThreadArg.CriSec);
		break;

	case SCENEID::SCENE_BOSS:
		pLoader->Load_Scene_Boss(tThreadArg.IsClientQuit, tThreadArg.CriSec);
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
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Alice", "Alice.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_LOBY, TAG_CP(Prototype_Texture_LobyScene),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"UI_Loby.txt")));


#pragma endregion

#pragma  region PROTOTYPE_GAMEOBJECT
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_UILoby), CLobyUI::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_PlayerLoby), CLobyAlice::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_TriggerCollider_SceneChager), CSceneChageTriger::Create(m_pDevice, m_pDeviceContext)));


	
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

	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Tornado1),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Tornado", "TornadoMesh.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Tornado2),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Tornado", "TornadoMesh02.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Tornado3),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Tornado", "TornadoMesh01.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Tornado4),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Tornado", "TornadoSwirl01.FBX", TransformMatrix)));

	TransformMatrix = XMMatrixScaling(0.0004f, 0.0004f, 0.0004f) *		XMMatrixRotationX(XMConvertToRadians(180.f)) *
		XMMatrixTranslation(0.f, 1.f, 0.f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_DustTornado),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "DustWind", "DustWind.FBX", TransformMatrix)));


	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_CircleMesh),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "CircleMesh", "CircleMesh.FBX", TransformMatrix)));
	

	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Protein),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Protein", "Protein.FBX", TransformMatrix)));

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

	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f)
	*
		XMMatrixRotationX(XMConvertToRadians(-20.865261f))*
		XMMatrixRotationY(XMConvertToRadians(-10.069997f))*
		XMMatrixRotationZ(XMConvertToRadians(-201.929626f))*
		XMMatrixTranslation(-0.61f, 0.0050f, -1.3650f);


	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Grinder),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Weapon/Grinder", "Grinder.FBX", TransformMatrix)));


	//-0.625000, 0.020000, -1.290000
	//-193.979645, 9.009996, -129.319794);

	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f)*
		XMMatrixRotationX(XMConvertToRadians(-193.979645f))*
		XMMatrixRotationY(XMConvertToRadians(9.009996f))*
		XMMatrixRotationZ(XMConvertToRadians(-129.319794f))*
		XMMatrixTranslation(-0.625f, 0.02f, -1.29f);

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
	


	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f)*
		XMMatrixRotationX(XMConvertToRadians(-48.203003f))*
		XMMatrixRotationY(XMConvertToRadians(255.028625f))*
		XMMatrixRotationZ(XMConvertToRadians(103.890877f))*
		XMMatrixTranslation(-0.597f, 0.033f, -1.281f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_ClockBomb),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Weapon/ClockBomb", "ClockBomb.FBX", TransformMatrix)));


	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Texture_MskTex),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"MSK_TEX.txt")));


	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(90.0f));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_SkyBox),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "SkyBox", "SkyBox_0.FBX", TransformMatrix)));


	TransformMatrix = XMMatrixScaling(0.00015f, 0.00015f, 0.00015f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_OilBullet),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "OilBullet", "OilBullet.FBX", TransformMatrix)));

	TransformMatrix = XMMatrixScaling(0.00005f, 0.00005f, 0.00005f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_TeaBullet),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "TeaBullet", "TeaBullet.FBX", TransformMatrix)));
	
	//_Matrix			TransformMatrix;
	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////Grunt
	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_Grunt),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Grunt", "Grunt.FBX", TransformMatrix)));
	//
	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_GruntSwpanMesh),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Grunt", "GruntSpwanMesh.FBX", TransformMatrix)));
	//
	//
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////WaspInk
	//TransformMatrix = XMMatrixScaling(0.0003f, 0.0003f, 0.0003f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_WaspInk),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "WaspInk", "WaspInk.FBX", TransformMatrix)));
	//TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////SpwanMesh
	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, L"Grunt_StaticSwpanMesh",
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Grunt", "GruntSpwanMesh2.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
								

	/*
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////Executor
	//TransformMatrix = XMMatrixScaling(0.00025f, 0.00025f, 0.00025f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_Executor),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Executor", "Executor.FBX", TransformMatrix)));
	//
	////Pivot  : -3.699998f , -0.075000f , -3.824997f , 1
	////size  : -88.699219f , 1.200000f , 10.100002f  
	//TransformMatrix = XMMatrixScaling(0.0002f, 0.00020f, 0.00020f)*
	//	XMMatrixRotationX(XMConvertToRadians(-88.699219f))*
	//	XMMatrixRotationY(XMConvertToRadians(1.200000f))*
	//	XMMatrixRotationZ(XMConvertToRadians(10.100002f))*
	//	XMMatrixTranslation(-3.699998f, -0.075000f, -3.824997f);
	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_Scythe),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "MonsterWeapon/Executioner", "Scythe.FBX", TransformMatrix)));
	//TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	//
	//
	//FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_WaspArrow),
	//	CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "WaspArrow", "WaspArrow.FBX", TransformMatrix)));


	*/



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

	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GiantToys_DominoC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GiantToys", "GiantToys_DominoC.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GiantToys_DominoE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GiantToys", "GiantToys_DominoE.FBX", TransformMatrix)));

	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
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

		TransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GiftBasket),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GiftBasket", "GiftBasket.FBX", TransformMatrix)));


	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	/* 테스트 알게락 */

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_AlgaeRock_Ledge),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "AlgaeRock", "AlgaeRock_Ledge.FBX", TransformMatrix)));


	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_AlgaeRock_Pillar),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "AlgaeRock", "AlgaeRock_Pillar.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_AlgaeRock_Wall),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "AlgaeRock", "AlgaeRock_Wall.FBX", TransformMatrix)));


	/////////////////////* 테스트 글로리그레스*/

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_GloryGrowthA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryGrowthA.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_GloryGrowthB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryGrowthB.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_GloryGrowthC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryGrowthC.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_GloryGrowthD),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryGrowthD.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_GloryTree_Base),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryTree_Base.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_GloryTree_MainB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryTree_MainB.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_GloryTree_Main),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryTree_Main.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_GloryTree_Shell),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "GloryGrass", "GloryTree_Shell.FBX", TransformMatrix)));


	////////////////////////////* 테스트 켈프*/

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_Kelp_Single),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Kelp", "Kelp_Single.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_Kelp_Wall),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Kelp", "Kelp_Wall.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_Kelp_WallFar),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Kelp", "Kelp_WallFar.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_KelpWallBack),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Kelp", "KelpWallBack.FBX", TransformMatrix)));

	///////////////키홀 
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_KeyHole),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Keyhole", "Keyhole_DollHouse.FBX", TransformMatrix)));

	////////////////모아이 
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_Moai_Gate),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Moai", "Moai_Gate.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_Mesh_Moai_Plat),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Moai", "Moai_Plat.FBX", TransformMatrix)));

	/////////////////////우는소녀동상
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_TearsRock),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "TearsRock", "TearsRock.FBX", TransformMatrix)));


	////////////////////해초 암석
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_GiantUnderseaRock_Holey),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "UnderSeaRock", "GiantUnderseaRock_Holey.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_UnderseaRock_Ledge),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "UnderSeaRock", "UnderseaRock_Ledge.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_UnderseaRock_Pile),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "UnderSeaRock", "UnderseaRock_Pile.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_UnderseaRock_Pillar),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "UnderSeaRock", "UnderseaRock_Pillar.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_UnderseaRock_Plat),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "UnderSeaRock", "UnderseaRock_Plat.FBX", TransformMatrix)));



	//////////////////////바다 통나무
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_WaterLogA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "WaterLog", "WaterLogA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_WaterLogB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "WaterLog", "WaterLogB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_WaterLogC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "WaterLog", "WaterLogC.FBX", TransformMatrix)));


	//////////////QRocks
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_QRocksB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QRocks", "QRocksB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_QRocksC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QRocks", "QRocksC.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_QRocksD),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QRocks", "QRocksD.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_QRocksE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QRocks", "QRocksE.FBX", TransformMatrix)));


	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_ShrinkFlower),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "ShrinkFlower", "ShrinkFlower.FBX", TransformMatrix)));





	//////////Terrain
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE1, TAG_CP(Prototype_VIBuffer_Terrain),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, L"Height_1.bmp")));

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
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_WeaponClockBomb), CClockBomb::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_EatableProtein), CProtein::Create(m_pDevice, m_pDeviceContext)));

	


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


	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_ShrinkFlower),
		CShrinkFlower::Create(m_pDevice, m_pDeviceContext)));



	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_BreakableObj),
		CBreakableObj::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_BreakablePiece),
		CBreakablePiece::Create(m_pDevice, m_pDeviceContext)));
	

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_StaticMapObject),
		CStaticMapObject::Create(m_pDevice, m_pDeviceContext)));



	//몬스터


	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Executor),
		CExecutor::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Scythe),
		CScythe::Create(m_pDevice, m_pDeviceContext)));


	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Grunt),
		CGrunt::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(L"Prototype_ChainExplosion",
		CChainExplosion::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(L"Prototype_ThrowOilBullet",
		CThrowOilBullet::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_PlayerCircleTornado),
		CCircleTornado::Create(m_pDevice, m_pDeviceContext)));
	

	


	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Eyepot),
		CEyepot::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_WaspInk),
		CWaspInk::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_WaspArrow),
		CWaspArrow::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_DustWind),
		CDustWind::Create(m_pDevice, m_pDeviceContext)));



	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(L"Stage1_SpwanGrunt",
		CStage1_SpwanGrunt::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(L"Stage1_SpwanWasp",
		CStage1_SpwanWasp::Create(m_pDevice, m_pDeviceContext)));
	

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Instance_Particle_Ball),
		CInstance_Particle::Create(m_pDevice, m_pDeviceContext)));
	
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

HRESULT CLoader::Load_Scene_Stage2(_bool * _IsClientQuit, CRITICAL_SECTION * _CriSec)
{


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);



#pragma region PROTOTYPE_COMPONENT


	_Matrix TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_ClockworkGears_DecoC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "ClockworkGears_DecoC.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_GiantGearPlatsA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "GiantGearPlatsA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterBlockC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterBlockC.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterBlocksE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterBlocksE.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterBlocksG),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterBlocksG.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterBlocksH),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterBlocksH.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterBuildingE_Door),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterBuildingE_Door.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterDoorA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterDoorA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterDoorC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterDoorC.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterExtTowerA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterExtTowerA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterExtTowerB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterExtTowerB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterExtTowerD),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterExtTowerD.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterExtWallB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterExtWallB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterExtWallB_Hole),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterExtWallB_Hole.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterExtWallC_LeftHole),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterExtWallC_LeftHole.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterExtWallC_RightHole),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterExtWallC_RightHole.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterExtWallD),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterExtWallD.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterFloorSystem_OriginalShortA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterFloorSystem_OriginalShortA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterFloorSystem_OriginalShortB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterFloorSystem_OriginalShortB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterFloorSystem_OriginalShortE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterFloorSystem_OriginalShortE.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterFloorSystem_OriginalTallE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterFloorSystem_OriginalTallE.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterFloorSystem_TeaMakerPlat),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterFloorSystem_TeaMakerPlat.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterJunk_M),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterJunk_M.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterJunk_S),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterJunk_S.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterLightsE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterLightsE.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterPillarsB_column),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPillarsB_column.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterPillarsC_top),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPillarsC_top.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterPillarsD_base),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPillarsD_base.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterPillarsG),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPillarsG.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterPlat_CheckeI),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPlat_CheckeI.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterPlat_CheckerA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPlat_CheckerA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterPlat_CheckerB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPlat_CheckerB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterPlat_CheckerC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPlat_CheckerC.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterPlat_CheckerE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPlat_CheckerE.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterPlat_CheckerF),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPlat_CheckerF.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterPlat_CheckerG),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPlat_CheckerG.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterPlat_MetalSquare),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPlat_MetalSquare.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterWallBricks_ChunkB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterWallBricks_ChunkB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterWallBricks_HalfA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterWallBricks_HalfA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterWallBricks_HalfB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterWallBricks_HalfB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterWallBricks_Pillar),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterWallBricks_Pillar.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterWallBricks_Rubble),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterWallBricks_Rubble.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_HatterWallBricks_Wall),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterWallBricks_Wall.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_Keyhole_Hatter),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "Keyhole_Hatter.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_KeyHoleTurnnels3),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "KeyHoleTurnnels3.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_OrnateGearA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "OrnateGearA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_OrnateGearC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "OrnateGearC.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_OrnateGrilleB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "OrnateGrilleB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_OrnateJunkA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "OrnateJunkA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_OrnatePlatsA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "OrnatePlatsA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_OrnatePlatsE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "OrnatePlatsE.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_PipeMachineB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "PipeMachineB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_PipeMessA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "PipeMessA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_Weights_ChainLong),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "Weights_ChainLong.FBX", TransformMatrix)));




	TransformMatrix = XMMatrixScaling(0.0005f, 0.0005f, 0.0005f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_SkyBox),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "SkyBox", "SkyBox_Down.FBX", TransformMatrix)));



	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_SteamPad),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "SteamPad", "SteamPad.FBX", TransformMatrix)));



	TransformMatrix = XMMatrixScaling(0.0002f, 0.0001f, 0.0001f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_Slide),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Slide", "DUSlide_Straight.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.0002f, 0.0002f, 0.0002f)*XMMatrixTranslation(0, 0.3f, 0);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_Button),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "ButtonPad", "ButtonPad.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.0002f, 0.0002f, 0.0002f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_Button_Frame),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "ButtonPad", "ButtonPad_Frame.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);



	TransformMatrix = XMMatrixScaling(0.0003f, 0.0003f, 0.0003f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Mesh_Eyepot),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Eyepot", "Eyepot.FBX", TransformMatrix)));

	//////////Terrain
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_VIBuffer_Terrain),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, L"Height_2.bmp")));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Texture_Terrain),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"Terrain_2.txt")));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE2, TAG_CP(Prototype_Texture_GamePlayScene),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"UI_GamePlay.txt")));


#pragma endregion

#pragma  region PROTOTYPE_GAMEOBJECT


	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_EscalatorPad),
		CEscalatorPad::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_SlideObj),
		CSlidePad::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_ButtonPad),
		CButtonPad::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_ShapeMemoryPad),
		CShapeMemoryPad::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_ShapeMemBtn),
		CShpaeMemButton::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Presser),
		CPresserObj::Create(m_pDevice, m_pDeviceContext)));


	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(L"Stage2_SpwanEyePot",
		CStage2_SpwanEyePot::Create(m_pDevice, m_pDeviceContext)));
	

#pragma endregion

	RELEASE_INSTANCE(CGameInstance);
	EnterCriticalSection(_CriSec);
	m_iLoadingMaxCount = 1;
	m_iLoadingProgressCount = 0;
	LeaveCriticalSection(_CriSec);

	EnterCriticalSection(_CriSec);
	m_bIsLoadingFinished = true;
	LeaveCriticalSection(_CriSec);


	return S_OK;
}

HRESULT CLoader::Load_Scene_Stage3(_bool * _IsClientQuit, CRITICAL_SECTION * _CriSec)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);



#pragma region PROTOTYPE_COMPONENT
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////Executor
	_Matrix TransformMatrix = XMMatrixScaling(0.00025f, 0.00025f, 0.00025f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_Executor),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "Executor", "Executor.FBX", TransformMatrix)));

	//Pivot  : -3.699998f , -0.075000f , -3.824997f , 1
	//size  : -88.699219f , 1.200000f , 10.100002f  
	TransformMatrix = XMMatrixScaling(0.0002f, 0.00020f, 0.00020f)*
		XMMatrixRotationX(XMConvertToRadians(-88.699219f))*
		XMMatrixRotationY(XMConvertToRadians(1.200000f))*
		XMMatrixRotationZ(XMConvertToRadians(10.100002f))*
		XMMatrixTranslation(-3.699998f, -0.075000f, -3.824997f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_Scythe),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "MonsterWeapon/Executioner", "Scythe.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_SK_QBattleTowerTentacle_Giant),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QBattleTower", "SK_QBattleTowerTentacle_Giant.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_SK_QBattleTowerTentacle_Group),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QBattleTower", "SK_QBattleTowerTentacle_Group.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_SK_QBattleTowerTentacle_Thin),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QBattleTower", "SK_QBattleTowerTentacle_Thin.FBX", TransformMatrix)));

	TransformMatrix = XMMatrixScaling(0.10f, 0.10f, 0.10f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_Gazebo_Piece01),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "Gazebo_Piece01.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_Gazebo_Piece02),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "Gazebo_Piece02.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_Gazebo_Piece03),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "Gazebo_Piece03.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_Gazebo_Piece04),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "Gazebo_Piece04.FBX", TransformMatrix)));

	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QBattleTowerParticleA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QBattleTower", "QBattleTowerParticleA.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QBattleTowerParticleB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QBattleTower", "QBattleTowerParticleB.FBX", TransformMatrix)));

	//////////////////////////////////////////////////////////////////////////
	TransformMatrix = XMMatrixScaling(0.000075f, 0.000075f, 0.000075f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_QMaze),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QMaze", "QMaze.FBX", TransformMatrix)));


	TransformMatrix = XMMatrixScaling(0.0005f, 0.0005f, 0.0005f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_SkyBox),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "SkyBox", "SkyBox_aftnoon.FBX", TransformMatrix)));



	TransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixTranslation(-0.9f, -0.9f, 0);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_CardPiece1),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "CardPiece", "CardPiece01.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixTranslation(0.8f, -1.f, 0);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_CardPiece2),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "CardPiece", "CardPiece02.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixRotationZ(XMConvertToRadians(-30))* XMMatrixTranslation(-0.9f, 1.14f, -0.2f);;
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_CardPiece3),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "CardPiece", "CardPiece03.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationX(XMConvertToRadians(93.0f)) * XMMatrixTranslation(0.76f, 0.97f, -0.34f);;
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_CardPiece4),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "CardPiece", "CardPiece04.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);


	
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_Gazebo_S),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "Gazebo_S.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_QStone_PillarC_Broken),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "QStone_PillarC_Broken.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_Qstone_PlatsB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "Qstone_PlatsB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_Vent_Steam_Mesh),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "Vent_Steam_Mesh.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_VentQStone),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "VentQStone.FBX", TransformMatrix)));


	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_HatterWallBricks_HalfA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterWallBricks_HalfA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_HatterWallBricks_HalfB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterWallBricks_HalfB.FBX", TransformMatrix)));




	TransformMatrix = XMMatrixScaling(0.05f, 0.08f, 0.05f)*		XMMatrixRotationZ(XMConvertToRadians(90))* 		XMMatrixTranslation(-8.f, 0, 0);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_HatterPillarsB_column),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPillarsB_column.FBX", TransformMatrix)));


	TransformMatrix = XMMatrixScaling(0.0002f, 0.0002f, 0.0002f)*XMMatrixTranslation(0, 0.3f, 0);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_Button),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "ButtonPad", "ButtonPad.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.0002f, 0.0002f, 0.0002f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_Button_Frame),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "ButtonPad", "ButtonPad_Frame.FBX", TransformMatrix)));



	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_Protein),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Protein", "Protein.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Mesh_ProteinTable),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Protein", "Table.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_ProteinTableCover),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Protein", "Cover.FBX", TransformMatrix)));

	TransformMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_QGiantBreakableGate_Broken),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QGiantBreakable", "QGiantBreakableGate_Broken.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_QGiantBreakableGate_Flesh),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QGiantBreakable", "QGiantBreakableGate_Flesh.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	//////////Terrain
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_VIBuffer_Terrain),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, L"Height_3.bmp")));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Texture_Terrain),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"Terrain_3.txt")));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STAGE3, TAG_CP(Prototype_Texture_GamePlayScene),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"UI_GamePlay.txt")));

#pragma endregion

#pragma  region PROTOTYPE_GAMEOBJECT


	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Vent),
		CVentObj::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_MazeDoor),
		CMovableColum::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_MazeDoorBtn),
		CMovableColumBtn::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_BreakableWall),
		CBreakableWall::Create(m_pDevice, m_pDeviceContext)));



	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_CardPiece),
		CCardPiece::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_EntireCard),
		CEntireCard::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_BreakedGazbo),
		CBreakedGazebo::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_Thron),
		CExecutorThron::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(L"Prototype_DustCurved",
		CDustWindCurvedMove::Create(m_pDevice, m_pDeviceContext)));
	



	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(L"Stage3_SpwanExecutor",
		CStage3_SpwanExecutor::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(L"Stage3_GientTrigger",
		CStage3_GientTrigger::Create(m_pDevice, m_pDeviceContext)));
	

#pragma endregion

	RELEASE_INSTANCE(CGameInstance);
	EnterCriticalSection(_CriSec);
	m_iLoadingMaxCount = 1;
	m_iLoadingProgressCount = 0;
	LeaveCriticalSection(_CriSec);

	EnterCriticalSection(_CriSec);
	m_bIsLoadingFinished = true;
	LeaveCriticalSection(_CriSec);


	return S_OK;
}

HRESULT CLoader::Load_Scene_Boss(_bool * _IsClientQuit, CRITICAL_SECTION * _CriSec)
{



	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_Matrix TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_BOSS, TAG_CP(Prototype_Mesh_DollMaker),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "DollMaker", "DollMaker.FBX", TransformMatrix)));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_BOSS, TAG_CP(Prototype_Mesh_HandyBoy),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "DollMaker", "HandyBoy.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_BOSS, TAG_CP(Prototype_Mesh_HandyGirl),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "DollMaker", "HandyGirl.FBX", TransformMatrix)));


	TransformMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_BOSS, TAG_CP(Prototype_QueenTower_BrokenC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QueenTower", "QueenTower_BrokenC.FBX", TransformMatrix)));

	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_BOSS, TAG_CP(Prototype_Mesh_HatterPillarsG),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPillarsG.FBX", TransformMatrix)));

	TransformMatrix = XMMatrixScaling(0.0005f, 0.0005f, 0.0005f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_BOSS, TAG_CP(Prototype_Mesh_SkyBox),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "SkyBox", "SkyBox_Boss.FBX", TransformMatrix)));



	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_BOSS, TAG_CP(Prototype_QBattleTowerParticleA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QBattleTower", "QBattleTowerParticleA.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_BOSS, TAG_CP(Prototype_QBattleTowerParticleB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QBattleTower", "QBattleTowerParticleB.FBX", TransformMatrix)));


	//TransparencyBall

	TransformMatrix = XMMatrixScaling(0.0003f, 0.0003f, 0.0003f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_BOSS, TAG_CP(Prototype_Mesh_BallMesh),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "BallMesh", "BallMesh.FBX", TransformMatrix)));

	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_BOSS, L"HandyGirlBulletMesh",
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Grunt", "GruntSpwanMesh2.FBX", TransformMatrix)));



	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	//////////Terrain
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_BOSS, TAG_CP(Prototype_VIBuffer_Terrain),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, L"Height_Boss.bmp")));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_BOSS, TAG_CP(Prototype_Texture_Terrain),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"Terrain.txt")));

	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_BOSS, TAG_CP(Prototype_Texture_GamePlayScene),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"UI_GamePlay.txt")));


	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_DollMaker),
		CDollMaker::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_HandyBoy),
		CHandyBoy::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_HandyGirl),
		CHandyGirl::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_BreakedGazbo),
		CBreakedGazebo::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_ScrechedBlock),
		CScrechedBlock::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_ScrechedDust),
		CScrechedDust::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_HyperVoice),
		CHyperVoice::Create(m_pDevice, m_pDeviceContext)));

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_HandyGirlBullet),
		CHandyGirlBullet::Create(m_pDevice, m_pDeviceContext)));

	

	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(L"TransparencyBall",
		CTransparencyBall::Create(m_pDevice, m_pDeviceContext)));


	FAILED_CHECK(pGameInstance->Add_GameObject_Prototype(L"StageBoss_SpwanBoss",
		CStageBoss_SpwanBoss::Create(m_pDevice, m_pDeviceContext)));



	RELEASE_INSTANCE(CGameInstance);
	EnterCriticalSection(_CriSec);
	m_iLoadingMaxCount = 1;
	m_iLoadingProgressCount = 0;
	LeaveCriticalSection(_CriSec);

	EnterCriticalSection(_CriSec);
	m_bIsLoadingFinished = true;
	LeaveCriticalSection(_CriSec);


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


	//////////////////////////////////////////////////////////////////////////퀸타워
	TransformMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QueenTower_BrokenC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QueenTower", "QueenTower_BrokenC.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);


	TransformMatrix = XMMatrixScaling(0.000075f, 0.000075f, 0.000075f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_QMaze),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "QMaze", "QMaze.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);


	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_ClockworkGears_DecoC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "ClockworkGears_DecoC.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_GiantGearPlatsA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "GiantGearPlatsA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterBlockC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterBlockC.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterBlocksE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterBlocksE.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterBlocksG),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterBlocksG.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterBlocksH),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterBlocksH.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterBuildingE_Door),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterBuildingE_Door.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterDoorA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterDoorA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterDoorC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterDoorC.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterExtTowerA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterExtTowerA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterExtTowerB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterExtTowerB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterExtTowerD),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterExtTowerD.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterExtWallB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterExtWallB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterExtWallB_Hole),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterExtWallB_Hole.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterExtWallC_LeftHole),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterExtWallC_LeftHole.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterExtWallC_RightHole),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterExtWallC_RightHole.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterExtWallD),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterExtWallD.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterFloorSystem_OriginalShortA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterFloorSystem_OriginalShortA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterFloorSystem_OriginalShortB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterFloorSystem_OriginalShortB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterFloorSystem_OriginalShortE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterFloorSystem_OriginalShortE.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterFloorSystem_OriginalTallE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterFloorSystem_OriginalTallE.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterFloorSystem_TeaMakerPlat),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterFloorSystem_TeaMakerPlat.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterJunk_M),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterJunk_M.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterJunk_S),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterJunk_S.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterLightsE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterLightsE.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterPillarsC_top),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPillarsC_top.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterPillarsD_base),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPillarsD_base.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterPillarsG),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPillarsG.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterPlat_CheckeI),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPlat_CheckeI.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterPlat_CheckerA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPlat_CheckerA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterPlat_CheckerB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPlat_CheckerB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterPlat_CheckerC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPlat_CheckerC.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterPlat_CheckerE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPlat_CheckerE.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterPlat_CheckerF),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPlat_CheckerF.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterPlat_CheckerG),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPlat_CheckerG.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterPlat_MetalSquare),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPlat_MetalSquare.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterWallBricks_ChunkB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterWallBricks_ChunkB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterWallBricks_HalfA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterWallBricks_HalfA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterWallBricks_HalfB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterWallBricks_HalfB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterWallBricks_Pillar),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterWallBricks_Pillar.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterWallBricks_Rubble),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterWallBricks_Rubble.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterWallBricks_Wall),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterWallBricks_Wall.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Keyhole_Hatter),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "Keyhole_Hatter.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_KeyHoleTurnnels3),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "KeyHoleTurnnels3.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_OrnateGearA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "OrnateGearA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_OrnateGearC),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "OrnateGearC.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_OrnateGrilleB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "OrnateGrilleB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_OrnateJunkA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "OrnateJunkA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_OrnatePlatsA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "OrnatePlatsA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_OrnatePlatsE),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "OrnatePlatsE.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_PipeMachineB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "PipeMachineB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_PipeMessA),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "PipeMessA.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Weights_ChainLong),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "Weights_ChainLong.FBX", TransformMatrix)));







	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Gazebo_Piece01),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "Gazebo_Piece01.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Gazebo_Piece02),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "Gazebo_Piece02.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Gazebo_Piece03),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "Gazebo_Piece03.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Gazebo_Piece04),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "Gazebo_Piece04.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Gazebo_S),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "Gazebo_S.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_QStone_PillarC_Broken),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "QStone_PillarC_Broken.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Qstone_PlatsB),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "Qstone_PlatsB.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Vent_Steam_Mesh),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "Vent_Steam_Mesh.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_VentQStone),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter3", "VentQStone.FBX", TransformMatrix)));



	TransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixTranslation(-0.9f,-0.9f,0);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_CardPiece1),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "CardPiece", "CardPiece01.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixTranslation(0.8f, -1.f, 0);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_CardPiece2),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "CardPiece", "CardPiece02.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixRotationZ(XMConvertToRadians(-30))* XMMatrixTranslation(-0.9f, 1.14f, -0.2f);;
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_CardPiece3),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "CardPiece", "CardPiece03.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationX(XMConvertToRadians(93.0f)) * XMMatrixTranslation(0.76f, 0.97f, -0.34f);;
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_CardPiece4),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "CardPiece", "CardPiece04.FBX", TransformMatrix)));

		
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

		
		

	TransformMatrix = XMMatrixScaling(0.05f, 0.08f, 0.05f)*		XMMatrixRotationZ(XMConvertToRadians(90))* 		XMMatrixTranslation(-8.f, 0, 0);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_HatterPillarsB_column),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Chapter2", "HatterPillarsB_column.FBX", TransformMatrix)));
		
		
		






	TransformMatrix = XMMatrixScaling(0.0002f, 0.0001f, 0.0001f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Slide),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Slide", "DUSlide_Straight.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.0002f, 0.0002f, 0.0002f)*XMMatrixTranslation(0,0.3f,0);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Button),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "ButtonPad", "ButtonPad.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.0002f, 0.0002f, 0.0002f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Button_Frame),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "ButtonPad", "ButtonPad_Frame.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);



	TransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_Protein),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Protein", "Protein.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_ProteinTable),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Protein", "Table.FBX", TransformMatrix)));
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_ProteinTableCover),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "Protein", "Cover.FBX", TransformMatrix)));


	TransformMatrix = XMMatrixScaling(0.0003f, 0.0003f, 0.0003f);
	FAILED_CHECK(pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Mesh_BallMesh),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "BallMesh", "BallMesh.FBX", TransformMatrix)));
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	

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
 