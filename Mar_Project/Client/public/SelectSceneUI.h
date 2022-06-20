#pragma once

#include "UI.h"

BEGIN(Client)
class CSelectSceneUI : public CGameObject
{
protected:
	explicit CSelectSceneUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	explicit CSelectSceneUI(const CSelectSceneUI&  rhs);
	virtual ~CSelectSceneUI() = default;

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

	_int				m_iIndex = 0;
	_double				m_PassedTime = 0;




	vector<FONTSDESC>			m_vecFontDesc;
protected:
	HRESULT SetUp_Components();

	HRESULT Ready_Font(CGameInstance* pInstance);
	HRESULT Update_Font(_double fDeltaTime);

public:
	static CSelectSceneUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free()override;

};


END
