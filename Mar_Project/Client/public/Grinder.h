#pragma once
#include "Weapon.h"

BEGIN(Client)

class CGrinder final : public CWeapon
{
private:
	CGrinder(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGrinder(const CGrinder& rhs);
	virtual ~CGrinder() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;

	CModel*	Get_WeaponModel() override {return m_pModel;};
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;

private:
	HRESULT SetUp_Components();


public:
	static CGrinder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END	