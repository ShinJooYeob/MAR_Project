#pragma once

#include "UI.h"

BEGIN(Client)

class CPauseUI : public CGameObject
{

protected:
	explicit CPauseUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	explicit CPauseUI(const CPauseUI&  rhs);
	virtual ~CPauseUI() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

	void Start_RollUp(_bool bBool);
protected:
	CRenderer*			m_pRendererCom = nullptr;

	class CPlayer*			m_pPlayer = nullptr;

	vector<CUI*>			m_vecUIContainer;
	_float2					m_vOldMousePos;
	
	CUI*					m_pPaperCurlUI = nullptr;
	_bool					m_bUpgrading[Upgrade_End] = { false };
	_float					m_fUpgradingPassedTime[Upgrade_End] = { 0 };


	UPGRADEID TargetUpgradeID;
	_float	StartX;
	_float	StartY;
	_float	DestX;
	_float	DestY;

protected:
	HRESULT Update_PaperCurl(_double fDeltaTime);

	HRESULT Update_Vopal(_double fDeltaTime);
	HRESULT Update_PapperGrinder(_double fDeltaTime);
	HRESULT Update_Horse(_double fDeltaTime);
	HRESULT Update_Teapot(_double fDeltaTime);


protected:
	HRESULT SetUp_Components();
	HRESULT Ready_PaperCurl(CGameInstance* pInstance);

	HRESULT Ready_BarBackGround(CGameInstance* pInstance);

	HRESULT Ready_Vopal(CGameInstance* pInstance);
	HRESULT Ready_PapperGrinder(CGameInstance* pInstance);
	HRESULT Ready_Horse(CGameInstance* pInstance);
	HRESULT Ready_Teapot(CGameInstance* pInstance);



public:
	static CPauseUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free()override;

};

END