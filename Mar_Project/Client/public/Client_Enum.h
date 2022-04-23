#pragma once


enum SCENEID
{
	SCENE_STATIC,
	SCENE_LOBY,
	SCENE_LOADING,
	SCENE_STAGESELECT,
	SCENE_STAGE1,
	SCENE_STAGE2,
	SCENE_STAGE3,

	SCENE_EDIT,
	SCENE_END
};



enum OBJECTPROTOTYPEID
{
	Prototype_Bullet_Normal,
	Prototype_Bullet_Grenade,

	Prototype_UIImage,

	Prototype_UIGamePlay,
	Prototype_UILoading,
	Prototype_UILoby,
	Prototype_UISetting,


	Prototype_SkyBox,
	Prototype_Player,

	Prototype_Executor,
	Prototype_Grunt,
	Prototype_Eyepot,
	Prototype_WaspArrow,
	Prototype_WaspInk,


	Prototype_Camera_Main,
	Prototype_Camera_Editor,
	Prototype_WireTerrain,

	Prototype_EditorCursor,
	Prototype_Rect,
	Prototype_Cube,
	Prototype_Terrain,
	Prototype_Ball,


	Prototype_JumpPad,
	Prototype_SteamPad,
	Prototype_HiddenPad,
	Prototype_TeethObj,
	Prototype_RoseObj,
	Prototype_BreakableObj,
	Prototype_BreakablePiece,
	Prototype_SlideObj,


	Prototype_StaticMapObject,



	Object_Prototype_End,
};



static const _tchar* Tag_Object_Prototype(OBJECTPROTOTYPEID eTag)
{
	switch (eTag)
	{
	case Prototype_Bullet_Normal:
		return TEXT("Bullet_Normal");
		break;
	case Prototype_Bullet_Grenade:
		return TEXT("Bullet_Grenade");
		break;

	case Prototype_UILoading:
		return TEXT("Prototype_UILoading");
		break;

	case Prototype_UILoby:
		return TEXT("Prototype_UILoby");
		break;
	case Prototype_UISetting:
		return TEXT("Prototype_UISetting");
		break;



	case Prototype_Camera_Editor:
		return TEXT("Camera_Editor");
		break;
	case Prototype_Camera_Main:
		return TEXT("Camera_Main");
		break;
	case Prototype_SkyBox:
		return TEXT("SkyBox");
		break;
	case Prototype_EditorCursor:
		return TEXT("EditorCursor");
		break;
	case Prototype_UIImage:
		return TEXT("UI_Image");
		break;
	case Prototype_UIGamePlay:
		return TEXT("UIGamePlay");
		break;


	case Prototype_Rect:
		return TEXT("Rect");
		break;
	case Prototype_Cube:
		return TEXT("Cube");
		break;
	case Prototype_Terrain:
		return TEXT("Terrain");
		break;
	case Prototype_WireTerrain:
		return TEXT("WireTerrain");
		break;

	case Prototype_Ball:
		return TEXT("Ball");
		break;
	case Prototype_Player:
		return TEXT("Player");
		break;


	case Prototype_Executor:
		return TEXT("Prototype_Executor");
		break;
	case Prototype_Grunt:
		return TEXT("Prototype_Grunt");
		break;
	case Prototype_Eyepot:
		return TEXT("Prototype_Eyepot");
		break;
	case Prototype_WaspArrow:
		return TEXT("Prototype_WaspArrow");
		break;
	case Prototype_WaspInk:
		return TEXT("Prototype_WaspInk");
		break;




	case Prototype_JumpPad:
		return TEXT("JumpPad");
		break;
	case Prototype_SteamPad:
		return TEXT("SteamPad");
		break;

	case Prototype_HiddenPad:
		return TEXT("HiddenPad");
		break;
	case Prototype_TeethObj:
		return TEXT("TeethObj");
		break;
	case Prototype_RoseObj:
		return TEXT("RoseObj");
		break;


	case Prototype_BreakableObj:
		return TEXT("Prototype_BreakableObj");
		break;
	case Prototype_BreakablePiece:
		return TEXT("Prototype_BreakablePiece");
		break;
	case Prototype_SlideObj:
		return TEXT("Prototype_SlideObj");
		break;





	case Prototype_StaticMapObject:
		return TEXT("StaticMapObject");
		break;
		//////////////////////////////////////////////////////////////////////////


	default:
		MSGBOX("Wrong Type Object Prototype");
		return nullptr;
		break;
	}


}
#define  TAG_OP Tag_Object_Prototype

