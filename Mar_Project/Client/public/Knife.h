#pragma once
#include "Weapon.h"

BEGIN(Client)

class CKnife final : public CWeapon
{
private:
	CKnife(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CKnife(const CKnife& rhs);
	virtual ~CKnife() = default;

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
	CCollider*			m_pColliderCom = nullptr;

private:
	HRESULT SetUp_Components();


public:
	static CKnife* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END	