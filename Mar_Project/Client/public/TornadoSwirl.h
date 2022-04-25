#pragma once


#include "GameObject.h"


BEGIN(Client)

class CTornadoSwirl :public CGameObject
{
private:
	CTornadoSwirl(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTornadoSwirl(const CTornadoSwirl& rhs);
	virtual ~CTornadoSwirl() = default;

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

	_uint				m_iRand = 0;
	_uint				m_iKindsOfSwirl = 0;
	_double				m_PassedTime = 0;
	_double				m_TargetTime = 0;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstTable();

public:
	static CTornadoSwirl* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};


END
