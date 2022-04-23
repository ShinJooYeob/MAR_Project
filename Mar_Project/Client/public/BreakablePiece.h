#pragma once
#include "MapObject.h"

BEGIN(Client)

class CBreakablePiece final :public CMapObject
{



private:
	CBreakablePiece(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBreakablePiece(const CBreakablePiece& rhs);
	virtual ~CBreakablePiece() = default;

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
	_float				m_fStartTimer = 0;
	_bool				m_bIsSpout = false;
	_float3				m_vSpoutDir;
	_float				m_fRandPower = 0;
	_float				m_fJumpPower = 0;
	_float				m_fMaxJumpPower = 0;

	_float3				m_vRotAxis;
	_double				m_fTurningTime = 0;

	_uint				m_iKindsOfPiece = 0;

private:
	HRESULT SetUp_Components();
	void Add_JumpPower(_float power);


public:
	static CBreakablePiece* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END