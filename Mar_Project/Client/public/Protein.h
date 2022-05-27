#pragma once
#include "Weapon.h"

BEGIN(Client)

class CProtein final : public CWeapon
{
private:
	CProtein(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CProtein(const CProtein& rhs);
	virtual ~CProtein() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;


private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;


	_double				m_PassedTime = 0;

private:
	HRESULT SetUp_Components();



public:
	static CProtein* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END	