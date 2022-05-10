#include "..\Public\VIBuffer_Terrain.h"
#include "Shader.h"

#define MovableHeight 0.5f


CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
#ifdef _DEBUG
	, m_pHeightMapSRV(rhs.m_pHeightMapSRV)
#endif // _DEBUG
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_fMinMapSize(rhs.m_fMinMapSize)
	, m_pNaviTerrain(rhs.m_pNaviTerrain)
{
#ifdef _DEBUG
	Safe_AddRef(m_pHeightMapSRV);
#endif // _DEBUG
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMap)
{
	FAILED_CHECK(__super::Initialize_Prototype(nullptr));

	_tchar szHeightMapPath[MAX_PATH] = L"../Bin/Resources/Textures/HeightMap/";

	lstrcat(szHeightMapPath, pHeightMap);


	HANDLE		hFile = CreateFile(szHeightMapPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong					dwByte = 0;
	BITMAPFILEHEADER		fh;
	BITMAPINFOHEADER		ih;

	ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);
	 
	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	_ulong*		pPixel = new _ulong[m_iNumVerticesX * m_iNumVerticesZ];
	ReadFile(hFile, pPixel, sizeof(_ulong) * m_iNumVerticesX * m_iNumVerticesZ, &dwByte, nullptr);



#pragma  region READY_VERTEX_BUFFER

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iNumVertexBuffers = 1;

	m_VBDesc.ByteWidth = sizeof(VTXNORTEX) * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.StructureByteStride = sizeof(VTXNORTEX);

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);
	m_pVertices = new _float3[m_iNumVertices];
	m_pNaviTerrain = new _float[m_iNumVertices];
	

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;
			//_float ValueY = (pPixel[iIndex] & 0x000000ff) / 10.f;			

			if ((_ulong((pPixel[iIndex] & 0xff000000) >> 24) ) == 0)
				pVertices[iIndex].vPosition = m_pVertices[iIndex] = _float3(_float(j), -FLT_MAX, _float(i));
			else
			{
				_float fInt = _float(_ulong((pPixel[iIndex] & 0x0000ff00) >> 8));
				if (fInt > 255)fInt = 255;
				_float fFlt = _float(_ulong(pPixel[iIndex] & 0x000000ff))*0.01f;

				pVertices[iIndex].vPosition = m_pVertices[iIndex] = _float3(_float(j), fInt + fFlt, _float(i));
				//pVertices[iIndex].vPosition = m_pVertices[iIndex] = _float3(_float(j), _float((pPixel[iIndex] & 0x000000ff))*0.1f, _float(i));
			}

			//m_pKeepVertices[iIndex].vPosition = m_pVertices[iIndex] = _float3(_float(j), (pPixel[iIndex] & 0x000000ff) / 1.f, _float(i));


			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
			m_pNaviTerrain[iIndex] = _float(_ulong((pPixel[iIndex] & 0x00ff0000) >> 16));
		}
	}



	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	//락언락으로 값을 채워주는 것이 이제는 불가능하기 떄문에
	//D3D11_SUBRESOURCE_DATA 구조체에 값을 던져서 해당 값을 복사해서 생성하도록 한다.
	//이렇게 되면 우리는 해당 정점에 접근하고 싶다면?
	m_VBSubResourceData.pSysMem = pVertices;

	Safe_Delete_Array(pPixel);
	CloseHandle(hFile);

#pragma  endregion 

#pragma  region READY_INDEX_BUFFER
	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iNumIndicesPerPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	// m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(FACEINDICES32) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	FACEINDICES32*	pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);
	_uint		iNumFace = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_Vector vSrc,vDest,vNorm;

			pIndices[iNumFace]._0 = iIndices[0];
			pIndices[iNumFace]._1 = iIndices[1];
			pIndices[iNumFace]._2 = iIndices[2];

			if (m_pVertices[pIndices[iNumFace]._0].y > -99999.f && m_pVertices[pIndices[iNumFace]._1].y > -99999.f && m_pVertices[pIndices[iNumFace]._2].y > -99999.f)
			{

				vSrc = (m_pVertices[pIndices[iNumFace]._1].XMVector()) - (m_pVertices[pIndices[iNumFace]._0].XMVector());
				vDest = (m_pVertices[pIndices[iNumFace]._2].XMVector()) - (m_pVertices[pIndices[iNumFace]._0].XMVector());
				vNorm = XMVector3Normalize(XMVector3Cross(vSrc, vDest));

				XMStoreFloat3(&pVertices[pIndices[iNumFace]._0].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._0].vNormal) + vNorm)));

				XMStoreFloat3(&pVertices[pIndices[iNumFace]._1].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._1].vNormal) + vNorm)));

				XMStoreFloat3(&pVertices[pIndices[iNumFace]._2].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._2].vNormal) + vNorm)));

			}

			++iNumFace;

			pIndices[iNumFace]._0 = iIndices[0];
			pIndices[iNumFace]._1 = iIndices[2];
			pIndices[iNumFace]._2 = iIndices[3];

			if (m_pVertices[pIndices[iNumFace]._0].y > -99999.f && m_pVertices[pIndices[iNumFace]._1].y > -99999.f && m_pVertices[pIndices[iNumFace]._2].y > -99999.f)
			{
				vSrc = (m_pVertices[pIndices[iNumFace]._1].XMVector()) - (m_pVertices[pIndices[iNumFace]._0].XMVector());
				vDest = (m_pVertices[pIndices[iNumFace]._2].XMVector()) - (m_pVertices[pIndices[iNumFace]._0].XMVector());
				vNorm = XMVector3Normalize(XMVector3Cross(vSrc, vDest));

				XMStoreFloat3(&pVertices[pIndices[iNumFace]._0].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._0].vNormal) + vNorm)));

				XMStoreFloat3(&pVertices[pIndices[iNumFace]._1].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._1].vNormal) + vNorm)));

				XMStoreFloat3(&pVertices[pIndices[iNumFace]._2].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._2].vNormal) + vNorm)));
			}

			++iNumFace;
		}
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

#pragma  endregion 

#pragma region VERTEX_BUFFER

	FAILED_CHECK(Create_VertexBuffer());
	Safe_Delete_Array(pVertices);


	FAILED_CHECK(Create_IndexBuffer());
	Safe_Delete_Array(pIndices);


	_float2 vTerrainSize = Get_NumVerticesXY();
	_float fCrossSize = vTerrainSize.Get_Lenth();
	m_fMinMapSize = fCrossSize / 5;

