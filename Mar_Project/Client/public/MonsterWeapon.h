#pragma once

#pragma once

#include "GameObject.h"

BEGIN(Client)

class CMonsterWeapon abstract : public CGameObject
{
public:
	typedef struct tagMonsterWeaponDesc
	{
		CModel*			pModel = nullptr;
		CTransform*		pParantTransform = nullptr;
		const char*		szHirarchyNodeName = nullptr;
	}MONWEAPONDESC;


protected:
	CMonsterWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMonsterWeapon(const CMonsterWeapon& rhs);
	virtual ~CMonsterWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;
	virtual HRESULT Initialize_Clone_Bullet(void* pArg);

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

	virtual CModel*	Get_WeaponModel() { return nullptr; };
	void Set_AttackAble(_bool bBool = true) { m_bIsAttackAble = bBool; };

protected:
	class CPlayer*			m_pPlayer = nullptr;

	MONWEAPONDESC				m_tWeaponDesc;
	ATTACHBONEMATRIX_PTR	m_tATBMat;
	_float4x4				m_BoneMatrix;
	_bool					m_bIsAttackAble = false;

public:
	virtual void Free() override;
};

END


