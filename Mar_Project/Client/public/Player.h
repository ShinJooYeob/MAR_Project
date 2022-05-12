#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CCamera;
END


BEGIN(Client)
class CWeapon;

class CPlayer final : public CGameObject
{
public:
	enum eWeaponState
	{
		Weapon_None = 0, Weapon_Knife = 47, Weapon_Grinder = 73, Weapon_Horse = 89, Weapon_Teapot = 113, Weapon_Umbrella = 134, Weapon_End
		//Weapon_None = 0, Weapon_Knife =88888888, Weapon_Grinder = 45651, Weapon_Horse = 387137, Weapon_Teapot = 123123, Weapon_Umbrella = 47, Weapon_End
	};


private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;
	HRESULT SetUp_Weapon();
	HRESULT Renew_Player(_float3 Position);
public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;

	void	Add_Dmg_to_Player(_uint iDmgAmount);
	void  Change_Weapon(_uint WeaponIndex);

	_float3	Get_FirePos() { return m_vBulletFirePos; };
	void		Set_FirePos(_float3 vPos) { m_vBulletFirePos = vPos; };

	void Add_JumpForce(_float JumpPower,_uint IsKindsOfJump = false);
	void Add_Force(_float3 vDir, _float Power);
	HRESULT Calculate_Force(_bool* _IsClientQuit, CRITICAL_SECTION* _CriSec);

	_float	Get_SmallVisualTime() { return m_fSmallVisualTime; }
	_float Get_CharedGauge() { return m_fCharedGauge; };
	_double Get_GrinderCoolGauge() { return m_fGrinderCoolTime; };
	_bool Get_IsNeedToGrinderCooling() { return m_bNeedToGrinderCooling; };
	_bool Get_IsNeedToTeapotCooling() { return m_bIsCoolTime; };
	
	_uint Get_NowWeaponIndex() {return m_iWeaponModelIndex;	}

	_bool	Get_IsAttached() { return m_bIsAttached; };
	_bool   Set_IsAttached(_bool bBool);
	
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModel = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

private:
	CCamera*			m_pMainCamera = nullptr;
	_float3				m_CamDegreeAngle = _float3(20.f,0,-5.f);

	/*For. Jump*/
	_float				m_fMaxJumpPower = 0;
	_float				m_fJumpPower = 0;
	_double				m_LevitationTime = 0;
	_uint				m_iJumpCount = 0;

	/*For Shrink*/
	_float				m_fSmallScale = 1.f;
	_float				m_fSmallPassedTime = 0;
	_float				m_fSmallVisualTime = 0;

	/*For Dash*/
	_float3				m_vDashDir;
	_float				m_fDashPower= 0;
	_float				m_fDashPassedTime = 0;

	/*For Bomb*/
	_double				m_BombCoolTime = 0;

	/*For Rock On*/
	_bool				m_bIsRockOn = false;
	CGameObject*		m_pRockOnMonster = nullptr;


	/*For Bullet*/
	_double				m_BulletNormalInterver = 0;


	/*For Weapon*/
	eWeaponState		m_eNowWeapon = Weapon_None;

	_bool				m_bIsAttackClicked = false;
	_uint				m_iAttackCount = 0;
	_bool				m_bAtkMoveMentChecker[3];

	_bool				m_bIsZoom = false;
	_bool				m_bIsCharged = false;
	_float				m_fCharedGauge = 0;
	_bool				m_bIsCoolTime = false;

	_float				m_fUmbrellaIntro = 0;


	_double				m_fGrinderCoolTime = 0;
	_bool				m_bNeedToGrinderCooling = false;


	/*For AddForce*/
	_bool				m_bIsActived = false;
	_float3				m_vAddedForce = _float3(0, 0, 0);


	/*For HP*/
	_uint				m_iHP = 32;

	/*For Particle*/
	vector<PARTICLEDESC>		m_vecParticleDesc;
	vector<CWeapon*>			m_vecWeapon;
	_uint						m_iWeaponModelIndex = 10;

	_float3					m_vBulletFirePos;
	ATTACHBONEMATRIX_PTR m_tCollisionAttachPtr[2];

	/*For Attached*/
	_bool				m_bIsAttached = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstTable();
	HRESULT Ready_ParticleDesc();
	HRESULT Manage_CoolTime(_double fDeltaTime);
	

	HRESULT Input_Keyboard(_double fDeltaTime);
	
	/*All Weapon*/
	HRESULT Dash_Update(_double fDeltaTime, CGameInstance* pInstance, _float TotalDashTime = 0.5f);
	HRESULT RockOn_Update(_double fDeltaTime, CGameInstance* pInstance);
	
	/*NoneWeapon*/
	HRESULT Smalling_Update(_double fDeltaTime, CGameInstance* pInstance);
	HRESULT Plant_ClockBomb(_double fDeltaTime, CGameInstance* pInstance);
	HRESULT Move_Update(_double fDeltaTime, CGameInstance* pInstance);
	HRESULT Jump_Update(_double fDeltaTime, CGameInstance* pInstance);


	/*Knife*/
	HRESULT Move_Update_Knife(_double fDeltaTime, CGameInstance* pInstance);
	HRESULT Jump_Update_Knife(_double fDeltaTime, CGameInstance* pInstance);
	HRESULT Attack_Update_Knife(_double fDeltaTime, CGameInstance* pInstance);


	/*Grinder*/
	HRESULT Move_Update_Grinder(_double fDeltaTime, CGameInstance* pInstance);
	HRESULT Jump_Update_Grinder(_double fDeltaTime, CGameInstance* pInstance);
	HRESULT Attack_Update_Grinder(_double fDeltaTime, CGameInstance* pInstance);
	HRESULT Lunch_Bullet(_double fDeltaTime, CGameInstance* pInstance);


	/*Horse*/
	HRESULT Move_Update_Horse(_double fDeltaTime, CGameInstance* pInstance);
	HRESULT Jump_Update_Horse(_double fDeltaTime, CGameInstance* pInstance);
	HRESULT Attack_Update_Horse(_double fDeltaTime, CGameInstance* pInstance);


	/*Teapot*/
	HRESULT Move_Update_Teapot(_double fDeltaTime, CGameInstance* pInstance);
	HRESULT Jump_Update_Teapot(_double fDeltaTime, CGameInstance* pInstance);
	HRESULT Attack_Update_Teapot(_double fDeltaTime, CGameInstance* pInstance);
	HRESULT Lunch_Grenade(_double fDeltaTime, CGameInstance* pInstance);
	
	/*Umbrella*/
	HRESULT Move_Update_Umbrella(_double fDeltaTime, CGameInstance* pInstance);
	HRESULT Jump_Update_Umbrella(_double fDeltaTime, CGameInstance* pInstance);
	HRESULT Attack_Update_Umbrella(_double fDeltaTime, CGameInstance* pInstance);
	
	HRESULT Set_Player_On_Terrain();
	HRESULT Set_Player_On_Terrain_DontPutonJumpMovable();
	HRESULT Set_Player_On_Slieder(_double fDeltatime);
	HRESULT Set_Camera_On_Player(_double fDeltaTime);


public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END