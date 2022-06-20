#pragma once

#include "GameObject.h"

BEGIN(Client)
class CNpc : public CGameObject
{
public:
	typedef struct tagNpcDesc
	{
		_float3 StartPosition;
		_float3 DestPosition;
		_uint	MeshKinds;
	}NPCDESC;


private:
	CNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNpc(const CNpc& rhs);
	virtual ~CNpc() = default;

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

	NPCDESC				m_tDesc;
	_bool				m_bMoveToDest = false;
	_double				m_WalkingTime = 0;
	_double				m_WalkingTargetTime = 0;

	_uint				m_iOldAnimIndex = INT_MAX;
	_uint				m_iAdjMovedIndex = 0;
	_bool				m_bNeedToRend = true;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstTable();

	HRESULT Move_Update(_double fDeltaTime, CGameInstance* pInstance);


public:
	static CNpc* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};


END