#pragma endregion

#ifdef _DEBUG
	FAILED_CHECK(LoadHeightMap());
#endif // _DEBUG


	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(_uint iNumWidthPixelX, _uint iNumWidthPixelY)
{
	FAILED_CHECK(__super::Initialize_Prototype(nullptr));



#pragma  region READY_VERTEX_BUFFER

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	if (iNumWidthPixelY == 0)
	{
		m_iNumVerticesX = m_iNumVerticesZ = iNumWidthPixelX;
	}
	else {
		m_iNumVerticesX = iNumWidthPixelX;
		m_iNumVerticesZ = iNumWidthPixelY;
	}

	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iNumVertexBuffers = 1;

	m_VBDesc.ByteWidth = sizeof(VTXNORTEX) * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.StructureByteStride = sizeof(VTXNORTEX);

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);
	m_pVertices = new _float3[m_iNumVertices];


	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = m_pVertices[iIndex] = _float3(_float(j), 0, _float(i));
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}


	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	//락언락으로 값을 채워주는 것이 이제는 불가능하기 떄문에
	//D3D11_SUBRESOURCE_DATA 구조체에 값을 던져서 해당 값을 복사해서 생성하도록 한다.
	//이렇게 되면 우리는 해당 정점에 접근하고 싶다면?
	m_VBSubResourceData.pSysMem = pVertices;

#pragma  endregion 

#pragma  region READY_INDEX_BUFFER
	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iNumIndicesPerPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	// m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(FACEINDICES32) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	FACEINDICES32*	pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);
	_uint		iNumFace = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_Vector vSrc, vDest, vNorm;

			pIndices[iNumFace]._0 = iIndices[0];
			pIndices[iNumFace]._1 = iIndices[1];
			pIndices[iNumFace]._2 = iIndices[2];

			if (m_pVertices[pIndices[iNumFace]._0].y > -99999.f && m_pVertices[pIndices[iNumFace]._1].y > -99999.f && m_pVertices[pIndices[iNumFace]._2].y > -99999.f)
			{

				vSrc = (m_pVertices[pIndices[iNumFace]._1].XMVector()) - (m_pVertices[pIndices[iNumFace]._0].XMVector());
				vDest = (m_pVertices[pIndices[iNumFace]._2].XMVector()) - (m_pVertices[pIndices[iNumFace]._0].XMVector());
				vNorm = XMVector3Normalize(XMVector3Cross(vSrc, vDest));

				XMStoreFloat3(&pVertices[pIndices[iNumFace]._0].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._0].vNormal) + vNorm)));

				XMStoreFloat3(&pVertices[pIndices[iNumFace]._1].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._1].vNormal) + vNorm)));

				XMStoreFloat3(&pVertices[pIndices[iNumFace]._2].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._2].vNormal) + vNorm)));

			}

			++iNumFace;

			pIndices[iNumFace]._0 = iIndices[0];
			pIndices[iNumFace]._1 = iIndices[2];
			pIndices[iNumFace]._2 = iIndices[3];

			if (m_pVertices[pIndices[iNumFace]._0].y > -99999.f && m_pVertices[pIndices[iNumFace]._1].y > -99999.f && m_pVertices[pIndices[iNumFace]._2].y > -99999.f)
			{
				vSrc = (m_pVertices[pIndices[iNumFace]._1].XMVector()) - (m_pVertices[pIndices[iNumFace]._0].XMVector());
				vDest = (m_pVertices[pIndices[iNumFace]._2].XMVector()) - (m_pVertices[pIndices[iNumFace]._0].XMVector());
				vNorm = XMVector3Normalize(XMVector3Cross(vSrc, vDest));

				XMStoreFloat3(&pVertices[pIndices[iNumFace]._0].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._0].vNormal) + vNorm)));

				XMStoreFloat3(&pVertices[pIndices[iNumFace]._1].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._1].vNormal) + vNorm)));

				XMStoreFloat3(&pVertices[pIndices[iNumFace]._2].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._2].vNormal) + vNorm)));
			}

			++iNumFace;
		}
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

#pragma  endregion 

#pragma region CREATE_BUFFER

	FAILED_CHECK(Create_VertexBuffer());
	Safe_Delete_Array(pVertices);


	FAILED_CHECK(Create_IndexBuffer());
	Safe_Delete_Array(pIndices);
#pragma endregion


	_float2 vTerrainSize = Get_NumVerticesXY();
	_float fCrossSize = vTerrainSize.Get_Lenth();
	m_fMinMapSize = fCrossSize / 5;


#ifdef _DEBUG
	FAILED_CHECK(LoadHeightMap());
#endif // _DEBUG
	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	return S_OK;
}

_float CVIBuffer_Terrain::Get_TerrainHeight(_bool* bIsMovable , _float3 PosOnTerrainLocal)
{
	_float fHeight = -FLT_MAX;
	EquationPlane(bIsMovable, PosOnTerrainLocal, &fHeight);
	return fHeight;
}

