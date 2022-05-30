#pragma once

#include "UI.h"

BEGIN(Client)

class CRadialBlur : public CUI
{

protected:
	explicit CRadialBlur(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	explicit CRadialBlur(const CRadialBlur&  rhs);
	virtual ~CRadialBlur() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

	void Chage_TextureIndex(_uint iIndex = 0);


	 
protected:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;


	_bool				m_bNeedToDraw = false;
	_double				m_PassedTime = FLT_MAX;
	_double				m_TotalTime = 0;
	_float				m_DashPower = 0;
	_float4				m_vTargetColor = _float4(0,0,0,0);

protected:
	HRESULT SetUp_Components();

public:
	static CRadialBlur* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free()override;

};
END

