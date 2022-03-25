#pragma once

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{

protected:
	explicit CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
		explicit CCamera(const CCamera&  rhs);
		virtual ~CCamera() = default;
public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize_Clone(void* pArg);

	virtual _int Update(_double fDeltaTime);
	virtual _int LateUpdate(_double fDeltaTime);
	virtual _int Render();
	virtual _int LateRender();

	//_float3 Get_Camera_Position() { return m_pTransform->Get_MatrixState(CTransform::STATE_POS); };
	CTransform*	 Get_Camera_Transform() { return m_pTransform; };

public:
	
	HRESULT Set_ProjectMatrix(_bool bIsOrtho);


protected:
	_bool						m_bIsOrtho = true;
	CTransform*					m_pTransform = nullptr;
	CAMERADESC					m_CameraDesc;



public:
	virtual CGameObject* Clone(void* pArg)PURE;
	virtual void Free()override;

};

END