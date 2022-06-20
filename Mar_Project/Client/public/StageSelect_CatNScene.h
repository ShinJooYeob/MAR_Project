#pragma once
#include "MapObject.h"

BEGIN(Client)

class CStageSelect_CatNScene final :public CMapObject
{
public:
	typedef struct tagSpwanTriggerDesc
	{
		_float3 vPosition;
	}SPWANTRIGGERDESC;


private:
	explicit CStageSelect_CatNScene(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStageSelect_CatNScene(const CStageSelect_CatNScene& rhs);
	virtual ~CStageSelect_CatNScene() = default;

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
	HRESULT Load_ActionCam3(const _tchar* szPath);

private:
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

#ifdef _DEBUG

	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
#endif // _DEBUG
private:
	class CStageSelectAlice*		m_pPlayer= nullptr;
	CTransform*			m_pPlayerTransform = nullptr;

	SPWANTRIGGERDESC			m_tDesc;


	_uint				m_ActionCount = 0;
	_uint				m_iChecker = 0;
	_double				m_Timer = 0;

	vector<CAMACTDESC>		 m_vecCamPositions;
	vector<CAMACTDESC>		 m_vecLookPostions;


	vector<CAMACTDESC>		 m_vecEndCamPositions;
	vector<CAMACTDESC>		 m_vecEndLookPostions;

private:
	HRESULT SetUp_Components();



public:
	static CStageSelect_CatNScene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END