#pragma once


#include "GameObject.h"


BEGIN(Client)

class CTransparencyBall :public CGameObject
{
public:
	typedef struct tagBallMeshDesc {
		_float3 vPosition;
		_float fSize = 1.f;
		class CHandyGirl*	HandyGirl = nullptr;
	}BALLMESHDESC;

private:
	CTransparencyBall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTransparencyBall(const CTransparencyBall& rhs);
	virtual ~CTransparencyBall() = default;

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
	
	BALLMESHDESC		m_tDesc;
	_uint				m_iRand = 0;
	_uint				m_iKindsOfSwirl = 0;
	_double				m_PassedTime = 0;
	_double				m_TargetTime = 0;
	_double				m_ShaderTime = 0;

	PARTICLEDESC		m_tParticleDesc;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstTable();
	HRESULT Ready_ParticleDesc();

public:
	static CTransparencyBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};


END
