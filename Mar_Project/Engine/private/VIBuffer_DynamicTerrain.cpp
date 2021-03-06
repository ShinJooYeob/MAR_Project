#include "..\Public\VIBuffer_DynamicTerrain.h"
#include "Shader.h"

CVIBuffer_DynamicTerrain::CVIBuffer_DynamicTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext),
	m_pHeightMapSRV(nullptr)
{

}

CVIBuffer_DynamicTerrain::CVIBuffer_DynamicTerrain(const CVIBuffer_DynamicTerrain & rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_fMinMapSize(rhs.m_fMinMapSize)
	,m_pHeightMapSRV(rhs.m_pHeightMapSRV)
	,m_pNaviTerrain(rhs.m_pNaviTerrain)
{
	Safe_AddRef(m_pHeightMapSRV);
}

HRESULT CVIBuffer_DynamicTerrain::Initialize_Prototype(const _tchar* pHeightMap)
{
	FAILED_CHECK(__super::Initialize_Prototype(nullptr));

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	_tchar szHeightMapPath[MAX_PATH] = L"../Bin/Resources/Textures/HeightMap/";

	lstrcat(szHeightMapPath, pHeightMap);

	//if (m_pHeightMapSRV)Safe_Release(m_pHeightMapSRV);
	//FAILED_CHECK(CreateWICTextureFromFile(m_pDevice, szHeightMapPath, nullptr, &m_pHeightMapSRV));
	//NULL_CHECK_RETURN(m_pHeightMapSRV, E_FAIL);



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
	m_VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.StructureByteStride = sizeof(VTXNORTEX);

	m_pKeepVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(m_pKeepVertices, sizeof(VTXNORTEX) * m_iNumVertices);
	m_pVertices = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(_float3) * m_iNumVertices);
	m_pNaviTerrain = new _float[m_iNumVertices];
	ZeroMemory(m_pNaviTerrain, sizeof(_float) * m_iNumVertices);


	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;
			//_float ValueY = (pPixel[iIndex] & 0x000000ff) / 10.f;			

			if ((_ulong((pPixel[iIndex] & 0xff000000) >> 24)) == 0)
			{
				m_pKeepVertices[iIndex].vPosition = m_pVertices[iIndex] = _float3(_float(j), -FLT_MAX, _float(i));

			}
			else
			{

				_float fInt = _float(_ulong((pPixel[iIndex] & 0x0000ff00)>> 8) );
				if (fInt > 255)fInt = 255;
				_float fFlt = _float(_ulong(pPixel[iIndex] & 0x000000ff))*0.01f;
				
				m_pKeepVertices[iIndex].vPosition = m_pVertices[iIndex] = _float3(_float(j), fInt+ fFlt, _float(i));

				//m_pKeepVertices[iIndex].vPosition = m_pVertices[iIndex] = _float3(_float(j), _float(_ulong(pPixel[iIndex] & 0x000000ff)*0.1f), _float(i));
			}

			//m_pKeepVertices[iIndex].vPosition = m_pVertices[iIndex] = _float3(_float(j), (pPixel[iIndex] & 0x000000ff) / 1.f, _float(i));

			m_pKeepVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			m_pKeepVertices[iIndex].vTexUV = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
			m_pNaviTerrain[iIndex] = _float(_ulong((pPixel[iIndex] & 0x00ff0000 )>> 16));
			//m_pNaviTerrain[iIndex] = Tile_Movable;
		}
	}


	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	//?????????? ???? ???????? ???? ?????? ?????????? ??????
	//D3D11_SUBRESOURCE_DATA ???????? ???? ?????? ???? ???? ???????? ?????????? ????.
	//?????? ???? ?????? ???? ?????? ???????? ???????
	m_VBSubResourceData.pSysMem = m_pKeepVertices;

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

				XMStoreFloat3(&m_pKeepVertices[pIndices[iNumFace]._0].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&m_pKeepVertices[pIndices[iNumFace]._0].vNormal) + vNorm)));

				XMStoreFloat3(&m_pKeepVertices[pIndices[iNumFace]._1].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&m_pKeepVertices[pIndices[iNumFace]._1].vNormal) + vNorm)));

				XMStoreFloat3(&m_pKeepVertices[pIndices[iNumFace]._2].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&m_pKeepVertices[pIndices[iNumFace]._2].vNormal) + vNorm)));

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

				XMStoreFloat3(&m_pKeepVertices[pIndices[iNumFace]._0].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&m_pKeepVertices[pIndices[iNumFace]._0].vNormal) + vNorm)));

				XMStoreFloat3(&m_pKeepVertices[pIndices[iNumFace]._1].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&m_pKeepVertices[pIndices[iNumFace]._1].vNormal) + vNorm)));

				XMStoreFloat3(&m_pKeepVertices[pIndices[iNumFace]._2].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&m_pKeepVertices[pIndices[iNumFace]._2].vNormal) + vNorm)));
			}

			++iNumFace;
		}
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

