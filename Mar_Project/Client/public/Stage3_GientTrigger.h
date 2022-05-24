#pragma once
#include "MapObject.h"

BEGIN(Client)

class CStage3_GientTrigger final :public CMapObject
{



private:
	explicit CStage3_GientTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStage3_GientTrigger(const CStage3_GientTrigger& rhs);
	virtual ~CStage3_GientTrigger() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;

public:
	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;


private:
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;

	
private:
	class CPlayer*		m_pPlayer= nullptr;
	CTransform*			m_pPlayerTransform = nullptr;


	_double				m_SpwanPassedTime = 0;
	_bool				m_bSpwanStart = false;
	_uint				m_iChecker = 0;

	_float3				m_vTempPlayerPosition;
	_float3				m_vTempTargetPosition;

	_bool				m_bOilDropRender = false;	
	_uint				m_EndCamActionCount = 0;

	_uint				m_iSpwanPoint = 0;

private:
	HRESULT SetUp_Components();



public:
	static CStage3_GientTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END