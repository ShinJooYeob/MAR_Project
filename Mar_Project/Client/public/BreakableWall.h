#pragma once

#include "MapObject.h"

BEGIN(Client)
class CBreakableWall final :public CMapObject
{
private:
	CBreakableWall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBreakableWall(const CBreakableWall& rhs);
	virtual ~CBreakableWall() = default;


public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

	_uint Get_NowPieceNum() { return m_iPieceNum; };
	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType) override;

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;
	CCollider*			m_pColliderCom = nullptr;

	_float				m_fHP = 4;

	_uint				m_iKindsOfMesh = 0;
	_uint				m_iPieceNum = 0;
	_double				m_PassedTime = 0;
private:
	class CPlayer*		m_pPlayer = nullptr;
	CTransform*			m_pPlayerTransform = nullptr;
private:
	HRESULT SetUp_Components();


public:
	static CBreakableWall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