_uint CVIBuffer_Terrain::Get_TerrainKinds(_bool * bIsMovable, _float3 PosOnTerrainLocal)
{
	_uint eTileKinds = Tile_None;


	if (PosOnTerrainLocal.x < 0 || PosOnTerrainLocal.x >= m_iNumVerticesX ||
		PosOnTerrainLocal.z < 0 || PosOnTerrainLocal.z >= m_iNumVerticesZ)
	{
		return Tile_None;
	}

	_uint iIndex = _uint(_uint(PosOnTerrainLocal.z) * m_iNumVerticesX + PosOnTerrainLocal.x);

	_uint		iIndices[4] = {
		iIndex + m_iNumVerticesX,
		iIndex + m_iNumVerticesX + 1,
		iIndex + 1,
		iIndex };

	if (m_pVertices[iIndices[0]].y < -9999.f || m_pVertices[iIndices[2]].y < -9999.f)
	{
		return Tile_None;

	}

	_float4 Plane;

	if (PosOnTerrainLocal.x - m_pVertices[iIndices[0]].x < m_pVertices[iIndices[0]].z - PosOnTerrainLocal.z)
	{//아래 023
		if (m_pVertices[iIndices[3]].y < -9999.f)
		{
			return Tile_None;
		}


		if (m_pNaviTerrain[iIndices[0]] == Tile_None || m_pNaviTerrain[iIndices[2]] == Tile_None || m_pNaviTerrain[iIndices[3]] == Tile_None)
		{
			return Tile_None;

		}

		eTileKinds = Tile_Movable;

		if (m_pNaviTerrain[iIndices[0]] != Tile_Movable)
		{
			switch (_uint(m_pNaviTerrain[iIndices[0]]))
			{
			case Tile_JumpMovable:

				if (MovableHeight < m_pVertices[iIndices[2]].y - m_pVertices[iIndices[0]].y || MovableHeight < m_pVertices[iIndices[3]].y - m_pVertices[iIndices[0]].y)
				{
					eTileKinds = Tile_JumpMovable;
				}

				break;
			case Tile_ShrinkMovable:
				eTileKinds = Tile_ShrinkMovable;
				break;
			}

		}
		else if (m_pNaviTerrain[iIndices[2]] != Tile_Movable)
		{
			switch (_uint(m_pNaviTerrain[iIndices[2]]))
			{
			case Tile_JumpMovable:
				if (MovableHeight < m_pVertices[iIndices[0]].y - m_pVertices[iIndices[2]].y || MovableHeight < m_pVertices[iIndices[3]].y - m_pVertices[iIndices[2]].y)
				{
					eTileKinds = Tile_JumpMovable;
				}

				break;
			case Tile_ShrinkMovable:
				eTileKinds = Tile_ShrinkMovable;
				break;
			}

		}
		else if (m_pNaviTerrain[iIndices[3]] != Tile_Movable)
		{
			switch (_uint(m_pNaviTerrain[iIndices[3]]))
			{
			case Tile_JumpMovable:
				if (MovableHeight < m_pVertices[iIndices[0]].y - m_pVertices[iIndices[3]].y || MovableHeight < m_pVertices[iIndices[2]].y - m_pVertices[iIndices[3]].y)
				{
					eTileKinds = Tile_JumpMovable;
				}

				break;
			case Tile_ShrinkMovable:
				eTileKinds = Tile_ShrinkMovable;
				break;
			}

		}

	}
	else
	{//위 012
		if (m_pVertices[iIndices[1]].y < -9999.f)
		{
			return Tile_None;
		}

		if (m_pNaviTerrain[iIndices[0]] == Tile_None || m_pNaviTerrain[iIndices[2]] == Tile_None || m_pNaviTerrain[iIndices[1]] == Tile_None)
		{
			return Tile_None;
		}

		eTileKinds = Tile_Movable;


		if (m_pNaviTerrain[iIndices[0]] != Tile_Movable)
		{
			switch (_uint(m_pNaviTerrain[iIndices[0]]))
			{
			case Tile_JumpMovable:
				if (MovableHeight < m_pVertices[iIndices[2]].y - m_pVertices[iIndices[0]].y || MovableHeight < m_pVertices[iIndices[1]].y - m_pVertices[iIndices[0]].y)
				{
					eTileKinds = Tile_JumpMovable;
				}

				break;
			case Tile_ShrinkMovable:
				eTileKinds = Tile_ShrinkMovable;
				break;
			}

		}
		else if (m_pNaviTerrain[iIndices[2]] != Tile_Movable)
		{
			switch (_uint(m_pNaviTerrain[iIndices[2]]))
			{
			case Tile_JumpMovable:
				if (MovableHeight < m_pVertices[iIndices[0]].y - m_pVertices[iIndices[2]].y || MovableHeight < m_pVertices[iIndices[1]].y - m_pVertices[iIndices[2]].y)
				{
					eTileKinds = Tile_JumpMovable;
				}

				break;
			case Tile_ShrinkMovable:
				eTileKinds = Tile_ShrinkMovable;
				break;
			}

		}
		else if (m_pNaviTerrain[iIndices[1]] != Tile_Movable)
		{
			switch (_uint(m_pNaviTerrain[iIndices[1]]))
			{
			case Tile_JumpMovable:
				if (MovableHeight < m_pVertices[iIndices[0]].y - m_pVertices[iIndices[1]].y || MovableHeight < m_pVertices[iIndices[2]].y - m_pVertices[iIndices[1]].y)
				{
					eTileKinds = Tile_JumpMovable;
				}

				break;
			case Tile_ShrinkMovable:
				eTileKinds = Tile_ShrinkMovable;
				break;
			}

		}

	}


	return eTileKinds;
}

_Vector CVIBuffer_Terrain::Caculate_TerrainY(_bool* pbIsOnTerrain ,_float3 PosOnTerrainLocal, _float3 OldPosOnTerrainLocal, _float3* vLocalPlaneNormVector, _uint* eNowTile )
{
	_uint NowNaviTile = Tile_None;
	_float CaculatedY = 0;
	_float CacluatedNow = EquationPlane_Kinds_Of_NavigationTile(&NowNaviTile, pbIsOnTerrain, PosOnTerrainLocal, &CaculatedY, vLocalPlaneNormVector);


	_uint OldNaviTile = Tile_None;
	_float CaculatedOldY = 0; 
	_bool IsOldOnTerrain = false;
	_float3 vSlidingVec = XMVector3Normalize(PosOnTerrainLocal.XMVector() - OldPosOnTerrainLocal.XMVector());
	_float CacluatedOld = EquationPlane_Kinds_Of_NavigationTile(&OldNaviTile, &IsOldOnTerrain, OldPosOnTerrainLocal,&CaculatedOldY,&vSlidingVec);
	
	if (NowNaviTile == Tile_None)
	{
		if (eNowTile)*eNowTile = Tile_None;
		*pbIsOnTerrain = false;

		
		OldNaviTile = Tile_None;

		_float3 Temp = OldPosOnTerrainLocal.XMVector() + Calculate_SlidingVector_ForNoneTile(OldPosOnTerrainLocal, PosOnTerrainLocal);
		CacluatedNow = EquationPlane_Kinds_Of_NavigationTile(&OldNaviTile, &IsOldOnTerrain, Temp, &CaculatedOldY, &vSlidingVec);

		for (_uint i = 1; i < 11; i++)
		{
			if (OldNaviTile != Tile_None) break;

			//Temp = OldPosOnTerrainLocal.XMVector() + Calculate_SlidingVector_ForNoneTile(OldPosOnTerrainLocal, PosOnTerrainLocal, 1.001f + (0.1f + (_float)i));
			Temp = OldPosOnTerrainLocal.XMVector() + Calculate_SlidingVector_ForNoneTile(OldPosOnTerrainLocal, Temp);
			CacluatedNow = EquationPlane_Kinds_Of_NavigationTile(&OldNaviTile, &IsOldOnTerrain, Temp, &CaculatedOldY, &vSlidingVec);
		}


		if (PosOnTerrainLocal.y <= CaculatedOldY)
			*pbIsOnTerrain = true;


			if (CaculatedOldY > OldPosOnTerrainLocal.y)
				return XMVectorSet(Temp.x, CaculatedOldY, Temp.z, 0);
			else
				return XMVectorSet(Temp.x, OldPosOnTerrainLocal.y, Temp.z, 0);

		

		//return OldPosOnTerrainLocal.XMVector();
	}




	if (*pbIsOnTerrain == false) {
		if (eNowTile)*eNowTile = NowNaviTile;
		*pbIsOnTerrain = false;
		return PosOnTerrainLocal.XMVector();
	}

	if (PosOnTerrainLocal.y <= CaculatedY && CacluatedOld >= -0.2f)
	{
		if (NowNaviTile == Tile_JumpMovable || NowNaviTile == Tile_ShrinkMovable)
		{
			*pbIsOnTerrain = IsOldOnTerrain;
			if (eNowTile)*eNowTile = NowNaviTile;

			_float3 Temp = OldPosOnTerrainLocal.XMVector() + Calculate_SlidingVector(OldPosOnTerrainLocal, PosOnTerrainLocal);

			EquationPlane(&IsOldOnTerrain, Temp, &CaculatedOldY, nullptr);

			return XMVectorSet(Temp.x, CaculatedOldY, Temp.z,0);
			//return OldPosOnTerrainLocal.XMVector();
		}
		else
		{
			if (eNowTile)*eNowTile = NowNaviTile;
			*pbIsOnTerrain = true;
			return _float3(PosOnTerrainLocal.x, CaculatedY, PosOnTerrainLocal.z).XMVector();
		}
	}
	else 
	{
		if (eNowTile)*eNowTile = NowNaviTile;
		*pbIsOnTerrain = false;
		return PosOnTerrainLocal.XMVector();
	}

}