//
//enum MESHTYPEID
//{
//	Mesh_None,
//	Mesh_Player,
//	Mesh_Monster,
//
//
//
//	Mesh_AlgaeRock_Ledge,
//	Mesh_AlgaeRock_Pillar,
//	Mesh_AlgaeRock_Wall,
//
//	MeshID_End
//};
//
//
//
//static const _tchar* Tag_MeshID(MESHTYPEID eTag)
//{
//	switch (eTag)
//	{
//	case Mesh_None:
//		return TEXT("Mesh_None");
//		break;
//
//	case Mesh_Monster:
//		return TEXT("Monster");
//		break;
//	case Mesh_Player:
//		return TEXT("Player");
//		break;
//
//
//
//
//
//
//
//
//		//////////////////////////////////////////////////////////////////////////
//	case Mesh_AlgaeRock_Ledge:
//		return TEXT("Mesh_AlgaeRock_Ledge");
//		break;
//	case Mesh_TestMesh2:
//		return TEXT("TestMesh2");
//		break;
//	case Mesh_TestMesh3:
//		return TEXT("TestMesh3");
//		break;
//	case Mesh_TestMesh4:
//		return TEXT("TestMesh4");
//		break;
//
//	default:
//		MSGBOX("Wrong Type Mesh");
//		return nullptr;
//		break;
//	}
//
//
//}
//#define  TAG_MESH Tag_MeshID

enum LAYERID
{
	Layer_Camera_Main,
	Layer_Camera_Editor,
	Layer_SkyBox,
	Layer_Player,
	Layer_Bullet,

	Layer_Monster,

	Layer_Terrain,
	Layer_WireTerrain,


	Layer_JumpPad,
	Layer_SteamPad,
	Layer_HiddenPad,
	Layer_TeethObj,
	Layer_RoseObj,
	Layer_Breakable,
	Layer_BreakablePiece,
	Layer_SlideObj,

	Layer_StaticMapObj,


	Layer_UI_GamePlay,



	Layer_UI_IMG,
	Layer_UI_BTN,
	Layer_UI_SLIDE,
	Layer_UI_INPUT

};

static const _tchar* Tag_Layer(LAYERID eTag)
{
	switch (eTag)
	{
	case Layer_Camera_Main:
		return TEXT("Layer_Camera_Main");
		break;
	case Layer_Camera_Editor:
		return TEXT("Layer_Camera_Editor");
		break;

	case Layer_Bullet:
		return TEXT("Layer_Bullet");
		break;
	case Layer_SkyBox:
		return TEXT("Layer_SkyBox");
		break;
	case Layer_Terrain:
		return TEXT("Layer_Terrain");
		break;
	case Layer_WireTerrain:
		return TEXT("Layer_WireTerrain");
		break;

	case Layer_Monster:
		return TEXT("Layer_Monster");
		break;
	case Layer_Player:
		return TEXT("Layer_Player");
		break;
	case Layer_JumpPad:
		return TEXT("Layer_JumpPad");
		break;
	case Layer_SteamPad:
		return TEXT("Layer_SteamPad");
		break;
	case Layer_HiddenPad:
		return TEXT("Layer_HiddenPad");
		break;
	case Layer_TeethObj:
		return TEXT("Layer_TeethObj");
		break;
	case Layer_RoseObj:
		return TEXT("Layer_RoseObj");
		break;
	case Layer_Breakable:
		return TEXT("Layer_Breakable");
		break;
	case Layer_BreakablePiece:
		return TEXT("Layer_BreakablePiece");
		break;
		
	case Layer_SlideObj:
		return TEXT("Layer_SlideObj");
		break;



	case Layer_StaticMapObj:
		return TEXT("Layer_StaticMapObj");
		break;

	case Layer_UI_GamePlay:
		return TEXT("Layer_UI_GamePlay");
		break;

	case Layer_UI_IMG:
		return TEXT("Layer_UI_IMG");
		break;
	case Layer_UI_BTN:
		return TEXT("Layer_UI_BTN");
		break;
	case Layer_UI_SLIDE:
		return TEXT("Layer_UI_SLIDE");
		break;
	case Layer_UI_INPUT:
		return TEXT("Layer_UI_INPUT");
		break;

		//////////////////////////////////////////////////////////////////////////
	default:
		MSGBOX("Wrong Type Layer");
		return nullptr;
		break;
	}


}
#define  TAG_LAY Tag_Layer

