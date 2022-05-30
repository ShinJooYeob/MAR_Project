#pragma once


#include "GameObject.h"


BEGIN(Client)

class CCircleTornado :public CGameObject
{
public:
	typedef struct tagCircleTornado {
		_float3 vPosition;
		_float3 vLook;
		_float fSize = 1.f;
	}CIRCLETORNADODESC;

private:
	CCircleTornado(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCircleTornado(const CCircleTornado& rhs);
	virtual ~CCircleTornado() = default;

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
	CTexture*			m_pTextureCom = nullptr;
	
	_uint				m_iRand = 0;
	_uint				m_iKindsOfSwirl = 0;
	_double				m_PassedTime = 0;
	_double				m_TargetTime = 0;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstTable();

public:
	static CCircleTornado* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};


END