#pragma  endregion 

#pragma region VERTEX_BUFFER


	FAILED_CHECK(Create_VertexBuffer());


#pragma endregion


#pragma region INDEX_BUFFER


	FAILED_CHECK(Create_IndexBuffer());
	Safe_Delete_Array(pIndices);

#pragma endregion
	_float2 vTerrainSize = Get_NumVerticesXY();
	_float fCrossSize = vTerrainSize.Get_Lenth();
	m_fMinMapSize = fCrossSize / 5;
	m_bIsHeightMapChange = true;
	return S_OK;
}

HRESULT CVIBuffer_DynamicTerrain::Initialize_Prototype(_uint iNumWidthPixelX, _uint iNumWidthPixelY)
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
	m_VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.StructureByteStride = sizeof(VTXNORTEX);

	m_pKeepVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(m_pKeepVertices, sizeof(VTXNORTEX) * m_iNumVertices);
	m_pVertices = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(_float3) * m_iNumVertices);
	m_pNaviTerrain = new _float[m_iNumVertices];
	ZeroMemory(m_pNaviTerrain, sizeof(_float) * m_iNumVertices);


	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			m_pKeepVertices[iIndex].vPosition = m_pVertices[iIndex] = _float3(_float(j), 0, _float(i));
			m_pKeepVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			m_pKeepVertices[iIndex].vTexUV = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}


	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	//?????????? ???? ???????? ???? ?????? ?????????? ??????
	//D3D11_SUBRESOURCE_DATA ???????? ???? ?????? ???? ???? ???????? ?????????? ????.
	//?????? ???? ?????? ???? ?????? ???????? ???????
	m_VBSubResourceData.pSysMem = m_pKeepVertices;

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

				XMStoreFloat3(&m_pKeepVertices[pIndices[iNumFace]._0].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&m_pKeepVertices[pIndices[iNumFace]._0].vNormal) + vNorm)));

				XMStoreFloat3(&m_pKeepVertices[pIndices[iNumFace]._1].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&m_pKeepVertices[pIndices[iNumFace]._1].vNormal) + vNorm)));

				XMStoreFloat3(&m_pKeepVertices[pIndices[iNumFace]._2].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&m_pKeepVertices[pIndices[iNumFace]._2].vNormal) + vNorm)));

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

				XMStoreFloat3(&m_pKeepVertices[pIndices[iNumFace]._0].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&m_pKeepVertices[pIndices[iNumFace]._0].vNormal) + vNorm)));

				XMStoreFloat3(&m_pKeepVertices[pIndices[iNumFace]._1].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&m_pKeepVertices[pIndices[iNumFace]._1].vNormal) + vNorm)));

				XMStoreFloat3(&m_pKeepVertices[pIndices[iNumFace]._2].vNormal,
					(XMVector3Normalize(XMLoadFloat3(&m_pKeepVertices[pIndices[iNumFace]._2].vNormal) + vNorm)));
			}

			++iNumFace;
		}
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

#pragma  endregion 

#pragma region CREATE_BUFFER


	FAILED_CHECK(Create_VertexBuffer());


	FAILED_CHECK(Create_IndexBuffer());
	Safe_Delete_Array(pIndices);

#pragma endregion
	_float2 vTerrainSize = Get_NumVerticesXY();
	_float fCrossSize = vTerrainSize.Get_Lenth();
	m_fMinMapSize = fCrossSize / 5;
	
	m_bIsHeightMapChange = true;

	return S_OK;
}