enum COMPONENTPROTOTYPEID
{
	Prototype_Renderer,
	Prototype_Transform,
	Prototype_Collision,
	Prototype_Shader_VT,
	Prototype_Shader_VCT,
	Prototype_Shader_VNT,
	Prototype_Shader_VNCT,
	Prototype_Shader_VAM,
	Prototype_Shader_VNAM,
	//////////////////////////////////////////////////////////////////////////
	Prototype_VIBuffer_Rect,
	Prototype_VIBuffer_Cube,
	Prototype_VIBuffer_Terrain,
	Prototype_VIBuffer_Terrain_2,
	Prototype_VIBuffer_Terrain_3,
	Prototype_VIBuffer_Terrain_Edit,
	//////////////////////////////////////////////////////////////////////////
	Prototype_Mesh_None,
	Prototype_Mesh_AlgaeRock_Ledge,
	Prototype_Mesh_AlgaeRock_Pillar,
	Prototype_Mesh_AlgaeRock_Wall,

	Prototype_Mesh_GloryGrowthA,
	Prototype_Mesh_GloryGrowthB,
	Prototype_Mesh_GloryGrowthC,
	Prototype_Mesh_GloryGrowthD,
	Prototype_Mesh_GloryTree_Base,
	Prototype_Mesh_GloryTree_Main,
	Prototype_Mesh_GloryTree_MainB,
	Prototype_Mesh_GloryTree_Shell,
	Prototype_Mesh_Kelp_Single,
	Prototype_Mesh_Kelp_Wall,
	Prototype_Mesh_Kelp_WallFar,
	Prototype_Mesh_KelpWallBack,


	Prototype_Mesh_GiantToys_Jacks,
	Prototype_Mesh_GiantToys_MarblesA,

	Prototype_Mesh_KeyHole,

	Prototype_Mesh_Moai_Gate,
	Prototype_Mesh_Moai_Plat,


	Prototype_Mesh_QBattleTower,
	Prototype_Mesh_QBattleTowerBaseDie,
	Prototype_QBattleTowerParticleA,
	Prototype_QBattleTowerParticleB,
	Prototype_QBattleTowerPieceA,
	Prototype_QBattleTowerPieceB,
	Prototype_QBattleTowerPieceC,
	Prototype_SK_QBattleTowerTentacle_Giant,
	Prototype_SK_QBattleTowerTentacle_Group,
	Prototype_SK_QBattleTowerTentacle_Thin,

	Prototype_SK_QCannon,

	Prototype_QFleshFloorA,
	Prototype_QFleshFloorB,
	Prototype_QFleshFloorC,
	Prototype_QfleshStairs,


	Prototype_QGiantBreakableGate_Broken,
	Prototype_QGiantBreakableGate_Flesh,

	Prototype_QOuterwall_WallB,
	Prototype_QOuterwall_WallD,
	Prototype_QOuterwall_WallE,


	Prototype_QRocksB,
	Prototype_QRocksC,
	Prototype_QRocksD,
	Prototype_QRocksE,

	Prototype_TearsRock,

