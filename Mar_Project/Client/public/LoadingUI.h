#pragma once

#include "UI.h"

BEGIN(Client)
class CLoadingUI : public CGameObject
{
protected:
	explicit CLoadingUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	explicit CLoadingUI(const CLoadingUI&  rhs);
	virtual ~CLoadingUI() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;


protected:
	CRenderer*			m_pRendererCom = nullptr;
	vector<CUI*>			m_vecUIContainer;

	_double					m_PassedTime = 0;
	_bool					m_bIsRight = false;
	_float					m_fKeyAngle = 0;

protected:
	HRESULT Update_KeyAngle(_double fDeltaTime);

protected:
	HRESULT SetUp_Components();

	HRESULT Ready_BackGround(CGameInstance* pInstance);
	HRESULT Ready_Key(CGameInstance* pInstance);
	HRESULT Ready_Text(CGameInstance* pInstance);

public:
	static CLoadingUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free()override;

};


END