HRESULT CVIBuffer_DynamicTerrain::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	return S_OK;
}



HRESULT CVIBuffer_DynamicTerrain::Bind_HeightMapOnShader(CShader * pShader, const char* szValueName)
{

	if (!m_pHeightMapSRV)return S_FALSE;
	//NULL_CHECK_RETURN(m_pHeightMapSRV, E_FAIL);
	
	return pShader->Set_Texture(szValueName, m_pHeightMapSRV);;
}

HRESULT CVIBuffer_DynamicTerrain::Bake_NavigationTerrain(_float MovableHeight)
{

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_int		iIndex = _int(i * m_iNumVerticesX + j);

			if (m_pKeepVertices[iIndex].vPosition.y < -9999)
			{
				m_pNaviTerrain[iIndex] = Tile_None;
				continue;
			}

			if (m_pNaviTerrain[iIndex] == Tile_None)
			{
				continue;
			}


			_int		iIndices[6] = {
				iIndex - 1, iIndex + 1 ,
				_int(iIndex + m_iNumVerticesX - 1), _int(iIndex + m_iNumVerticesX),
				_int(iIndex - m_iNumVerticesX), _int(iIndex - m_iNumVerticesX + 1),
			};

			_int PivotZ = _int(iIndex / m_iNumVerticesX);
			if (_int(iIndices[0] / m_iNumVerticesX) != PivotZ)
				iIndices[0] = -1;

			if (_int(iIndices[1] / m_iNumVerticesX) != PivotZ)
				iIndices[1] = -1;

			if (_int(iIndices[2] / m_iNumVerticesX) != PivotZ + 1)
				iIndices[2] = -1;

			if (_int(iIndices[3] / m_iNumVerticesX) != PivotZ + 1)
				iIndices[3] = -1;

			if (_int(iIndices[4] / m_iNumVerticesX) != PivotZ - 1)
				iIndices[4] = -1;

			if (_int(iIndices[5] / m_iNumVerticesX) != PivotZ - 1)
				iIndices[5] = -1;


			for (_uint k = 0 ; k < 6 ; k ++)
			{
				if (iIndices[k] < 0 || iIndices[k] > _int(m_iNumVertices))
				{
					iIndices[k] = - 1;
				}
			}


			for (_uint k = 0 ; k < 6; k++)
			{
				if(iIndices[k] == -1) continue;

				if (m_pNaviTerrain[iIndex] == Tile_Movable && 
					m_pKeepVertices[iIndices[k]].vPosition.y > m_pKeepVertices[iIndex].vPosition.y &&
					m_pKeepVertices[iIndices[k]].vPosition.y - m_pKeepVertices[iIndex].vPosition.y > MovableHeight)
				{
					m_pNaviTerrain[iIndex] = Tile_JumpMovable;
				}

			}

		}
	}


	m_bIsHeightMapChange = true;
	return S_OK;
}

HRESULT CVIBuffer_DynamicTerrain::Reset_NavigationTerrain(_uint ClearTarget)
{

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_int		iIndex = _int(i * m_iNumVerticesX + j);
	
			if (m_pNaviTerrain[iIndex] == ClearTarget)
				m_pNaviTerrain[iIndex] = Tile_Movable;
		}
	}


	m_bIsHeightMapChange = true;
	return S_OK;
}	

