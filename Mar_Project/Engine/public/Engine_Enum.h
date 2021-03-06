#pragma once


namespace Engine {

	enum TerrainTileID{	Tile_None,  Tile_JumpMovable, Tile_ShrinkMovable, Tile_Movable, Tile_DynamicNoneTile, Tile_End };

	enum CollisionTypeID{  CollisionType_Player, CollisionType_PlayerWeapon, CollisionType_Monster, CollisionType_MonsterWeapon,  CollisionType_NPC, CollisionType_DynaicObject, CollisionType_Terrain, CollisionType_END	};

	enum CHANNELID {  CHANNEL_PLAYER, CHANNEL_OBJECT, CHANNEL_EFFECT, CHANNEL_UI, CHANNEL_BGM, CHANNEL_MAXCHANNEL };


	enum TRANSFORMSTATETYPE { PLM_VIEW, PLM_PROJ, PLM_END };
	enum TARGETPOSITIONTYPE { PLV_CAMERA, PLV_CAMLOOK, PLV_PLAYER, PLV_END };
	enum COLLIDERTYPE { COLLIDER_AABB, COLLIDER_OBB, COLLIDER_SPHERE, COLLIDERTYPE_END };

	enum EasingTypeID
	{
		TYPE_Linear = 0,

		TYPE_QuadIn = 1,
		TYPE_QuadOut,
		TYPE_QuadInOut,

		TYPE_CubicIn = 4,
		TYPE_CubicOut,
		TYPE_CubicInOut,

		TYPE_QuarticIn = 7,
		TYPE_QuarticOut,
		TYPE_QuarticInOut,

		TYPE_QuinticIn = 10,
		TYPE_QuinticOut,
		TYPE_QuinticInOut,

		TYPE_SinIn = 13,
		TYPE_SinOut,
		TYPE_SinInOut,

		TYPE_ExpoIn = 16,
		TYPE_ExpoOut,
		TYPE_ExpoInOut,

		TYPE_CircularIn = 19,
		TYPE_CircularOut,
		TYPE_CircularInOut,

		TYPE_ElasticIn = 22,
		TYPE_ElasticOut,
		TYPE_ElasticInOut,

		TYPE_BounceIn = 25,
		TYPE_BounceOut,


		TYPE_End
	};
}

