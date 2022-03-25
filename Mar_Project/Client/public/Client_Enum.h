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
	Prototype_Camera_Main, 
	Prototype_Player,


	Prototype_UIImage,


	Prototype_TestObject,
	Prototype_TestObject2,

	Object_Prototype_End,
};



static const _tchar* Tag_Object_Prototype(OBJECTPROTOTYPEID eTag)
{
	switch (eTag)
	{
	case Prototype_Camera_Main:
		return TEXT("Camera_Main");
		break;
	case Prototype_Player:
		return TEXT("Player");
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
	Layer_Player,

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
	case Layer_Player:
		return TEXT("Layer_Player");
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
	//////////////////////////////////////////////////////////////////////////
	Prototype_VIBuffer_Rect,
	Prototype_VIBuffer_Cube,
	//////////////////////////////////////////////////////////////////////////
	Prototype_Texture_Player,
	Prototype_Texture_SkyBox,

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




	case Prototype_VIBuffer_Rect:
		return TEXT("Prototype_Component_VIBuffer_Rect");
		break;

	case Prototype_VIBuffer_Cube:
		return TEXT("Prototype_Component_VIBuffer_Cube");
		break;




	case Prototype_Texture_Player:
		return TEXT("Prototype_Component_Texture_Player");
		break;
	case Prototype_Texture_SkyBox:
		return TEXT("Prototype_Component_Texture_SkyBox");
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


