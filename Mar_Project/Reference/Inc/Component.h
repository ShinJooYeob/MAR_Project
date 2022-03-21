#pragma once

#include "Base.h"


BEGIN(Engine)
class ENGINE_DLL CComponent abstract :public CBase
{
protected:
	explicit CComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype(void * pArg);
	virtual HRESULT Initialize_Clone(void * pArg);


protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	_bool				m_bIsClone = false;


public:
	virtual CComponent* Clone(void* pArg = nullptr)PURE;
	virtual void  Free() override;
};

END	