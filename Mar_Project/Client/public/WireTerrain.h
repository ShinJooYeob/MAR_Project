#pragma once

#include "GameObject.h"

BEGIN(Client)

class CWireTerrain final : public CGameObject
{
private:
	CWireTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CWireTerrain(const CWireTerrain& rhs);
	virtual ~CWireTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;

public:
	_float3 Pick_OnTerrain(_bool* pbIsObTerrain, _fVector ObjectWorldPos, _fVector ObjectOldWorldPos,_float3* vOutPlaneNormalVec = nullptr);

	void Set_ShaderPass(_uint iPassIndex) { m_iPassIndex = iPassIndex; };
	void Set_PickedPos(_float3* vPickPos) { memcpy(&m_vPickedPos, vPickPos, sizeof(_float3)); };
private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CVIBuffer_DynamicTerrain*		m_pVIBufferCom = nullptr;
	CTransform*						m_pTransformCom = nullptr;
	CTexture*						m_pTextureCom = nullptr;

private:
	_uint				m_iPassIndex = 2;
	_float3				m_vPickedPos = NOT_EXIST_VECTOR;
private:
	HRESULT SetUp_Components();
	

private:
	_float4x4			m_InverseWorldMat;

public:
	static CWireTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END