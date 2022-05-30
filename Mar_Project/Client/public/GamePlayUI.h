#pragma once

#include "UI.h"

BEGIN(Client)

class CGamePlayUI : public CGameObject
{
protected:
	explicit CGamePlayUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	explicit CGamePlayUI(const CGamePlayUI&  rhs);
	virtual ~CGamePlayUI() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;
	HRESULT Add_Dmg_to_Player(_uint iNowHP,_int iDmg);

	void Set_DrawClockBombUI(_bool bBool);

	void Set_DrawFightUI(_bool bBool);
protected:
	CRenderer*			m_pRendererCom = nullptr;
	class CPlayer*			m_pPlayer = nullptr;

	vector<CUI*>			m_vecUIContainer;
	_float2				m_vOldMousePos;
	
	_bool				m_bDrawClockBomb = false;
	_double				m_PassedClockBombTime = 0;

	_bool				m_bFightUIDraw = false;
	_double				m_PassedFightUITime = 100;

protected:
	HRESULT Change_WeaponUI();
	HRESULT Update_CrossHead();
	HRESULT Update_GrinderHUD();
	HRESULT Update_TeapotHUD(_double fDeltaTime);
	HRESULT Update_ClockBombHUD(_double fDeltaTime);
	HRESULT Update_FightHUD(_double fDeltaTime);

protected:
	HRESULT SetUp_Components();

	HRESULT Ready_Player(CGameInstance* pInstance);

	HRESULT Ready_HpBarBackGround(CGameInstance* pInstance);
	HRESULT Ready_TeethUI(CGameInstance* pInstance);
	HRESULT Ready_HpFlowerPetal(CGameInstance* pInstance);
	HRESULT Ready_WeaponChageRing(CGameInstance* pInstance);
	HRESULT Ready_WeaponChageCursor(CGameInstance* pInstance);
	HRESULT Ready_WeaponGrinderHUD(CGameInstance* pInstance);
	HRESULT Ready_WeaponTeapotHUD(CGameInstance* pInstance);
	HRESULT Ready_WeaponClockBombHUD(CGameInstance* pInstance);
	HRESULT Ready_FightHUD(CGameInstance* pInstance);

	HRESULT Ready_CrossHead(CGameInstance* pInstance);


public:
	static CGamePlayUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free()override;

};

END