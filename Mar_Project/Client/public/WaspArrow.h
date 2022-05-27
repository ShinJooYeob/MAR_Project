#pragma once

#include "Monster.h"

BEGIN(Client)

class CWaspArrow final : public CMonster
{
private:
	CWaspArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CWaspArrow(const CWaspArrow& rhs);
	virtual ~CWaspArrow() = default;


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
	CModel*				m_pModel = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

	ATTACHBONEMATRIX_PTR m_tCollisionAttachPtr[2];

private:
	HRESULT SetUp_Components();

public:
	static CWaspArrow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};


END