HRESULT CVIBuffer_DynamicTerrain::Save_HeightMap(const _tchar * FileFullPath)
{
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


	for (_uint i  = 0; i < m_iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;
			_uint		pPixelIndex = (m_iNumVerticesZ - 1 - i) * m_iNumVerticesX + j;


			if (m_pKeepVertices[iIndex].vPosition.y >= 0)
			{
				_ulong TerrainKinds = _ulong(m_pNaviTerrain[iIndex]);
				_ulong TempHieht = _ulong(m_pKeepVertices[iIndex].vPosition.y * 100);
				_ulong	Value = _ulong(TempHieht / 100);
				if (Value > 255)Value = 255;
				_ulong	Value2 = TempHieht % 100;
				if (Value2 > 255)Value2 = 255;

				pPixel[pPixelIndex] = D3D11COLOR_ARGB(255, TerrainKinds, Value, Value2);
				//pPixel[iIndex] = D3D11COLOR_ARGB(255, TerrainKinds, Value, Value2);

			}
			else
			{
				_ulong TerrainKinds = Tile_None;

				pPixel[pPixelIndex] = D3D11COLOR_ARGB(0, TerrainKinds, 0, 0);
				//pPixel[iIndex] = D3D11COLOR_ARGB(0, TerrainKinds, 0, 0);

			}
		}
	}



	SubResourceData.pSysMem = pPixel;
	SubResourceData.SysMemPitch = sizeof(_ulong) * TextureDesc.Width;


	ID3D11Texture2D*				pTexture = nullptr;

	FAILED_CHECK(m_pDevice->CreateTexture2D(&TextureDesc, &SubResourceData, &pTexture));

	FAILED_CHECK(SaveWICTextureToFile(m_pDeviceContext, pTexture, GUID_ContainerFormatBmp, FileFullPath, &GUID_WICPixelFormat32bppPBGRA));

	Safe_Release(pTexture);

	Safe_Delete_Array(pPixel);

	return S_OK;
}

_Vector CVIBuffer_DynamicTerrain::Caculate_TerrainY(_bool* pbIsOnTerrain ,_float3 PosOnTerrainLocal, _float3 OldPosOnTerrainLocal, _float3* vLocalPlaneNormVector)
{
	

	_float CacluatedOld = EquationPlane(pbIsOnTerrain, OldPosOnTerrainLocal);
	//if (*pbIsOnTerrain == false || CacluatedOld < 0)
	//{
	//	*pbIsOnTerrain = false;
	//	return _float3(PosOnTerrainLocal.x, -FLT_MAX, PosOnTerrainLocal.z);
	//}

	_float CaculatedY = 0;
	_float CacluatedNow = EquationPlane(pbIsOnTerrain, PosOnTerrainLocal, &CaculatedY, vLocalPlaneNormVector);

	if (*pbIsOnTerrain == false ) {
		*pbIsOnTerrain = false;
		return PosOnTerrainLocal.XMVector();
	}

	if (PosOnTerrainLocal.y <= CaculatedY && CacluatedOld >= -0.2f)
	{
		*pbIsOnTerrain = true;
		return _float3(PosOnTerrainLocal.x, CaculatedY, PosOnTerrainLocal.z).XMVector();

	}
	else {
		*pbIsOnTerrain = false;
		return PosOnTerrainLocal.XMVector();
	}
}


_Vector CVIBuffer_DynamicTerrain::Caculate_Terrain_Pick_byRay(_bool* pbIsOnTerrain, _float3 PosOnTerrainLocal, _float3 OldPosOnTerrainLocal, _float3* vLocalPlaneNormVector)
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


