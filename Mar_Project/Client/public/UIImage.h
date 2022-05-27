#pragma once

#include "UI.h"

BEGIN(Client)

class CUIImage : public CUI
{
protected:
	explicit CUIImage(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	explicit CUIImage(const CUIImage&  rhs);
	virtual ~CUIImage() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;



protected:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;


protected:
	HRESULT SetUp_Components();

public:
	static CUIImage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free()override;

};
END

