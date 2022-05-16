#pragma once
#include "MapObject.h"

BEGIN(Client)

class CMovableColum final :public CMapObject
{
public:
	typedef struct tagClockColumDesc
	{
		_float3 vPosition;
		_float3 vAngle;

	}CLOCKCOLUMDESC;


private:
	CMovableColum(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMovableColum(const CMovableColum& rhs);
	virtual ~CMovableColum() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;


public:
	_bool Start_Turning_Colum(_uint iKinds);

public:
	HRESULT Set_TerrainTileKindsAToB(_uint iSour, _uint iDest);

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;


	
private:
	class CPlayer*		m_pPlayer= nullptr;
	CTransform*			m_pPlayerTransform = nullptr;
	_bool				m_bIsPlayerCloser = false;
	CLOCKCOLUMDESC			m_tDesc;


	_float4				m_NevRectPoint[4];


	_bool				m_bIsTurning = false;
	_double				m_PassedTime = 0;
	_float				m_fStartAngle = 0;
	_float				m_fTargetAngle = 0;

private:
	HRESULT SetUp_Components();
	HRESULT Update_Turning(_double fDeltaTime);

public:
	static CMovableColum* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END