	Prototype_GiantUnderseaRock_Holey,
	Prototype_UnderseaRock_Ledge,
	Prototype_UnderseaRock_Pile,
	Prototype_UnderseaRock_Pillar,
	Prototype_UnderseaRock_Plat,

	Prototype_WaterLogA,
	Prototype_WaterLogB,
	Prototype_WaterLogC,







	Prototype_Mesh_Player,
	Prototype_Mesh_Executor,
	Prototype_Mesh_Grunt,
	Prototype_Mesh_WaspArrow,
	Prototype_Mesh_WaspInk,
	Prototype_Mesh_Eyepot,

	Prototype_Mesh_JumpPad,
	Prototype_Mesh_HealthRose,
	Prototype_Mesh_Tooth,
	Prototype_Mesh_GoldenTooth,
	Prototype_Mesh_SteamPad,

	Prototype_Mesh_GiantToys_Die,
	Prototype_Mesh_GiantToys_DominoC,
	Prototype_Mesh_GiantToys_DominoE,
	Prototype_Mesh_GiantToys_DominoStair,

	Prototype_Mesh_BreakableBox,
	Prototype_Mesh_BreakableBoxPiece1,
	Prototype_Mesh_BreakableBoxPiece2,
	Prototype_Mesh_BreakableBoxPiece3,
	Prototype_Mesh_BreakableBoxPiece4,
	Prototype_Mesh_BreakableBoxPiece5,
	Prototype_Mesh_BreakableBoxPiece6,
	Prototype_Mesh_BreakableBarrel,
	Prototype_Mesh_BreakableBarrelPiece1,
	Prototype_Mesh_BreakableBarrelPiece2,
	Prototype_Mesh_BreakableBarrelPiece3,
	Prototype_Mesh_BreakableBarrelPiece4,
	Prototype_Mesh_BreakableBarrelPiece5,
	Prototype_Mesh_GiftBasket,


