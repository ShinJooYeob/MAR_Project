#pragma once

#include "Monster.h"

BEGIN(Client)

class CWaspInk final : public CMonster
{
private:
	CWaspInk(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CWaspInk(const CWaspInk& rhs);
	virtual ~CWaspInk() = default;


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
	CModel*				m_pModel = nullptr;
	CTransform*			m_pTransformCom = nullptr;


private:
	HRESULT SetUp_Components();

public:
	static CWaspInk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};


END