_Vector CVIBuffer_Terrain::Caculate_Terrain_Pick_byRay(_bool* pbIsOnTerrain, _float3 PosOnTerrainLocal, _float3 OldPosOnTerrainLocal, _float3* vLocalPlaneNormVector)
{


	_float CacluatedOld = EquationPlane(pbIsOnTerrain, OldPosOnTerrainLocal);
	//if (*pbIsOnTerrain == false || CacluatedOld < 0)
	//{
	//	*pbIsOnTerrain = false;
	//	return _float3(PosOnTerrainLocal.x, -FLT_MAX, PosOnTerrainLocal.z);
	//}

	_float CaculatedY = 0;
	_float CacluatedNow = EquationPlane(pbIsOnTerrain, PosOnTerrainLocal, &CaculatedY, vLocalPlaneNormVector);

	if (*pbIsOnTerrain == false) {
		*pbIsOnTerrain = false;
		return PosOnTerrainLocal.XMVector();
	}

	if (PosOnTerrainLocal.y <= CaculatedY && CacluatedOld >= -0.2f)
	{

		RECT IndexRange = { 0 };

		if (OldPosOnTerrainLocal.x < PosOnTerrainLocal.x)
		{
			IndexRange.left = LONG(OldPosOnTerrainLocal.x);
			IndexRange.right = LONG(PosOnTerrainLocal.x);
		}
		else {
			IndexRange.left = LONG(PosOnTerrainLocal.x);
			IndexRange.right = LONG(OldPosOnTerrainLocal.x);
		}

		if (OldPosOnTerrainLocal.z < PosOnTerrainLocal.z)
		{
			IndexRange.bottom = LONG(OldPosOnTerrainLocal.z);
			IndexRange.top = LONG(PosOnTerrainLocal.z);
		}
		else
		{
			IndexRange.bottom = LONG(PosOnTerrainLocal.z);
			IndexRange.top = LONG(OldPosOnTerrainLocal.z);
		}

		IndexRange.left -= 1;
		IndexRange.bottom -= 1;
		IndexRange.right += 1;
		IndexRange.top += 1;

		if (IndexRange.left < 0)IndexRange.left = 0;
		if (IndexRange.bottom < 0)IndexRange.bottom = 0;
		if (IndexRange.right > _int(m_iNumVerticesX))IndexRange.right = m_iNumVerticesX;
		if (IndexRange.top > _int(m_iNumVerticesZ))IndexRange.top = m_iNumVerticesZ;

		vector<_float2> vecAvailIndex;
		vecAvailIndex.reserve(20);

		for (_uint i = IndexRange.left; _int(i) <= IndexRange.right; i++)
		{
			for (_uint j = IndexRange.bottom; _int(j) <= IndexRange.top; j++)
			{
				vecAvailIndex.push_back(_float2(_float(i), _float(j)));
			}
		}

		_bool IsPiecked = false;
		_Vector CalculatedLocalPos;


		_Vector vRayPos = OldPosOnTerrainLocal.XMVector();
		_Vector vRayDir = XMVector3Normalize(PosOnTerrainLocal.XMVector() - vRayPos);


		for (auto& AvailIndex : vecAvailIndex)
		{
			CalculatedLocalPos = Pick_ByRay(vRayPos, vRayDir, AvailIndex,&IsPiecked);

			if (IsPiecked)
			{
				*pbIsOnTerrain = true;
				return CalculatedLocalPos;
			}
		}


		*pbIsOnTerrain = false;
		return PosOnTerrainLocal.XMVector();

	}
	else {
		*pbIsOnTerrain = false;
		return PosOnTerrainLocal.XMVector();
	}
}


