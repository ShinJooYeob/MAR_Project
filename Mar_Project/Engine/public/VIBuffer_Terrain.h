#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
public:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMap);
	virtual HRESULT Initialize_Prototype(_uint iNumWidthPixelX, _uint iNumWidthPixelY);
	virtual HRESULT Initialize_Clone(void* pArg)override;

	_float Get_TerrainHeight(_bool* bIsMovable ,_float3 PosOnTerrainLocal);
	_uint Get_TerrainKinds(_bool* bIsMovable, _float3 PosOnTerrainLocal);
	_Vector Caculate_TerrainY(_bool* pbIsOnTerrain, _float3 PosOnTerrainLocal, _float3 OldPosOnTerrainLocal, _float3* vLocalPlaneNormVector = nullptr, _uint* eNowTile = nullptr);
	_Vector Caculate_TerrainY_IgnoreTile(_bool* pbIsOnTerrain, _float3 PosOnTerrainLocal, _float3 OldPosOnTerrainLocal, _float3* vLocalPlaneNormVector = nullptr, _uint* eNowTile = nullptr);
	_Vector Caculate_Terrain_Pick_byRay(_bool* pbIsOnTerrain, _float3 PosOnTerrainLocal, _float3 OldPosOnTerrainLocal, _float3* vLocalPlaneNormVector = nullptr);
	_float EquationPlane(_bool* pbIsOnTerrain, _float3 PosOnTerrainLocal, _float* CaculateY = nullptr, _float3* vNormVector = nullptr);
	_float EquationPlane_Kinds_Of_NavigationTile(_uint* pOutKidsOfTile, _bool* pbIsOnTerrain, _float3 PosOnTerrainLocal,
		_float* CaculateY = nullptr, _float3* vNormVector = nullptr);

	HRESULT Chage_TileKindsNHeight(_uint iTileKinds, _float3 PosNHeight);
	HRESULT Chage_SourTile_To_DestTile(_uint iSourTileKinds, _uint iDestTileKinds,  _float3 Position);

	_Vector Calculate_SlidingVector(_float3 OldPosOnTerrainLocal, _float3 PosOnTerrainLocal);
	_Vector Calculate_SlidingVector_ForNoneTile(_float3 OldPosOnTerrainLocal, _float3 PosOnTerrainLocal, _float Rate = 1.001f);
	_bool   OutOfLine(_fVector StartPoint, _fVector EndPoint, _fVector OldPos, _gVector NowPos, _float3* pOutDirVec);
	_bool   OutOfLine_ForNoneTile(_uint iStartIndex, _uint iEndIndex, _fVector OldPos, _gVector NowPos, _float3* pOutDirVec,_float Rate = 1.001f);

	_Vector Pick_ByRay(_fVector vRayPos, _fVector vRayDir, _float2 vIndex, _bool* bIsPieck);
	_float2 Get_NumVerticesXY() { return _float2(_float(m_iNumVerticesX), _float(m_iNumVerticesZ)); };
	_float Get_MinMapSize() { return m_fMinMapSize; };
private:
	_uint			m_iNumVerticesX = 0;
	_uint			m_iNumVerticesZ = 0;

	_float			m_fMinMapSize = 20;

	_float*			m_pNaviTerrain = nullptr;

#ifdef _DEBUG
	ID3D11ShaderResourceView* m_pHeightMapSRV = nullptr;
private:
	HRESULT LoadHeightMap();
public:
	HRESULT Bind_HeightMapOnShader(class CShader* pShader, const char* szValueName);

#endif // _DEBUG


public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pHeightMap);
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumWidthPixelX, _uint iNumWidthPixelY = 0);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END