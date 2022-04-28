#pragma once
#include "Weapon.h"


BEGIN(Client)

class CBullet abstract : public CWeapon
{
protected:
	CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBullet(const CBullet& rhs);
	virtual ~CBullet() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;

protected:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;

	_float3				m_vTargetDir;
private:
	HRESULT SetUp_Components();

protected:
	_float					m_fLifeTime = 0;
	_float					m_fTotalLifeTime = 0;

public:
	virtual void Free() override;
};


END
