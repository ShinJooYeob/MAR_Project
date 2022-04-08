#pragma once
#include "MapObject.h"

BEGIN(Client)

class CSteamPad final :public CMapObject
{
private:
	CSteamPad(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CSteamPad(const CSteamPad& rhs);
	virtual ~CSteamPad() = default;

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
	_float				m_fRangeRadius = 1.414f;
	_float				m_fMaxHight = 20;

private:
	HRESULT SetUp_Components();


public:
	static CSteamPad* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END