#pragma once

#include "GameObject.h"

BEGIN(Client)

class CUI abstract : public CGameObject
{
protected:
	explicit CUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	explicit CUI(const CUI&  rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;


	HRESULT Bind_Transform_OnShader(class CShader* pShader, const char* pValueName);

	UIDESC Convert_UI_To_Rect(FLOATRECT tRect);
	FLOATRECT Convert_Rect_To_UI(UIDESC tUIDesc);

	HRESULT Apply_UIDesc_To_Transform();
	HRESULT Apply_Rect_To_Transform();

protected:
	UIDESC			m_UIDesc;
	FLOATRECT		m_UIRect;
	_float4x4			m_ProjMatrix;
	CTransform*		m_pTransformCom = nullptr;

public:
	virtual void Free()override;

};
END

