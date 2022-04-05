#pragma once

#include "Base.h"

BEGIN(Client)

class CMainApp final :public CBase
{
public:
	explicit CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT		Initialize(); 
	_int		Update(_double fDeltaTime);
	_double		Update_SlowMotion(_double fDeltaTime);
	HRESULT		Render();

public:
	void		SlowMotionStart(_float fTargetTime = 0.5f,_float TargetSpeed = 0.3f);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	// ID3D11ShaderResourceView*
	ID3D11RenderTargetView*		m_pBackBufferRTV = nullptr;
	ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;
	IDXGISwapChain*				m_pSwapChain = nullptr;

	CGameInstance*			m_pGameInstance = nullptr;
	CRenderer*				m_pComRenderer = nullptr;
	//CCollision*				m_pCollision = nullptr;


private: //슬로우 모션용
	_bool					m_bIsSlowed = false;
	_float					m_fTargetSpeed = 0.3f;
	_float					m_fPassedTime = 0;
	_float					m_fTargetTime = 1.f;
	_double					m_SlowTimes = 1;

private:
	HRESULT Scene_Change(SCENEID eSceneID);
	HRESULT Default_Setting();
	
	HRESULT Ready_SingletonMgr();
	HRESULT Free_SingletonMgr();

	HRESULT Ready_Static_Component_Prototype();
	HRESULT Ready_Static_GameObject_Prototype();


public:
	static CMainApp* Create();
	virtual void Free() override;
};

END