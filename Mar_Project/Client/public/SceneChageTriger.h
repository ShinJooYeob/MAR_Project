#pragma once
#include "MapObject.h"

BEGIN(Client)

class CSceneChageTriger final :public CMapObject
{
public:
	typedef struct tagSceneDesc
	{
		_float3 vPosition;
		_float3 vScale;
		_float3 vTargetPosition;
		_uint	eTargetScene = SCENE_STATIC;
	}SCNCHGTRGDESC;


private:
	explicit CSceneChageTriger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSceneChageTriger(const CSceneChageTriger& rhs);
	virtual ~CSceneChageTriger() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

public:
	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;
	_bool Come_Over_Player_By_Vent();
	HRESULT Load_ActionCam(const _tchar* szPath );

private:
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

#ifdef _DEBUG
	CRenderer*			m_pRendererCom = nullptr;
#endif // _DEBUG

	
private:
	class CPlayer*		m_pPlayer= nullptr;
	CTransform*			m_pPlayerTransform = nullptr;

	SCNCHGTRGDESC			m_tDesc;

	_double				m_VentingPassedTime = 0;
	_bool				m_bVentingStart = false;
	_bool				m_bIsFadeStart = false;
	_bool				m_bSceneChagingStarted = false;

	_float3				m_vTempPlayerPosition;
	_float3				m_vTempTargetPosition;


	vector<CAMACTDESC>		 m_vecCamPositions;
	vector<CAMACTDESC>		 m_vecLookPostions;

private:
	HRESULT SetUp_Components();



public:
	static CSceneChageTriger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END