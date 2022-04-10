#pragma once

#include "GameObject.h"

BEGIN(Client)

class CTerrain final : public CGameObject
{
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;

public:

	_float3 PutOnTerrain(_bool* pbIsObTerrain, _fVector ObjectWorldPos, _fVector ObjectOldWorldPos,_float3* vOutPlaneNormalVec = nullptr);


private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;


	ID3D11ShaderResourceView*		m_pFilterMap = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT Ready_FilterMap();


private:
	_float4x4			m_InverseWorldMat;

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END