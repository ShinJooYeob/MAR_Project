#pragma once
#include "MapObject.h"

BEGIN(Client)

class CStage1_SpwanGrunt final :public CMapObject
{
public:
	typedef struct tagSpwanTriggerDesc
	{
		_float3 vPosition;
	}SPWANTRIGGERDESC;


private:
	explicit CStage1_SpwanGrunt(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStage1_SpwanGrunt(const CStage1_SpwanGrunt& rhs);
	virtual ~CStage1_SpwanGrunt() = default;

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
	HRESULT Load_ActionCam(const _tchar* szPath);
	HRESULT Load_ActionCam2(const _tchar* szPath);

private:
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModel = nullptr;
	CShader*			m_pShaderCom = nullptr;

	
private:
	class CPlayer*		m_pPlayer= nullptr;
	CTransform*			m_pPlayerTransform = nullptr;

	SPWANTRIGGERDESC			m_tDesc;

	_double				m_SpwanPassedTime = 0;
	_bool				m_bSpwanStart = false;
	_uint				m_iChecker = 0;

	_float3				m_vTempPlayerPosition;
	_float3				m_vTempTargetPosition;

	_bool			m_bOilDropRender = false;	
	_uint			m_JumpPadSpwanCount = 0;


	vector<CAMACTDESC>		 m_vecCamPositions;
	vector<CAMACTDESC>		 m_vecLookPostions;


	vector<CAMACTDESC>		 m_vecCamPositions2;
	vector<CAMACTDESC>		 m_vecLookPostions2;

private:
	HRESULT SetUp_Components();



public:
	static CStage1_SpwanGrunt* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END