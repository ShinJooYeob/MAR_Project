#pragma once

#include "GameObject.h"

BEGIN(Client)
class CWeapon abstract : public CGameObject
{
public:
	typedef struct tagWeaponDesc
	{
		CModel*			pModel = nullptr;
		CTransform*		pParantTransform = nullptr;
		const char*		szHirarchyNodeName = nullptr;
	}WEAPONDESC;

protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;
	virtual HRESULT Initialize_Clone_Bullet(void* pArg);

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;

	virtual CModel*	Get_WeaponModel() { return nullptr; };
	void Set_AttackAble(_bool bBool = true) { m_bIsAttackAble = bBool; };

protected:
	class CPlayer*			m_pPlayer = nullptr;

	WEAPONDESC				m_tWeaponDesc;
	ATTACHBONEMATRIX_PTR	m_tATBMat;
	_float4x4				m_BoneMatrix;
	_bool					m_bIsAttackAble = false;

public:
	virtual void Free() override;
};


END