_float CVIBuffer_DynamicTerrain::EquationPlane(_bool * pbIsOnTerrain, _float3 PosOnTerrainLocal, _float* pCaculateY, _float3* vNormVector)
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

	if (m_pVertices[iIndices[0]].y < -99999.f || m_pVertices[iIndices[2]].y < -99999.f)
	{
		*pbIsOnTerrain = false;
		return -FLT_MAX;
	}

	_float4 Plane;

	if (PosOnTerrainLocal.x - m_pVertices[iIndices[0]].x < m_pVertices[iIndices[0]].z - PosOnTerrainLocal.z)
	{//???? 023
		if (m_pVertices[iIndices[3]].y < -99999.f)
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
	{//?? 012
		if (m_pVertices[iIndices[1]].y < -99999.f)
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

_float CVIBuffer_DynamicTerrain::Get_Kinds(_float2 Pos)
{
	if (Pos.x < 0 || Pos.x >= m_iNumVerticesX ||
		Pos.y < 0 || Pos.y >= m_iNumVerticesZ)
	{
		return -FLT_MAX;
	}

	_uint iIndex = _uint(_uint(Pos.y) * m_iNumVerticesX + Pos.x);


	return m_pNaviTerrain[iIndex];
}

_Vector CVIBuffer_DynamicTerrain::Pick_ByRay(_fVector vRayPos, _fVector vRayDir, _float2 vIndex, _bool * bIsPieck)
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

	for (_uint i = 0 ; i<4; i++)
	{
		if (iIndices[i] >= m_iNumVertices)
		{
			*bIsPieck = false;
			return _Vector();
		}
	}

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

_float CVIBuffer_DynamicTerrain::Get_NowValueY(_float2 vIndex)
{
	if (vIndex.x < 0 || vIndex.x >= m_iNumVerticesX ||
		vIndex.y < 0 || vIndex.y >= m_iNumVerticesZ)
	{
		return NOT_EXIST_FLOAT;
	}

	_uint iIndex = _uint(_uint(vIndex.y) * m_iNumVerticesX + vIndex.x);

	return m_pKeepVertices[iIndex].vPosition.y;
}

HRESULT CVIBuffer_DynamicTerrain::Chage_VertexBuffer(_float2 vChangeVertexIndex, _float fValueY)
{
	m_bIsVertexChange = true;

	_uint		iIndex = _uint(vChangeVertexIndex.y) * m_iNumVerticesX + _uint(vChangeVertexIndex.x);
	m_pKeepVertices[iIndex].vPosition.y = fValueY;

	return S_OK;
}
HRESULT CVIBuffer_DynamicTerrain::Erase_VertexBuffer(_float2 vChangeVertexIndex)
{
	m_bIsVertexChange = true;

	_uint		iIndex = _uint(vChangeVertexIndex.y) * m_iNumVerticesX + _uint(vChangeVertexIndex.x);
	m_pKeepVertices[iIndex].vPosition.y = -999999.f;

	return S_OK;
}

HRESULT CVIBuffer_DynamicTerrain::Chage_KindsOfTile(_float2 vChangeVertexIndex, _float fValueY)
{

	m_bIsHeightMapChange = true;

	_uint		iIndex = _uint(vChangeVertexIndex.y) * m_iNumVerticesX + _uint(vChangeVertexIndex.x);
	m_pNaviTerrain[iIndex] = fValueY;



	return S_OK;
}

HRESULT CVIBuffer_DynamicTerrain::Renew_VertexBuffer()
{
	if (m_bIsVertexChange)
	{

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		FAILED_CHECK(m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
		//  Update the vertex buffer here.
		memcpy(mappedResource.pData, m_pKeepVertices, sizeof(VTXNORTEX) * m_iNumVertices);
		//  Reenable GPU access to the vertex buffer data.
		m_pDeviceContext->Unmap(m_pVB, 0);

		m_bIsVertexChange = false;
	}

	if (m_bIsHeightMapChange)
	{
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


				if (m_pKeepVertices[iIndex].vPosition.y >= 0)
				{
					_ulong TerrainKinds = _ulong(m_pNaviTerrain[iIndex]);
					_ulong TempHieht = _ulong(m_pKeepVertices[iIndex].vPosition.y * 100);
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


		m_bIsHeightMapChange = false;
	}

	return S_OK;
}


CVIBuffer_DynamicTerrain * CVIBuffer_DynamicTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pHeightMap)
{
	CVIBuffer_DynamicTerrain*	pInstance = new CVIBuffer_DynamicTerrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMap)))
	{
		MSGBOX("Failed to Created CVIBuffer_DynamicTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CVIBuffer_DynamicTerrain * CVIBuffer_DynamicTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iNumWidthPixelX, _uint iNumWidthPixelY)
{
	CVIBuffer_DynamicTerrain*	pInstance = new CVIBuffer_DynamicTerrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumWidthPixelX, iNumWidthPixelY)))
	{
		MSGBOX("Failed to Created CVIBuffer_DynamicTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_DynamicTerrain::Clone(void * pArg)
{
	CVIBuffer_DynamicTerrain*	pInstance = new CVIBuffer_DynamicTerrain(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CVIBuffer_DynamicTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_DynamicTerrain::Free()
{
	__super::Free();

	if (!m_bIsClone)
	{
		Safe_Delete_Array(m_pKeepVertices);
		Safe_Delete_Array(m_pNaviTerrain);
	}
		Safe_Release(m_pHeightMapSRV);
}