_float CVIBuffer_Terrain::EquationPlane(_bool * pbIsOnTerrain, _float3 PosOnTerrainLocal, _float* pCaculateY, _float3* vNormVector)
{
	if (PosOnTerrainLocal.x < 0 || PosOnTerrainLocal.x >= m_iNumVerticesX ||
		PosOnTerrainLocal.z < 0 || PosOnTerrainLocal.z >= m_iNumVerticesZ)
	{
		*pbIsOnTerrain = false;
		return -FLT_MAX;
	}

	_uint iIndex = _uint(_uint(PosOnTerrainLocal.z) * m_iNumVerticesX + PosOnTerrainLocal.x);

	_uint		iIndices[4] = {
		iIndex + m_iNumVerticesX,
		iIndex + m_iNumVerticesX + 1,
		iIndex + 1,
		iIndex };

	if (m_pVertices[iIndices[0]].y < -9999.f || m_pVertices[iIndices[2]].y < -9999.f)
	{
		*pbIsOnTerrain = false;
		return -FLT_MAX;
	}

	_float4 Plane;

	if (PosOnTerrainLocal.x - m_pVertices[iIndices[0]].x < m_pVertices[iIndices[0]].z - PosOnTerrainLocal.z)
	{//아래 023
		if (m_pVertices[iIndices[3]].y < -9999.f)
		{
			*pbIsOnTerrain = false;
			return -FLT_MAX;
		}

		Plane = XMPlaneFromPoints(XMLoadFloat3(&m_pVertices[iIndices[0]]),
			XMLoadFloat3(&m_pVertices[iIndices[2]]), XMLoadFloat3(&m_pVertices[iIndices[3]]));


		if (vNormVector != nullptr)
		{
			*vNormVector = XMVector3Normalize(XMVector3Cross(
				(m_pVertices[iIndices[2]].XMVector()) - (m_pVertices[iIndices[0]].XMVector()),
				(m_pVertices[iIndices[3]].XMVector()) - (m_pVertices[iIndices[0]].XMVector())));
		}
	}
	else
	{//위 012
		if (m_pVertices[iIndices[1]].y < -9999.f)
		{
			*pbIsOnTerrain = false;
			return -FLT_MAX;
		}

		Plane = XMPlaneFromPoints(XMLoadFloat3(&m_pVertices[iIndices[0]]),
			XMLoadFloat3(&m_pVertices[iIndices[1]]), XMLoadFloat3(&m_pVertices[iIndices[2]]));

		if (vNormVector != nullptr)
		{
			*vNormVector = XMVector3Normalize(XMVector3Cross(
				(m_pVertices[iIndices[1]].XMVector()) - (m_pVertices[iIndices[0]].XMVector()),
				(m_pVertices[iIndices[2]].XMVector()) - (m_pVertices[iIndices[0]].XMVector())));
		}

	}

	if (pCaculateY != nullptr)
		*pCaculateY = ((Plane.x * PosOnTerrainLocal.x + Plane.z * PosOnTerrainLocal.z + Plane.w) / -Plane.y);


	*pbIsOnTerrain = true;
	return (Plane.x * PosOnTerrainLocal.x + Plane.y * PosOnTerrainLocal.y + Plane.z * PosOnTerrainLocal.z + Plane.w);
}


