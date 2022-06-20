#pragma once

#include "GameObject.h"

BEGIN(Client)
class CCatNpc : public CGameObject
{

private:
	CCatNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCatNpc(const CCatNpc& rhs);
	virtual ~CCatNpc() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;


public:
	void MoveToNext();

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModel = nullptr;
	CTransform*			m_pTransformCom = nullptr;

	_bool				m_bMoveToDest = false;
	_uint				m_bMoveIndex = 0;

	_uint				m_iOldAnimIndex = INT_MAX;
	_uint				m_iAdjMovedIndex = 0;

	_bool				m_bNeedToRend = true;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstTable();

	HRESULT Move_Update(_double fDeltaTime, CGameInstance* pInstance);


public:
	static CCatNpc* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};


END
