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
	virtual _int LightRender()override;

public:
	_float3 Pick_OnTerrain(_bool* pbIsObTerrain, _fVector ObjectWorldPos, _fVector ObjectOldWorldPos,_float3* vOutPlaneNormalVec = nullptr);

	HRESULT Change_VertexBuffer(_float2 vChangedVertexIndex, _float fValueY);

	void Set_ShaderPass(_uint iPassIndex) { m_iPassIndex = iPassIndex; };
	void Set_Radius(_float fRadius) { m_fRadius = fRadius; };
	void Set_PickedPos(_float3* vPickPos) { memcpy(&m_vPickedPos, vPickPos, sizeof(_float3)); };

	HRESULT Easing_Terrain_Curve(EasingTypeID eEasingType, _float3 vPosition, _float fTargetHeight, _float fRadius);
	HRESULT Chage_Terrain_Tile(_float3 vPosition, _float fTargetKinds, _float fRadius);
	HRESULT Chage_Terrain_Tile_Line(_float fTargetKinds, _float3 vPosition, _bool IsVerticle,  _uint iCount);
	HRESULT Erasing_TerrainBuffer(_float3 vPosition, _float fRadius);
	HRESULT Save_HeightMap(const _tchar* FileFullpath);;


	HRESULT AutoBakeNavigateTerrain(_float vMovableHeight);
	HRESULT Reset_BakeNavigateTerrain(_int Target);


	HRESULT Draw_FilterMap(_uint iFilterMapIndex, _float3 vPosition, _float fTargetHeight, _float fRadius, _bool bEasing = false);
	HRESULT Create_FilterMap();
	HRESULT Create_FilterMap_byLoad(const char* pFileFullPath);
	HRESULT Delete_FilterMap();
	HRESULT Save_FilterMap(const _tchar* FileFullpath);
	_float Get_Kinds(_float2 Pos);
private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CVIBuffer_DynamicTerrain*		m_pVIBufferCom = nullptr;
	CTransform*						m_pTransformCom = nullptr;
	CTexture*						m_pTextureCom = nullptr;

	ID3D11ShaderResourceView*		m_pFilterMap = nullptr;
	_ulong*							m_pFilterPixel = nullptr;
	ID3D11Texture2D*				m_pFilterTexture = nullptr;

	D3D11_TEXTURE2D_DESC		m_tTextureDesc;
private:
	_float				m_fRadius = 3.f;
	_uint				m_iPassIndex = 2;
	_float3				m_vPickedPos = _float3(1,1,1);

	_bool				m_bIsFilterChaged = false;

private:
	HRESULT SetUp_Components();

	HRESULT Renew_FilterMap();

private:
	_float4x4			m_InverseWorldMat;
	

public:
	static CWireTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END