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
	Prototype_UIImage,
	Prototype_SkyBox,


	Prototype_Camera_Main,
	Prototype_Camera_Editor,

	Prototype_EditorCursor,
	Prototype_Rect,
	Prototype_Cube,
	Prototype_Terrain,
	Prototype_Ball,

	Prototype_Player,

	Prototype_JumpPad,
	Prototype_SteamPad,
	Prototype_HiddenPad,
	Prototype_TeethObj,
	Prototype_RoseObj,




	Prototype_TestObject,
	Prototype_TestObject2,

	Object_Prototype_End,
};



static const _tchar* Tag_Object_Prototype(OBJECTPROTOTYPEID eTag)
{
	switch (eTag)
	{
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

		
	case Prototype_Rect:
		return TEXT("Rect");
		break;
	case Prototype_Cube:
		return TEXT("Cube");
		break;
	case Prototype_Terrain:
		return TEXT("Terrain");
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
		
		
		

	case Prototype_UIImage:
		return TEXT("UI_Image");
		break;

		//////////////////////////////////////////////////////////////////////////
	case Prototype_TestObject:
		return TEXT("TestObject");
		break;
	case Prototype_TestObject2:
		return TEXT("TestObject2");
		break;
	default:
		MSGBOX("Wrong Type Object Prototype");
		return nullptr;
		break;
	}


}
#define  TAG_OP Tag_Object_Prototype


enum MESHTYPEID
{
	Mesh_None,
	Mesh_Player,
	Mesh_Monster,

	Mesh_TestMesh,
	Mesh_TestMesh2,
	Mesh_TestMesh3,
	Mesh_TestMesh4,
	MeshID_End
};



static const _tchar* Tag_MeshID(MESHTYPEID eTag)
{
	switch (eTag)
	{
	case Mesh_None:
		return TEXT("Mesh_None");
		break;

	case Mesh_Monster:
		return TEXT("Monster");
		break;
	case Mesh_Player:
		return TEXT("Player");
		break;








		//////////////////////////////////////////////////////////////////////////
	case Mesh_TestMesh:
		return TEXT("TestMesh");
		break;
	case Mesh_TestMesh2:
		return TEXT("TestMesh2");
		break;
	case Mesh_TestMesh3:
		return TEXT("TestMesh3");
		break;
	case Mesh_TestMesh4:
		return TEXT("TestMesh4");
		break;

	default:
		MSGBOX("Wrong Type Mesh");
		return nullptr;
		break;
	}


}
#define  TAG_MESH Tag_MeshID

enum LAYERID
{
	Layer_Camera_Main,
	Layer_Camera_Editor,
	Layer_SkyBox,
	Layer_Player,
	Layer_Terrain,


	Layer_JumpPad,
	Layer_SteamPad,
	Layer_HiddenPad,
	Layer_TeethObj,
	Layer_RoseObj,

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
		
	case Layer_SkyBox:
		return TEXT("Layer_SkyBox");
		break;
		
	case Layer_Player:
		return TEXT("Layer_Player");
		break;
	case Layer_Terrain:
		return TEXT("Layer_Terrain");
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
	Prototype_VIBuffer_Terrain_1,
	Prototype_VIBuffer_Terrain_2,
	Prototype_VIBuffer_Terrain_3,
	Prototype_VIBuffer_Terrain_4,
	//////////////////////////////////////////////////////////////////////////
	Prototype_Mesh_Player,

	//////////////////////////////////////////////////////////////////////////
	Prototype_Texture_Player,
	Prototype_Texture_SkyBox,
	Prototype_Texture_EditScene,
	Prototype_Texture_Terrain

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

	case Prototype_Mesh_Player:
		return TEXT("Prototype_Component_Mesh_Player");
		break;

		


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
	case 	Prototype_VIBuffer_Terrain_1:
		return TEXT("Prototype_Component_VIBuffer_Terrain_1");
		break;
	case 	Prototype_VIBuffer_Terrain_2:
		return TEXT("Prototype_Component_VIBuffer_Terrain_2");
		break;
	case 	Prototype_VIBuffer_Terrain_3:
		return TEXT("Prototype_Component_VIBuffer_Terrain_3");
		break;
	case 	Prototype_VIBuffer_Terrain_4:
		return TEXT("Prototype_Component_VIBuffer_Terrain_4");
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