_float CVIBuffer_Terrain::EquationPlane_Kinds_Of_NavigationTile(_uint * pOutKidsOfTile, _bool * pbIsOnTerrain, _float3 PosOnTerrainLocal,
	_float * pCaculateY, _float3 * vNormVector)
{
	if (PosOnTerrainLocal.x < 0 || PosOnTerrainLocal.x >= m_iNumVerticesX ||
		PosOnTerrainLocal.z < 0 || PosOnTerrainLocal.z >= m_iNumVerticesZ)
	{
		*pOutKidsOfTile = Tile_None;
		*pbIsOnTerrain = false;
		return -FLT_MAX;
	}

	_uint iIndex = _uint(_uint(PosOnTerrainLocal.z) * m_iNumVerticesX + PosOnTerrainLocal.x);

	_uint		iIndices[4] = {
		iIndex + m_iNumVerticesX,
		iIndex + m_iNumVerticesX + 1,
		iIndex + 1,
		iIndex };

	if (m_pVertices[iIndices[0]].y < -9999.f || m_pVertices[iIndices[2]].y < -9999.f)
	{
		*pOutKidsOfTile = Tile_None;
		*pbIsOnTerrain = false;
		return -FLT_MAX;
	}

	_float4 Plane;

	if (PosOnTerrainLocal.x - m_pVertices[iIndices[0]].x < m_pVertices[iIndices[0]].z - PosOnTerrainLocal.z)
	{//아래 023
		if (m_pVertices[iIndices[3]].y < -9999.f)
		{
			*pOutKidsOfTile = Tile_None;
			*pbIsOnTerrain = false;
			return -FLT_MAX;
		}


		if (m_pNaviTerrain[iIndices[0]] == Tile_None || m_pNaviTerrain[iIndices[2]] == Tile_None || m_pNaviTerrain[iIndices[3]] == Tile_None)
		{
			*pOutKidsOfTile = Tile_None;
			*pbIsOnTerrain = false;
			return -FLT_MAX;
		}


		_uint TargetTile = Tile_Movable;
		if (m_pNaviTerrain[iIndices[0]] != Tile_Movable)
		{
			switch (_uint(m_pNaviTerrain[iIndices[0]]))
			{
			case Tile_JumpMovable:
				
				if (MovableHeight < m_pVertices[iIndices[2]].y - m_pVertices[iIndices[0]].y || MovableHeight < m_pVertices[iIndices[3]].y - m_pVertices[iIndices[0]].y)
				{
				TargetTile = Tile_JumpMovable;
				}

				break;
			case Tile_ShrinkMovable:
					TargetTile = Tile_ShrinkMovable;
				break;
			}

		}
		else if (m_pNaviTerrain[iIndices[2]] != Tile_Movable)
		{
			switch (_uint(m_pNaviTerrain[iIndices[2]]))
			{
			case Tile_JumpMovable:
				if (MovableHeight < m_pVertices[iIndices[0]].y - m_pVertices[iIndices[2]].y || MovableHeight < m_pVertices[iIndices[3]].y - m_pVertices[iIndices[2]].y)
				{
				TargetTile = Tile_JumpMovable;
				}

				break;
			case Tile_ShrinkMovable:
				TargetTile = Tile_ShrinkMovable;
				break;
			}

		}
		else if (m_pNaviTerrain[iIndices[3]] != Tile_Movable)
		{
			switch (_uint(m_pNaviTerrain[iIndices[3]]))
			{
			case Tile_JumpMovable:
				if (MovableHeight< m_pVertices[iIndices[0]].y - m_pVertices[iIndices[3]].y || MovableHeight < m_pVertices[iIndices[2]].y - m_pVertices[iIndices[3]].y)
				{
				TargetTile = Tile_JumpMovable;
				}

				break;
			case Tile_ShrinkMovable:
				TargetTile = Tile_ShrinkMovable;
				break;
			}

		}
		*pOutKidsOfTile = TargetTile;

		Plane = XMPlaneFromPoints(XMLoadFloat3(&m_pVertices[iIndices[0]]),
			XMLoadFloat3(&m_pVertices[iIndices[2]]), XMLoadFloat3(&m_pVertices[iIndices[3]]));


		if (vNormVector != nullptr)
		{
			*vNormVector = XMVector3Normalize(XMVector3Cross(
				(m_pVertices[iIndices[2]].XMVector()) - (m_pVertices[iIndices[0]].XMVector()),
				(m_pVertices[iIndices[3]].XMVector()) - (m_pVertices[iIndices[0]].XMVector())));
		}
	}
	else
	{//위 012
		if (m_pVertices[iIndices[1]].y < -9999.f)
		{
			*pOutKidsOfTile = Tile_None;
			*pbIsOnTerrain = false;
			return -FLT_MAX;
		}

		if (m_pNaviTerrain[iIndices[0]] == Tile_None || m_pNaviTerrain[iIndices[2]] == Tile_None || m_pNaviTerrain[iIndices[1]] == Tile_None)
		{
			*pOutKidsOfTile = Tile_None;
			*pbIsOnTerrain = false;
			return -FLT_MAX;
		}

		_uint TargetTile = Tile_Movable;
		if (m_pNaviTerrain[iIndices[0]] != Tile_Movable)
		{
			switch (_uint(m_pNaviTerrain[iIndices[0]]))
			{
			case Tile_JumpMovable:
				if (MovableHeight< m_pVertices[iIndices[2]].y  - m_pVertices[iIndices[0]].y || MovableHeight< m_pVertices[iIndices[1]].y - m_pVertices[iIndices[0]].y)
				{
				TargetTile = Tile_JumpMovable;
				}

				break;
			case Tile_ShrinkMovable:
				TargetTile = Tile_ShrinkMovable;
				break;
			}

		}
		else if (m_pNaviTerrain[iIndices[2]] != Tile_Movable)
		{
			switch (_uint(m_pNaviTerrain[iIndices[2]]))
			{
			case Tile_JumpMovable:
				if (MovableHeight< m_pVertices[iIndices[0]].y  - m_pVertices[iIndices[2]].y || MovableHeight< m_pVertices[iIndices[1]].y - m_pVertices[iIndices[2]].y)
				{
				TargetTile = Tile_JumpMovable;
				}

				break;
			case Tile_ShrinkMovable:
				TargetTile = Tile_ShrinkMovable;
				break;
			}

		}
		else if (m_pNaviTerrain[iIndices[1]] != Tile_Movable)
		{
			switch (_uint(m_pNaviTerrain[iIndices[1]]))
			{
			case Tile_JumpMovable:
				if (MovableHeight < m_pVertices[iIndices[0]].y - m_pVertices[iIndices[1]].y || MovableHeight< m_pVertices[iIndices[2]].y- m_pVertices[iIndices[1]].y)
				{
				TargetTile = Tile_JumpMovable;
				}

				break;
			case Tile_ShrinkMovable:
				TargetTile = Tile_ShrinkMovable;
				break;
			}

		}
		*pOutKidsOfTile = TargetTile;

		Plane = XMPlaneFromPoints(XMLoadFloat3(&m_pVertices[iIndices[0]]),
			XMLoadFloat3(&m_pVertices[iIndices[1]]), XMLoadFloat3(&m_pVertices[iIndices[2]]));

		if (vNormVector != nullptr)
		{
			*vNormVector = XMVector3Normalize(XMVector3Cross(
				(m_pVertices[iIndices[1]].XMVector()) - (m_pVertices[iIndices[0]].XMVector()),
				(m_pVertices[iIndices[2]].XMVector()) - (m_pVertices[iIndices[0]].XMVector())));
		}

	}

	if (pCaculateY != nullptr)
		*pCaculateY = ((Plane.x * PosOnTerrainLocal.x + Plane.z * PosOnTerrainLocal.z + Plane.w) / -Plane.y);


	*pbIsOnTerrain = true;
	return (Plane.x * PosOnTerrainLocal.x + Plane.y * PosOnTerrainLocal.y + Plane.z * PosOnTerrainLocal.z + Plane.w);
}

HRESULT CVIBuffer_Terrain::Chage_TileKindsNHeight(_uint iTileKinds, _float3 PosNHeight)
{
	if (PosNHeight.x < 0 || PosNHeight.x >= m_iNumVerticesX ||	PosNHeight.z < 0 || PosNHeight.z >= m_iNumVerticesZ)
		return E_FAIL;
	

	_uint iIndex = _uint(_uint(PosNHeight.z) * m_iNumVerticesX + PosNHeight.x);

	if (PosNHeight.y >= 0)
		m_pVertices[iIndex].y = PosNHeight.y;
	m_pNaviTerrain[iIndex] = _float(iTileKinds);


	return S_OK;
}

