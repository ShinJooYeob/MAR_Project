#pragma once

#include "MapObject.h"

BEGIN(Client)
class CBreakableObj final :public CMapObject
{
private:
	CBreakableObj(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBreakableObj(const CBreakableObj& rhs);
	virtual ~CBreakableObj() = default;


public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;

public:
	HRESULT Add_Dmg_To_BreakableObj(_uint iDmg);

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;
	CCollider*			m_pColliderCom = nullptr;

	_float				m_fHP = 4;

	_uint				m_iKindsOfMesh = 0;
	_uint				m_iPieceNum = 0;
private:
	HRESULT SetUp_Components();


public:
	static CBreakableObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

