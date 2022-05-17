#pragma once
#include "MapObject.h"

BEGIN(Client)

class CJumpPad final :public CMapObject
{
public:
	typedef struct tagJumpPadDesc
	{
		_float3 vPosition;
		_float3 vScale =_float3(1);
		_float fPower = 20;

	}JUMPPADDESC;

private:
	CJumpPad(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CJumpPad(const CJumpPad& rhs);
	virtual ~CJumpPad() = default;

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


private:
	class CPlayer*		m_pPlayer= nullptr;
	CTransform*			m_pPlayerTransform = nullptr;
	_bool				m_bIsPlayerCloser = false;
	_float				m_fRangeRadius = 0.707f;

	JUMPPADDESC			m_tDesc;

private:
	HRESULT SetUp_Components();


public:
	static CJumpPad* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END