	//////////////////////////////////////////////////////////////////////////
	Prototype_Texture_Player,
	Prototype_Texture_SkyBox,
	Prototype_Texture_EditScene,
	Prototype_Texture_Terrain,
	Prototype_Texture_Edit_Terrain,
	Prototype_Texture_DefaultUI,
	Prototype_Texture_GamePlayScene,
	Prototype_Texture_LoadingScene,
	Prototype_Texture_LobyScene,
	Prototype_Texture_SettingScene,


};
static const _tchar* Tag_Component_Prototype(COMPONENTPROTOTYPEID eTag)
{
	switch (eTag)
	{
	case Prototype_Renderer:
		return TEXT("Prototype_Component_Renderer");
		break;
	case Prototype_Transform:
		return TEXT("Prototype_Component_Transform");
		break;
	case Prototype_Collision:
		return TEXT("Prototype_Component_Collision");
		break;


	case Prototype_Shader_VT:
		return TEXT("Prototype_Component_Shader_VTXTEX");
		break;
	case Prototype_Shader_VCT:
		return TEXT("Prototype_Component_Shader_VTXCUBETEX");
		break;

	case Prototype_Shader_VNT:
		return TEXT("Prototype_Component_Shader_VTXNORMTEX");
		break;

	case Prototype_Shader_VNCT:
		return TEXT("Prototype_Component_Shader_VTXNORMCUBETEX");
		break;
	case Prototype_Shader_VAM:
		return TEXT("Prototype_Component_Shader_VTXANIMMODEL");
		break;
	case Prototype_Shader_VNAM:
		return TEXT("Prototype_Component_Shader_VTXNONANIMMODEL");
		break;



	case Prototype_VIBuffer_Rect:
		return TEXT("Prototype_Component_VIBuffer_Rect");
		break;

	case Prototype_VIBuffer_Cube:
		return TEXT("Prototype_Component_VIBuffer_Cube");
		break;


		//메쉬////////////////////////////////////////////////////////////////////////

	case Prototype_Mesh_None:
		return TEXT("Mesh_None");
		break;

	case Prototype_Mesh_AlgaeRock_Ledge:
		return TEXT("Mesh_AlgaeRock_Ledge");
		break;
	case Prototype_Mesh_AlgaeRock_Pillar:
		return TEXT("Mesh_AlgaeRock_Pillar");
		break;
	case Prototype_Mesh_AlgaeRock_Wall:
		return TEXT("Mesh_AlgaeRock_Wall");
		break;

	case Prototype_Mesh_GloryGrowthA:
		return TEXT("Mesh_GloryGrowthA");
		break;
	case Prototype_Mesh_GloryGrowthB:
		return TEXT("Mesh_GloryGrowthB");
		break;
	case Prototype_Mesh_GloryGrowthC:
		return TEXT("Mesh_GloryGrowthC");
		break;
	case Prototype_Mesh_GloryGrowthD:
		return TEXT("Mesh_GloryGrowthD");
		break;
	case Prototype_Mesh_GloryTree_Base:
		return TEXT("Mesh_GloryTree_Base");
		break;
	case Prototype_Mesh_GloryTree_Main:
		return TEXT("Mesh_GloryTree_Main");
		break;
	case Prototype_Mesh_GloryTree_MainB:
		return TEXT("Mesh_GloryTree_MainB");
		break;
	case Prototype_Mesh_GloryTree_Shell:
		return TEXT("Mesh_GloryTree_Shell");
		break;
	case Prototype_Mesh_Kelp_Single:
		return TEXT("Mesh_Kelp_Single");
		break;
	case Prototype_Mesh_Kelp_Wall:
		return TEXT("Mesh_Kelp_Wall");
		break;
	case Prototype_Mesh_Kelp_WallFar:
		return TEXT("Mesh_Kelp_WallFar");
		break;
	case Prototype_Mesh_KelpWallBack:
		return TEXT("Mesh_KelpWallBack");
		break;

	case Prototype_Mesh_GiantToys_Die:
		return TEXT("Mesh_GiantToys_Die");
		break;
	case Prototype_Mesh_GiantToys_DominoC:
		return TEXT("Mesh_GiantToys_DominoC");
		break;
	case Prototype_Mesh_GiantToys_DominoE:
		return TEXT("Mesh_GiantToys_DominoE");
		break;
	case Prototype_Mesh_GiantToys_DominoStair:
		return TEXT("Mesh_GiantToys_DominoStair");
		break;
	case Prototype_Mesh_GiantToys_Jacks:
		return TEXT("Mesh_GiantToys_Jacks");
		break;
	case Prototype_Mesh_GiantToys_MarblesA:
		return TEXT("Mesh_GiantToys_MarblesA");
		break;

	case Prototype_Mesh_KeyHole:
		return TEXT("Mesh_KeyHole");
		break;

	case Prototype_Mesh_Moai_Gate:
		return TEXT("Mesh_Moai_Gate");
		break;
	case Prototype_Mesh_Moai_Plat:
		return TEXT("Mesh_Moai_Plat");
		break;

	case Prototype_Mesh_QBattleTower:
		return TEXT("Mesh_QBattleTower");
		break;
	case Prototype_Mesh_QBattleTowerBaseDie:
		return TEXT("Mesh_QBattleTowerBaseDie");
		break;
	case Prototype_QBattleTowerParticleA:
		return TEXT("QBattleTowerParticleA");
		break;
	case Prototype_QBattleTowerParticleB:
		return TEXT("QBattleTowerParticleB");
		break;
	case Prototype_QBattleTowerPieceA:
		return TEXT("QBattleTowerPieceA");
		break;
	case Prototype_QBattleTowerPieceB:
		return TEXT("QBattleTowerPieceB");
		break;
	case Prototype_QBattleTowerPieceC:
		return TEXT("QBattleTowerPieceC");
		break;
	case Prototype_SK_QBattleTowerTentacle_Giant:
		return TEXT("SK_QBattleTowerTentacle_Giant");
		break;
	case Prototype_SK_QBattleTowerTentacle_Group:
		return TEXT("SK_QBattleTowerTentacle_Group");
		break;
	case Prototype_SK_QBattleTowerTentacle_Thin:
		return TEXT("SK_QBattleTowerTentacle_Thin");
		break;

	case Prototype_SK_QCannon:
		return TEXT("SK_QCannon");
		break;

	case Prototype_QFleshFloorA:
		return TEXT("QFleshFloorA");
		break;
	case Prototype_QFleshFloorB:
		return TEXT("QFleshFloorB");
		break;
	case Prototype_QFleshFloorC:
		return TEXT("QFleshFloorC");
		break;
	case Prototype_QfleshStairs:
		return TEXT("QfleshStairs");
		break;

	case Prototype_QGiantBreakableGate_Broken:
		return TEXT("QGiantBreakableGate_Broken");
		break;
	case Prototype_QGiantBreakableGate_Flesh:
		return TEXT("QGiantBreakableGate_Flesh");
		break;

	case Prototype_QOuterwall_WallB:
		return TEXT("QOuterwall_WallB");
		break;
	case Prototype_QOuterwall_WallD:
		return TEXT("QOuterwall_WallD");
		break;
	case Prototype_QOuterwall_WallE:
		return TEXT("QOuterwall_WallE");
		break;

	case Prototype_QRocksB:
		return TEXT("QRocksB");
		break;
	case Prototype_QRocksC:
		return TEXT("QRocksC");
		break;
	case Prototype_QRocksD:
		return TEXT("QRocksD");
		break;
	case Prototype_QRocksE:
		return TEXT("QRocksE");
		break;

	case Prototype_TearsRock:
		return TEXT("TearsRock");
		break;


	case Prototype_GiantUnderseaRock_Holey:
		return TEXT("GiantUnderseaRock_Holey");
		break;
	case Prototype_UnderseaRock_Ledge:
		return TEXT("UnderseaRock_Ledge");
		break;
	case Prototype_UnderseaRock_Pile:
		return TEXT("UnderseaRock_Pile");
		break;
	case Prototype_UnderseaRock_Pillar:
		return TEXT("UnderseaRock_Pillar");
		break;
	case Prototype_UnderseaRock_Plat:
		return TEXT("UnderseaRock_Plat");
		break;


	case Prototype_WaterLogA:
		return TEXT("WaterLogA");
		break;
	case Prototype_WaterLogB:
		return TEXT("WaterLogB");
		break;
	case Prototype_WaterLogC:
		return TEXT("WaterLogC");
		break;



	case Prototype_Mesh_Player:
		return TEXT("Mesh_Player");
		break;
	case Prototype_Mesh_Executor:
		return TEXT("Mesh_Executor");
		break;
	case Prototype_Mesh_Grunt:
		return TEXT("Mesh_Grunt");
		break;
	case Prototype_Mesh_WaspArrow:
		return TEXT("Mesh_WaspArrow");
		break;
	case Prototype_Mesh_WaspInk:
		return TEXT("Mesh_WaspInk");
		break;
	case Prototype_Mesh_Eyepot:
		return TEXT("Mesh_Eyepot");
		break;


	case Prototype_Mesh_JumpPad:
		return TEXT("Prototype_Mesh_JumpPad");
		break;

	case Prototype_Mesh_HealthRose:
		return TEXT("Prototype_Mesh_HealthRose");
		break;

	case Prototype_Mesh_Tooth:
		return TEXT("Prototype_Mesh_Tooth");
		break;

	case Prototype_Mesh_GoldenTooth:
		return TEXT("Prototype_Mesh_GoldenTooth");
		break;

	case Prototype_Mesh_SteamPad:
		return TEXT("Prototype_Mesh_SteamPad");
		break;


	case Prototype_Mesh_BreakableBox:
		return TEXT("Prototype_Mesh_BreakableBox");
		break;
	case Prototype_Mesh_BreakableBoxPiece1:
		return TEXT("Prototype_Mesh_BreakableBoxPiece1");
		break;
	case Prototype_Mesh_BreakableBoxPiece2:
		return TEXT("Prototype_Mesh_BreakableBoxPiece2");
		break;
	case Prototype_Mesh_BreakableBoxPiece3:
		return TEXT("Prototype_Mesh_BreakableBoxPiece3");
		break;
	case Prototype_Mesh_BreakableBoxPiece4:
		return TEXT("Prototype_Mesh_BreakableBoxPiece4");
		break;
	case Prototype_Mesh_BreakableBoxPiece5:
		return TEXT("Prototype_Mesh_BreakableBoxPiece5");
		break;
	case Prototype_Mesh_BreakableBoxPiece6:
		return TEXT("Prototype_Mesh_BreakableBoxPiece6");
		break;

	case Prototype_Mesh_BreakableBarrel:
		return TEXT("Prototype_Mesh_BreakableBarrel");
		break;
	case Prototype_Mesh_BreakableBarrelPiece1:
		return TEXT("Prototype_Mesh_BreakableBarrelPiece1");
		break;
	case Prototype_Mesh_BreakableBarrelPiece2:
		return TEXT("Prototype_Mesh_BreakableBarrelPiece2");
		break;
	case Prototype_Mesh_BreakableBarrelPiece3:
		return TEXT("Prototype_Mesh_BreakableBarrelPiece3");
		break;
	case Prototype_Mesh_BreakableBarrelPiece4:
		return TEXT("Prototype_Mesh_BreakableBarrelPiece4");
		break;
	case Prototype_Mesh_BreakableBarrelPiece5:
		return TEXT("Prototype_Mesh_BreakableBarrelPiece5");
		break;
	case Prototype_Mesh_GiftBasket:
		return TEXT("Prototype_Mesh_GiftBasket");
		break;



		/////////////////텍스처////////////////////////////////////////////////////////


	case Prototype_Texture_Player:
		return TEXT("Prototype_Component_Texture_Player");
		break;
	case Prototype_Texture_SkyBox:
		return TEXT("Prototype_Component_Texture_SkyBox");
		break;

	case Prototype_Texture_EditScene:
		return TEXT("Prototype_Component_Texture_EditScene");

		break;
	case 	Prototype_Texture_Terrain:
		return TEXT("Prototype_Component_Texture_Terrain");

		break;
	case 	Prototype_Texture_Edit_Terrain:
		return TEXT("Prototype_Texture_Edit_Terrain");

	case 	Prototype_Texture_DefaultUI:
		return TEXT("Prototype_Texture_DefaultUI");
		break;


	case 	Prototype_Texture_GamePlayScene:
		return TEXT("Prototype_Texture_GamePlayScene");
		break;

	case 	Prototype_Texture_LoadingScene:
		return TEXT("Prototype_Texture_LoadingScene");
		break;

	case 	Prototype_Texture_LobyScene:
		return TEXT("Prototype_Texture_LobyScene");
		break;

		case
			Prototype_Texture_SettingScene:
				return TEXT("Prototype_Texture_SettingScene");
				break;

				//메쉬////////////////////////////////////////////////////////////////////////







				break;
		case 	Prototype_VIBuffer_Terrain:
			return TEXT("Prototype_Component_VIBuffer_Terrain");
			break;
		case 	Prototype_VIBuffer_Terrain_2:
			return TEXT("Prototype_Component_VIBuffer_Terrain_2");
			break;
		case 	Prototype_VIBuffer_Terrain_3:
			return TEXT("Prototype_Component_VIBuffer_Terrain_3");
			break;
		case 	Prototype_VIBuffer_Terrain_Edit:
			return TEXT("Prototype_Component_VIBuffer_Terrain_Edit");
			break;



			//////////////////////////////////////////////////////////////////////////
		default:
			MSGBOX("Wrong Type Layer");
			return nullptr;
			break;
	}


}

