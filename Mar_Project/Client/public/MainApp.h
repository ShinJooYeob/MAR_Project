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
	void		DistFogRendOn(_bool bBool) { m_pComRenderer->DistFogRendOn(bBool); };
	HRESULT		Start_IntroViedo();
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

	class CUI*				m_pMouseCursor = nullptr;


private: //슬로우 모션용
	_bool					m_bIsSlowed = false;
	_float					m_fTargetSpeed = 0.3f;
	_float					m_fPassedTime = 0;
	_float					m_fTargetTime = 1.f;
	_double					m_SlowTimes = 1;

#ifdef _DEBUG
private:
	_tchar				m_szFPS[MAX_PATH] = TEXT("");
	_ulong				m_dwNumRender = 0;
	_double				m_dTimerAcc = 0.0f;
#endif // _DEBUG

private:
	HRESULT Scene_Change(SCENEID eSceneID);
	HRESULT Default_Setting();
	
	HRESULT Ready_SingletonMgr();
	HRESULT Free_SingletonMgr();

	HRESULT Ready_Static_Component_Prototype();
	HRESULT Ready_Static_GameObject_Prototype();

	HRESULT Ready_MouseCursor();
	HRESULT Update_Mouse();


public:
	static CMainApp* Create();
	virtual void Free() override;
};

END