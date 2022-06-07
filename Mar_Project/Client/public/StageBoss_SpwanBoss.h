#pragma once
#include "MapObject.h"

BEGIN(Client)

class CStageBoss_SpwanBoss final :public CMapObject
{
public:
	typedef struct tagSpwanTriggerDesc
	{
		_float3 vPosition;
	}SPWANTRIGGERDESC;


private:
	explicit CStageBoss_SpwanBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStageBoss_SpwanBoss(const CStageBoss_SpwanBoss& rhs);
	virtual ~CStageBoss_SpwanBoss() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

	void PlayCamAction();

public:
	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;
	HRESULT Load_ActionCam(const _tchar* szPath);
	HRESULT Load_ActionCam2(const _tchar* szPath);

private:
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

#ifdef _DEBUG

	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
#endif // _DEBUG
private:
	class CPlayer*		m_pPlayer= nullptr;
	CTransform*			m_pPlayerTransform = nullptr;

	SPWANTRIGGERDESC			m_tDesc;

	_double				m_SpwanPassedTime = 0;
	_bool				m_bSpwanStart = false;
	_uint				m_iChecker = 0;

	_float3				m_vTempPlayerPosition;
	_float3				m_vTempTargetPosition;

	_uint				m_EndCamActionCount = 0;


	vector<CAMACTDESC>		 m_vecCamPositions;
	vector<CAMACTDESC>		 m_vecLookPostions;


	vector<CAMACTDESC>		 m_vecEndCamPositions;
	vector<CAMACTDESC>		 m_vecEndLookPostions;

private:
	HRESULT SetUp_Components();



public:
	static CStageBoss_SpwanBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END