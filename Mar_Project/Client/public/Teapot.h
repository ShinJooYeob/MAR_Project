#pragma once
#include "Weapon.h"

BEGIN(Client)

class CTeapot final : public CWeapon
{
private:
	CTeapot(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTeapot(const CTeapot& rhs);
	virtual ~CTeapot() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;

private:
	HRESULT SetUp_Components();


public:
	static CTeapot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END	