_Vector CVIBuffer_Terrain::Calculate_SlidingVector(_float3 OldPosOnTerrainLocal, _float3 PosOnTerrainLocal)
{
	if (OldPosOnTerrainLocal.x < 0 || OldPosOnTerrainLocal.x >= m_iNumVerticesX ||
		OldPosOnTerrainLocal.z < 0 || OldPosOnTerrainLocal.z >= m_iNumVerticesZ)
	{
		return XMVectorSet(0,0,0,0);
	}

	_uint iIndex = _uint(_uint(OldPosOnTerrainLocal.z) * m_iNumVerticesX + OldPosOnTerrainLocal.x);

	_uint		iIndices[4] = {
		iIndex + m_iNumVerticesX,
		iIndex + m_iNumVerticesX + 1,
		iIndex + 1,
		iIndex };


	_float4 Plane;
	_Vector TempNow = PosOnTerrainLocal.XMVector();
	//_Vector TempNow = PosOnTerrainLocal.XMVector()*2.f - OldPosOnTerrainLocal.XMVector();

	if (OldPosOnTerrainLocal.x - m_pVertices[iIndices[0]].x < m_pVertices[iIndices[0]].z - OldPosOnTerrainLocal.z)
	{//아래 023
		//if (m_pVertices[iIndices[3]].y < -9999.f)
		//{
		//	return XMVectorSet(0, 0, 0, 0);;
		//}

	
		_float3 SlidVec;


		if (OutOfLine(m_pVertices[iIndices[0]].XMVector(), m_pVertices[iIndices[2]].XMVector(), OldPosOnTerrainLocal.XMVector(), TempNow, &SlidVec))
			return SlidVec.XMVector();
		
		if (OutOfLine(m_pVertices[iIndices[2]].XMVector(), m_pVertices[iIndices[3]].XMVector(), OldPosOnTerrainLocal.XMVector(), TempNow, &SlidVec))
			return SlidVec.XMVector();

		if (OutOfLine(m_pVertices[iIndices[3]].XMVector(), m_pVertices[iIndices[0]].XMVector(), OldPosOnTerrainLocal.XMVector(), TempNow, &SlidVec))
			return SlidVec.XMVector();

		return XMVectorSet(0, 0, 0, 0);;
	
	}
	else
	{//위 012
		//if (m_pVertices[iIndices[1]].y < -9999.f)
		//{
		//	return XMVectorSet(0, 0, 0, 0);;
		//}

		_float3 SlidVec;


		if (OutOfLine(m_pVertices[iIndices[0]].XMVector(), m_pVertices[iIndices[1]].XMVector(), OldPosOnTerrainLocal.XMVector(), TempNow, &SlidVec))
			return SlidVec.XMVector();

		if (OutOfLine(m_pVertices[iIndices[1]].XMVector(), m_pVertices[iIndices[2]].XMVector(), OldPosOnTerrainLocal.XMVector(), TempNow, &SlidVec))
			return SlidVec.XMVector();

		if (OutOfLine(m_pVertices[iIndices[2]].XMVector(), m_pVertices[iIndices[0]].XMVector(), OldPosOnTerrainLocal.XMVector(), TempNow, &SlidVec))
			return SlidVec.XMVector();

		return XMVectorSet(0, 0, 0, 0);

	}

	return XMVectorSet(0, 0, 0, 0);
}

_Vector CVIBuffer_Terrain::Calculate_SlidingVector_ForNoneTile(_float3 OldPosOnTerrainLocal, _float3 PosOnTerrainLocal, _float Rate)
{
	if (PosOnTerrainLocal.x < 0 || PosOnTerrainLocal.x >= m_iNumVerticesX ||
		PosOnTerrainLocal.z < 0 || PosOnTerrainLocal.z >= m_iNumVerticesZ)
	{
		return (PosOnTerrainLocal.XMVector()- PosOnTerrainLocal.XMVector());
	}

	_uint iIndex = _uint(_uint(PosOnTerrainLocal.z) * m_iNumVerticesX + PosOnTerrainLocal.x);

	_uint		iIndices[4] = {
		iIndex + m_iNumVerticesX,
		iIndex + m_iNumVerticesX + 1,
		iIndex + 1,
		iIndex };


	_float4 Plane;
	_Vector TempNow = PosOnTerrainLocal.XMVector();
	//_Vector TempNow = PosOnTerrainLocal.XMVector()*2.f - OldPosOnTerrainLocal.XMVector();

	if (PosOnTerrainLocal.x - m_pVertices[iIndices[0]].x < m_pVertices[iIndices[0]].z - PosOnTerrainLocal.z)
	{

		_float3 SlidVec;

	
			if (OutOfLine_ForNoneTile(iIndices[2], iIndices[3], OldPosOnTerrainLocal.XMVector(), TempNow, &SlidVec, Rate))
				return SlidVec.XMVector();


			if (OutOfLine_ForNoneTile(iIndices[3], iIndices[0], OldPosOnTerrainLocal.XMVector(), TempNow, &SlidVec, Rate))
				return SlidVec.XMVector();

			if (OutOfLine_ForNoneTile(iIndices[0], iIndices[2], OldPosOnTerrainLocal.XMVector(), TempNow, &SlidVec, Rate))
				return SlidVec.XMVector();

		
		return (OldPosOnTerrainLocal.XMVector() - PosOnTerrainLocal.XMVector());

	}
	else
	{

		_float3 SlidVec;


	
			if (OutOfLine_ForNoneTile(iIndices[0], iIndices[1], OldPosOnTerrainLocal.XMVector(), TempNow, &SlidVec, Rate))
				return SlidVec.XMVector();



			if (OutOfLine_ForNoneTile(iIndices[1], iIndices[2], OldPosOnTerrainLocal.XMVector(), TempNow, &SlidVec, Rate))
				return SlidVec.XMVector();


			if (OutOfLine_ForNoneTile(iIndices[2], iIndices[0], OldPosOnTerrainLocal.XMVector(), TempNow, &SlidVec, Rate))
				return SlidVec.XMVector();
		
		return (OldPosOnTerrainLocal.XMVector() - PosOnTerrainLocal.XMVector());

	}

	return (OldPosOnTerrainLocal.XMVector() - PosOnTerrainLocal.XMVector());
}

_bool CVIBuffer_Terrain::OutOfLine(_fVector StartPoint, _fVector EndPoint, _fVector OldPos, _gVector NowPos, _float3* pOutDirVec)
{
	_int IsUp = 1;

	if (fabs(XMVectorGetY(StartPoint) - XMVectorGetY(EndPoint)) > MovableHeight)
	{
		//IsUp = -1;
		return false;
	}
	_Vector vStart = XMVectorSetY(StartPoint,0);
	_Vector vEnd = XMVectorSetY(EndPoint,0);
	_Vector vOld = XMVectorSetY(OldPos, 0);
	_Vector vNew = XMVectorSetY(NowPos, 0);


	_Vector		vDestDir = XMVector3Normalize(vNew - vStart);
	_Vector		TempVec = vEnd - vStart;
	_Vector		vSourDir = XMVector3Normalize(XMVectorSet(XMVectorGetZ(TempVec)* -1.f, 0, XMVectorGetX(TempVec),0));

	//if (XMVectorGetX(XMVector3Dot(vSourDir, vDestDir)) > 0)
	//{

		_Vector vVerticVec = vSourDir;
		vVerticVec = vVerticVec  * XMVector3Dot(-vNew + vOld, vVerticVec);
		*pOutDirVec = (vNew - vOld + vVerticVec * 1.00001f) * _float(IsUp);

		return true;

	//}	
	return false;
}

