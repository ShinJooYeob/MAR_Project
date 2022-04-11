#pragma once


enum SCENEID
{
	SCENE_STATIC,
	SCENE_LOBY,
	SCENE_LOADING,
	SCENE_STAGESELECT,
	SCENE_STAGE1,

	SCENE_EDIT,
	SCENE_END
};



enum OBJECTPROTOTYPEID
{
	Prototype_Bullet_Normal,
	Prototype_Bullet_Grenade,

	Prototype_UIImage,
	Prototype_SkyBox,
	Prototype_Player,


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

	Layer_Terrain,
	Layer_WireTerrain,


	Layer_JumpPad,
	Layer_SteamPad,
	Layer_HiddenPad,
	Layer_TeethObj,
	Layer_RoseObj,

	Layer_StaticMapObj,


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
		
	case Layer_StaticMapObj:
		return TEXT("Layer_StaticMapObj");
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

	Prototype_Mesh_Player,
	Prototype_Mesh_Executor,
	Prototype_Mesh_Grunt,
	Prototype_Mesh_WaspArrow,
	Prototype_Mesh_WaspInk,
	Prototype_Mesh_Eyepot,
	//////////////////////////////////////////////////////////////////////////
	Prototype_Texture_Player,
	Prototype_Texture_SkyBox,
	Prototype_Texture_EditScene,
	Prototype_Texture_Terrain,
	Prototype_Texture_Edit_Terrain

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


