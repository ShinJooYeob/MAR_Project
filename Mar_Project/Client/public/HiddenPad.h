#pragma once
#include "MapObject.h"

BEGIN(Client)

class CHiddenPad final :public CMapObject
{
private:
	CHiddenPad(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CHiddenPad(const CHiddenPad& rhs);
	virtual ~CHiddenPad() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;

public:
	HRESULT Set_TerrainTileKinds();
	HRESULT Reset_TerrainTileKindsMovableNHeightZero();

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;


	
private:
	class CPlayer*		m_pPlayer= nullptr;
	CTransform*			m_pPlayerTransform = nullptr;
	_bool				m_bIsPlayerCloser = false;

	_uint				m_iKindsOfHiddenPad = 0;

	_float4				m_NevRectPoint[4];
private:
	HRESULT SetUp_Components();


public:
	static CHiddenPad* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END