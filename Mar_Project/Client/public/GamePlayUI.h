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
	virtual _int LateRender()override;
	HRESULT Add_Dmg_to_Player(_uint iNowHP,_uint iDmg);


protected:
	CRenderer*			m_pRendererCom = nullptr;
	class CPlayer*			m_pPlayer = nullptr;

	vector<CUI*>			m_vecUIContainer;


	_float2				m_vOldMousePos;
	
protected:
	HRESULT Change_WeaponUI();

protected:
	HRESULT SetUp_Components();

	HRESULT Ready_Player(CGameInstance* pInstance);

	HRESULT Ready_HpBarBackGround(CGameInstance* pInstance);
	HRESULT Ready_TeethUI(CGameInstance* pInstance);
	HRESULT Ready_HpFlowerPetal(CGameInstance* pInstance);
	HRESULT Ready_WeaponChageRing(CGameInstance* pInstance);
	HRESULT Ready_WeaponChageCursor(CGameInstance* pInstance);

	HRESULT Ready_CrossHead(CGameInstance* pInstance);


public:
	static CGamePlayUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free()override;

};

END