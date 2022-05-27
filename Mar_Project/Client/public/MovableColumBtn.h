#pragma once
#include "MapObject.h"

BEGIN(Client)

class CMovableColumBtn final :public CMapObject
{
public:
	typedef struct tagColumButtonDesc
	{
		_float3 vPosition;

		CGameObject* pTargetObject = nullptr;
		_uint	eKindsOfObject = INT_MAX;
	}COLUMBTNDESC;


private:
	CMovableColumBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMovableColumBtn(const CMovableColumBtn& rhs);
	virtual ~CMovableColumBtn() = default;

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
	void	LetWorkButton(_bool bBool);

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;
	CModel*				m_pSubModel = nullptr;
	CCollider*			m_pColliderCom = nullptr;


	
private:
	class CPlayer*		m_pPlayer= nullptr;
	CTransform*			m_pPlayerTransform = nullptr;

	COLUMBTNDESC		m_tDesc;
	_float4				m_NevRectPoint[4];

	_bool				m_bIsUp = true;
	_float				m_ButtonHight = 0;
	_double				m_PassedTime = 0;
	_bool				m_bChecker = false;


	vector<CAMACTDESC>		 m_vecCamPositions;
	vector<CAMACTDESC>		 m_vecLookPostions;
private:
	HRESULT SetUp_Components();
	HRESULT Update_ButtonAnim(_double fDeltaTime);
	HRESULT Load_ActionCam(const _tchar* szPath);

public:
	static CMovableColumBtn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END