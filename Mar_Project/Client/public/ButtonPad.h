#pragma once
#include "MapObject.h"

BEGIN(Client)

class CButtonPad final :public CMapObject
{
public:
	typedef struct tagButtonDesc
	{
		_float3 vPosition;
		_float3 vAngle;

		CGameObject* pTargetObject = nullptr;
		_uint	eKindsOfObject = INT_MAX;
	}BUTTONDESC;


private:
	CButtonPad(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CButtonPad(const CButtonPad& rhs);
	virtual ~CButtonPad() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

	void Set_Visuable() { m_bVisuable = true; };

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
	_bool				m_bIsPlayerCloser = false;
	BUTTONDESC			m_tDesc;

	_bool				m_bIsUp = true;
	_float				m_ButtonHight = 0;
	_double				m_PassedTime = 0;

	_bool				m_bChecker = false;

	_bool				m_bVisuable = false;


private:
	HRESULT SetUp_Components();
	HRESULT Update_ButtonAnim(_double fDeltaTime);

public:
	static CButtonPad* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END