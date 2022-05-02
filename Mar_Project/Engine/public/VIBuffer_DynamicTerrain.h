#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_DynamicTerrain final : public CVIBuffer
{
public:
	CVIBuffer_DynamicTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_DynamicTerrain(const CVIBuffer_DynamicTerrain& rhs);
	virtual ~CVIBuffer_DynamicTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMap);
	virtual HRESULT Initialize_Prototype(_uint iNumWidthPixelX, _uint iNumWidthPixelY);
	virtual HRESULT Initialize_Clone(void* pArg)override;


public:
	HRESULT Bind_HeightMapOnShader(class CShader* pShader,const char* szValueName);
	HRESULT Bake_NavigationTerrain(_float MovableHeight);
	HRESULT Reset_NavigationTerrain(_uint ClearTarget);

public:
	HRESULT Save_HeightMap(const _tchar* FileFullPath);

	_Vector Caculate_TerrainY(_bool* pbIsOnTerrain, _float3 PosOnTerrainLocal,_float3 OldPosOnTerrainLocal ,_float3* vLocalPlaneNormVector = nullptr);
	_Vector Caculate_Terrain_Pick_byRay(_bool* pbIsOnTerrain, _float3 PosOnTerrainLocal, _float3 OldPosOnTerrainLocal, _float3* vLocalPlaneNormVector = nullptr);
	_float EquationPlane(_bool* pbIsOnTerrain, _float3 PosOnTerrainLocal, _float* CaculateY = nullptr, _float3* vNormVector = nullptr);
	_float Get_Kinds(_float2 Pos);
	_Vector Pick_ByRay(_fVector vRayPos, _fVector vRayDir, _float2 vIndex, _bool* bIsPieck);
	_float Get_NowValueY(_float2 vIndex);

	_float2 Get_NumVerticesXY() { return _float2(_float(m_iNumVerticesX), _float(m_iNumVerticesZ)); };
	_float	Get_MinMapSize() { return m_fMinMapSize; };
public:
	HRESULT		Chage_VertexBuffer(_float2 vChangeVertexIndex, _float fValueY);
	HRESULT		Renew_VertexBuffer();
	HRESULT		Erase_VertexBuffer(_float2 vChangeVertexIndex);
	HRESULT		Chage_KindsOfTile(_float2 vChangeVertexIndex, _float fValueY);

private:
	_uint			m_iNumVerticesX = 0;
	_uint			m_iNumVerticesZ = 0;

	_float			m_fMinMapSize = 20;

	VTXNORTEX*		m_pKeepVertices = nullptr;
	_float*			m_pNaviTerrain = nullptr;
	_bool			m_bIsVertexChange = false;
	_bool			m_bIsHeightMapChange = false;
	
	ID3D11ShaderResourceView* m_pHeightMapSRV = nullptr;
public:
	static CVIBuffer_DynamicTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pHeightMap);
	static CVIBuffer_DynamicTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumWidthPixelX, _uint iNumWidthPixelY = 0);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END