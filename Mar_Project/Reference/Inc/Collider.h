#pragma once

#include "Component.h"


BEGIN(Engine)
class CColliderBuffer;

class ENGINE_DLL CCollider final : public CComponent
{
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg)override;

	void Update_Transform(_uint iIndex, _fMatrix Transform);
	_int Add_ColliderBuffer(COLLIDERTYPE eColliderType, COLLIDERDESC* pColliderDesc = nullptr);

#ifdef _DEBUG
public:
	virtual _int Render();
#endif // _DEBUG

private:
	vector<CColliderBuffer*>    m_vecColliderBuffer;
	vector<CColliderBuffer*>	m_vecPrototypeCollideBuffer;


public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END