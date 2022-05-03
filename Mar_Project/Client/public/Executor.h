#pragma once

#include "Monster.h"

BEGIN(Client)

class CExecutor final : public CMonster
{
private:
	CExecutor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CExecutor(const CExecutor& rhs);
	virtual ~CExecutor() = default;


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
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModel = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;


	_bool				m_bIsJumping = false;


	ATTACHBONEMATRIX_PTR m_tCollisionAttachPtr;

	vector<class CMonsterWeapon*>			m_vecWeapon;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_Weapon();

	virtual HRESULT Set_Monster_On_Terrain(CTransform* pTransform, _double fDeltaTime)override;


public:
	static CExecutor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};


END