#define  TAG_CP Tag_Component_Prototype

enum COMPONENTID
{
	Com_Renderer,
	Com_VIBuffer,
	Com_Transform,
	Com_Texture,
	Com_Collision,
	Com_Inventory,
	Com_Shader,
	Com_ShaderSub,
	Com_Model,

};
static const _tchar* Tag_Component(COMPONENTID eTag)
{
	switch (eTag)
	{
	case Com_Renderer:
		return TEXT("Com_Renderer");
		break;
	case Com_VIBuffer:
		return TEXT("Com_VIBuffer");
		break;
	case Com_Transform:
		return TEXT("Com_Transform");
		break;
	case Com_Texture:
		return TEXT("Com_Texture");
		break;
	case Com_Collision:
		return TEXT("Com_Collision");
		break;
	case Com_Inventory:
		return TEXT("Com_Inventory");
		break;
	case Com_Shader:
		return TEXT("Com_Shader");
		break;
	case Com_ShaderSub:
		return TEXT("Com_ShaderSub");
		break;
	case Com_Model:
		return TEXT("Com_Model");
		break;



		//////////////////////////////////////////////////////////////////////////
	default:
		MSGBOX("Wrong Type Component");
		return nullptr;
		break;
	}


}
#define  TAG_COM Tag_Component



