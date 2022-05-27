#pragma once

#include "UI.h"

BEGIN(Client)
class CLobyUI : public CGameObject
{
protected:
	explicit CLobyUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	explicit CLobyUI(const CLobyUI&  rhs);
	virtual ~CLobyUI() = default;

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
	vector<CUI*>		m_vecUIContainer;

	_int				m_iIndex = 0;
	_double				m_PassedTime = 0;


protected:
	HRESULT SetUp_Components();

	HRESULT Ready_BackGround(CGameInstance* pInstance);
	HRESULT Ready_LobyFontGuide(CGameInstance* pInstance);
	HRESULT Ready_LobyFont(CGameInstance* pInstance, _uint iLayerIndex);
	HRESULT Ready_BloodBG(CGameInstance* pInstance);

public:
	static CLobyUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free()override;

};


END
