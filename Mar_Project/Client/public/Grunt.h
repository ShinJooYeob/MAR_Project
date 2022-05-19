#pragma once

#include "Monster.h"

BEGIN(Client)

class CGrunt final : public CMonster
{
private:
	CGrunt(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGrunt(const CGrunt& rhs);
	virtual ~CGrunt() = default;


public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;

	_int		Update_DmgCalculate(_double fDeltaTime);
	virtual _int Update_Pattern(_double fDeltaTime)override;
	virtual void Add_Dmg_to_Monster(_float iDmgAmount)override;

	void Set_SpwanFinished() { m_bSpwanedAnimFinished = true; };
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModel = nullptr;
	CModel*				m_pSubModel = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

	_bool				m_bIsDmgAnimUpdated[3];

	_bool				m_bSpwanedAnimFinished = false;
	_uint				m_iSpwanMeshRend = 0;
	_float				m_SpwanMeshScale = 2;
	_double				m_SpwanedPassedTime = 0;


	_bool				m_bDeathAnimStart = false;
private:
	HRESULT SetUp_Components();



public:
	static CGrunt* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};


END