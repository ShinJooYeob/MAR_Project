#pragma once
#include "MapObject.h"

BEGIN(Client)

class CSlidePad final :public CMapObject
{
public:
	typedef struct tagSlideDesc
	{
		_float3 vPosition;
		_float3 vAngle;

		_bool bStartCollider = false;
		_bool bEndCollider = false;
	}SLIDEDESC;


private:
	CSlidePad(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CSlidePad(const CSlidePad& rhs);
	virtual ~CSlidePad() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

public:
	HRESULT Set_TerrainTileKinds();
	HRESULT Reset_TerrainTileKindsMovableNHeightZero();

	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;
	CCollider*			m_pColliderCom = nullptr;


	
private:
	class CPlayer*		m_pPlayer= nullptr;
	CTransform*			m_pPlayerTransform = nullptr;
	_bool				m_bIsPlayerCloser = false;
	SLIDEDESC			m_tDesc;
	_double				m_PassedTime = 0;
	_float4				m_NevRectPoint[4];


	_int			m_iStartPointColliderIndex = -1;
	_int			m_iEndPointColliderIndex = -1;


private:
	HRESULT SetUp_Components();

public:
	static CSlidePad* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END