static const char* Tag_ModelTextureType(_uint eTag)
{
	switch (eTag)
	{
	case aiTextureType_NONE:		return "None";
		break;
	case aiTextureType_DIFFUSE:	return "g_DiffuseTexture";
		break;
	case aiTextureType_SPECULAR:return "g_SpecularTexture";
		break;
	case aiTextureType_AMBIENT:return "g_AmbientTexture";
		break;
	case aiTextureType_EMISSIVE:return "g_EmissiveTexture";
		break;
	case aiTextureType_HEIGHT:return "g_HeightTexture";
		break;
	case aiTextureType_NORMALS:return "g_NormalTexture";
		break;
	case aiTextureType_SHININESS:return "g_ShininessTexture";
		break;
	case aiTextureType_OPACITY:return "g_OpacityTexture";
		break;
	case aiTextureType_DISPLACEMENT:return "g_DisplaceTexture";
		break;
	case aiTextureType_LIGHTMAP:return "g_LightMapTexture";
		break;
	case aiTextureType_REFLECTION:return "g_ReflectTexture";
		break;
	case aiTextureType_BASE_COLOR:return "g_BaseColorTexture";
		break;
	case aiTextureType_NORMAL_CAMERA:return "g_NormalCamTexture";
		break;
	case aiTextureType_EMISSION_COLOR:return "g_EmissionColorTexture";
		break;
	case aiTextureType_METALNESS:return "g_MetalTexture";
		break;
	case aiTextureType_DIFFUSE_ROUGHNESS:return "g_DiffuseRoughTexture";
		break;
	case aiTextureType_AMBIENT_OCCLUSION:return "g_AmbientOcculusionTexture";
		break;
	case aiTextureType_UNKNOWN:return "";
		break;
	case _aiTextureType_Force32Bit:return "";
		break;
	default:
		OutputDebugStringW(L"Wrong Type Texture");
		__debugbreak();
		return nullptr;
		break;
	}
}

#define  MODLETEXTYPE Tag_ModelTextureType















//
//
//enum EasingTypeID
//{
//	TYPE_Linear = 0,
//
//	TYPE_QuadIn = 1,
//	TYPE_QuadOut,
//	TYPE_QuadInOut,
//
//	TYPE_CubicIn = 4,
//	TYPE_CubicOut,
//	TYPE_CubicInOut,
//
//	TYPE_QuarticIn = 7,
//	TYPE_QuarticOut,
//	TYPE_QuarticInOut,
//
//	TYPE_QuinticIn = 10,
//	TYPE_QuinticOut,
//	TYPE_QuinticInOut,
//
//	TYPE_SinIn = 13,
//	TYPE_SinOut,
//	TYPE_SinInOut,
//
//	TYPE_ExpoIn = 16,
//	TYPE_ExpoOut,
//	TYPE_ExpoInOut,
//
//	TYPE_CircularIn = 19,
//	TYPE_CircularOut,
//	TYPE_CircularInOut,
//
//	TYPE_ElasticIn = 22,
//	TYPE_ElasticOut,
//	TYPE_ElasticInOut,
//
//	TYPE_BounceIn = 25,
//	TYPE_BounceOut,
//
//
//	TYPE_End
//};