_bool CVIBuffer_Terrain::OutOfLine_ForNoneTile(_uint iStartIndex, _uint iEndIndex, _fVector OldPos, _gVector NowPos, _float3 * pOutDirVec, _float Rate )
{

	if (!((m_pNaviTerrain[iStartIndex] != Tile_None && m_pNaviTerrain[iEndIndex] != Tile_None) || (m_pNaviTerrain[iStartIndex] == Tile_None && m_pNaviTerrain[iEndIndex] == Tile_None)))
	{
		return false;
	}



	_Vector vStart = XMVectorSetY(m_pVertices[iStartIndex].XMVector(), 0);
	_Vector vEnd = XMVectorSetY(m_pVertices[iEndIndex].XMVector(), 0);
	_Vector vOld = XMVectorSetY(OldPos, 0);
	_Vector vNew = XMVectorSetY(NowPos, 0);


	//_Vector		vDestDir = XMVector3Normalize(vNew - vStart);
	_Vector		TempVec = vEnd - vStart;
	_Vector		vSourDir = XMVector3Normalize(XMVectorSet(XMVectorGetZ(TempVec)* -1.f, 0, XMVectorGetX(TempVec), 0));

	_Vector vVerticVec = vSourDir;
	vVerticVec = vVerticVec  * XMVector3Dot(vOld -vNew  , vVerticVec);

	_Vector SlidingVector = (vNew - vOld + vVerticVec * Rate);



	*pOutDirVec = (vNew - vOld + vVerticVec * Rate);


	return true;

}



_Vector CVIBuffer_Terrain::Pick_ByRay(_fVector vRayPos, _fVector vRayDir, _float2 vIndex, _bool * bIsPieck)
{
	if (vIndex.x < 0 || vIndex.x >= m_iNumVerticesX ||
		vIndex.y < 0 || vIndex.y >= m_iNumVerticesZ)
	{
		*bIsPieck = false;
		return _Vector();
	}

	_uint iIndex = _uint(_uint(vIndex.y) * m_iNumVerticesX + vIndex.x);

	_uint		iIndices[4] = {
		iIndex + m_iNumVerticesX,
		iIndex + m_iNumVerticesX + 1,
		iIndex + 1,	iIndex };

	_Vector vVertex1, vVertex2, vVertex3;


		vVertex1 = XMLoadFloat3(&m_pVertices[iIndices[0]]);
		vVertex2 = XMLoadFloat3(&m_pVertices[iIndices[2]]);
		vVertex3  = XMLoadFloat3(&m_pVertices[iIndices[3]]);



	_float Dist;
	if (TriangleTests::Intersects(vRayPos, vRayDir, vVertex1, vVertex2, vVertex3, Dist))
	{

		*bIsPieck = true;
		return vRayPos + (vRayDir * Dist) + XMVectorSet(0,0.00001f,0,0);
	}


 	vVertex1 = XMLoadFloat3(&m_pVertices[iIndices[0]]);
	vVertex2 = XMLoadFloat3(&m_pVertices[iIndices[1]]);
	vVertex3 = XMLoadFloat3(&m_pVertices[iIndices[2]]);

	if (TriangleTests::Intersects(vRayPos, vRayDir, vVertex1, vVertex2, vVertex3, Dist))
	{

		*bIsPieck = true;
		return vRayPos + (vRayDir * Dist) + XMVectorSet(0, 0.00001f, 0, 0);
	}

	*bIsPieck = false;
	return _Vector();

}

HRESULT CVIBuffer_Terrain::LoadHeightMap()
{
#ifdef _DEBUG
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);


	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = _uint(m_iNumVerticesX);
	TextureDesc.Height = _uint(m_iNumVerticesZ);
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA			SubResourceData;
	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	_ulong*		pPixel = nullptr;

	pPixel = new _ulong[TextureDesc.Width * TextureDesc.Height];
	ZeroMemory(pPixel, sizeof(_ulong) * TextureDesc.Width * TextureDesc.Height);


	for (_uint i = 0; i < m_iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;
			//_uint		pPixelIndex = (m_iNumVerticesZ - 1 - i) * m_iNumVerticesX + j;


			if (m_pVertices[iIndex].y >= 0)
			{
				_ulong TerrainKinds = _ulong(m_pNaviTerrain[iIndex]);
				_ulong TempHieht = _ulong(m_pVertices[iIndex].y * 100);
				_ulong	Value = _ulong(TempHieht / 100);
				if (Value > 255)Value = 255;
				_ulong	Value2 = TempHieht % 100;
				if (Value2 > 255)Value2 = 255;

				pPixel[iIndex] = D3D11COLOR_ARGB(255, TerrainKinds, Value, Value2);
				//pPixel[iIndex] = D3D11COLOR_ARGB(255, TerrainKinds, Value, Value2);

			}
			else
			{
				_ulong TerrainKinds = Tile_None;

				pPixel[iIndex] = D3D11COLOR_ARGB(0, TerrainKinds, 0, 0);
				//pPixel[iIndex] = D3D11COLOR_ARGB(0, TerrainKinds, 0, 0);

			}
		}
	}



	SubResourceData.pSysMem = pPixel;
	SubResourceData.SysMemPitch = sizeof(_ulong) * TextureDesc.Width;


	ID3D11Texture2D*				pTexture = nullptr;

	FAILED_CHECK(m_pDevice->CreateTexture2D(&TextureDesc, &SubResourceData, &pTexture));

	if (m_pHeightMapSRV)Safe_Release(m_pHeightMapSRV);
	FAILED_CHECK(m_pDevice->CreateShaderResourceView(pTexture, nullptr, &m_pHeightMapSRV));


	//FAILED_CHECK(SaveWICTextureToFile(m_pDeviceContext, pTexture, GUID_ContainerFormatBmp, FileFullPath, &GUID_WICPixelFormat32bppPBGRA));


	Safe_Release(pTexture);
	Safe_Delete_Array(pPixel);
#endif // _DEBUG
	return S_OK;
}

HRESULT CVIBuffer_Terrain::Bind_HeightMapOnShader(CShader * pShader, const char * szValueName)
{

	if (!m_pHeightMapSRV)return S_FALSE;
	//NULL_CHECK_RETURN(m_pHeightMapSRV, E_FAIL);

	return pShader->Set_Texture(szValueName, m_pHeightMapSRV);;
	return S_OK;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pHeightMap)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMap)))
	{
		MSGBOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iNumWidthPixelX, _uint iNumWidthPixelY)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumWidthPixelX, iNumWidthPixelY)))
	{
		MSGBOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();

#ifdef _DEBUG
		Safe_Release(m_pHeightMapSRV);
#endif // _DEBUG

	if (!m_bIsClone)
		Safe_Delete_Array(m_pNaviTerrain);

}
