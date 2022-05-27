#pragma once
#include "MapObject.h"

BEGIN(Client)

class CEscalatorPad final :public CMapObject
{
public:
	typedef struct tagEscalatorDesc
	{
		_uint	KindsOfMesh;
		_float3 vStartPos;
		_float3 vDestPos;
		_float  MoveSpeed;
	}ESCLATORDESC;


private:
	CEscalatorPad(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEscalatorPad(const CEscalatorPad& rhs);
	virtual ~CEscalatorPad() = default;

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

	void	LetEscalatingToDest(_bool bBool);
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

	ESCLATORDESC		m_tDesc;

	_bool				m_bMoveToDest = false;
	_double				m_PassedTime = 0;

	_float4				m_NevRectPoint[4];
private:
	HRESULT SetUp_Components();
	HRESULT Update_Escalating(_double fDeltaTime);

public:
	static CEscalatorPad* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END