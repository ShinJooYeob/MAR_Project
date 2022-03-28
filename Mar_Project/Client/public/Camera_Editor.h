#pragma once

#include "Camera.h"

BEGIN(Client)

class CCamera_Editor :public CCamera
{
private:
	explicit CCamera_Editor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	explicit CCamera_Editor(const CCamera_Editor&  rhs);
	virtual ~CCamera_Editor() = default;
public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;


private:
	_bool			m_bIsCamMoving = false;

private:
	HRESULT			SetUp_Components();



public:
	static CCamera_Editor* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free()override;
};

END