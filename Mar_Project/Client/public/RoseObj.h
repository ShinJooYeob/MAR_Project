#pragma once
#include "MapObject.h"

BEGIN(Client)

class CRoseObj final :public CMapObject
{
private:
	CRoseObj(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CRoseObj(const CRoseObj& rhs);
	virtual ~CRoseObj() = default;

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
	CVIBuffer_Cube*		m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
private:
	class CPlayer*		m_pPlayer= nullptr;
	CTransform*			m_pPlayerTransform = nullptr;
	_bool				m_bIsPlayerCloser = false;
	_float				m_fRangeRadius = 3.f;



	_float				m_fStartTimer = 0;
	_bool				m_bIsSpout = false;
	_float3				m_vSpoutDir;
	_float				m_fRandPower = 0;
	_float				m_fJumpPower = 0;
	_float				m_fMaxJumpPower = 0;

private:
	HRESULT SetUp_Components();
	void Add_JumpPower(_float power);


public:
	static CRoseObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END