#include "stdafx.h"
#include "..\public\WireTerrain.h"



CWireTerrain::CWireTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext) 
	: CGameObject(pDevice, pDeviceContext)
{
}

CWireTerrain::CWireTerrain(const CWireTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWireTerrain::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CWireTerrain::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());

	FAILED_CHECK(Create_FilterMap());


	m_vPickedPos = NOT_EXIST_VECTOR;
	return S_OK;
}

_int CWireTerrain::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	//m_InverseWorldMat = m_pTransformCom->Get_InverseWorldMatrix();




	return _int();
}

_int CWireTerrain::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	FAILED_CHECK(m_pVIBufferCom->Renew_VertexBuffer());

	FAILED_CHECK(Renew_FilterMap());

	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this));

	return _int();
}

_int CWireTerrain::Render()
{
	if (__super::Render() < 0)
		return -1;
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	CGameInstance* pInstance = GetSingle(CGameInstance);


	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));

	if (m_iPassIndex == 3)
	{

		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamPosition", &pInstance->Get_TargetPostion_float4(PLV_CAMERA), sizeof(_float4)));
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamLookDir", &pInstance->Get_TargetPostion_float4(PLV_CAMLOOK), sizeof(_float4)));

	
			FAILED_CHECK(m_pShaderCom->Set_RawValue("g_fRadius", &(m_fRadius), sizeof(_float)));
			FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vBrushPos", &(_float4(m_vPickedPos, 1)), sizeof(_float4)));
			
		


		const LIGHTDESC* pLightDesc = pInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
		NULL_CHECK_RETURN(pLightDesc, -1);

		
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightVector", &(pLightDesc->vVector), sizeof(_float4)));
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4)));
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4)));
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4)));

		FAILED_CHECK(m_pTextureCom->Change_TextureLayer(L"Diffuse"));
		FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_SourDiffuseTexture", 0));
		FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DestDiffuseTexture1", 1));
		FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DestDiffuseTexture2", 2));
		FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DestDiffuseTexture3", 3));
		FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DestDiffuseTexture4", 4));

		FAILED_CHECK(m_pTextureCom->Change_TextureLayer(L"Brush"));
		FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_BrushTexture", 0));

		
		_float fMinMap = m_pVIBufferCom->Get_MinMapSize();
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_fMimMapSize", &fMinMap, sizeof(_float)));

		if (m_pFilterMap != nullptr)
			FAILED_CHECK(m_pShaderCom->Set_Texture("g_FilterTexture", m_pFilterMap));

		//FAILED_CHECK(m_pTextureCom->Change_TextureLayer(L"Filter"));
		//FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_FilterTexture", 0));

	}
	FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, m_iPassIndex));

	return _int();
}

_int CWireTerrain::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;




	return _int();
}

_float3 CWireTerrain::Pick_OnTerrain(_bool * pbIsObTerrain, _fVector ObjectWorldPos, _fVector ObjectOldWorldPos, _float3 * vOutPlaneNormalVec)
{
	if (XMVectorGetY(ObjectOldWorldPos) < XMVectorGetY(ObjectWorldPos))
		return ObjectWorldPos;

	_Matrix InverMat = m_InverseWorldMat.XMatrix();

	_Vector CaculatedFloat3 = m_pVIBufferCom->Caculate_Terrain_Pick_byRay(pbIsObTerrain,
		(XMVector3TransformCoord(ObjectWorldPos, InverMat)), (XMVector3TransformCoord(ObjectOldWorldPos, InverMat)), vOutPlaneNormalVec);


	if (*pbIsObTerrain)
	{
		if (vOutPlaneNormalVec != nullptr)
			*vOutPlaneNormalVec = XMVector3TransformNormal(vOutPlaneNormalVec->XMVector(), m_pTransformCom->Get_WorldMatrix());

		return XMVector3TransformCoord(CaculatedFloat3, m_pTransformCom->Get_WorldMatrix());
	}

	return ObjectWorldPos;
}

HRESULT CWireTerrain::Change_VertexBuffer(_float2 vChangedVertexIndex, _float fValueY)
{
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);

	return m_pVIBufferCom->Chage_VertexBuffer(vChangedVertexIndex, fValueY);
}


HRESULT CWireTerrain::Easing_Terrain_Curve(EasingTypeID eEasingType, _float3 vPosition, _float fTargetHeight, _float fRadius)
{
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);

	_Matrix InverMat = m_InverseWorldMat.XMatrix();
	_float3 vLocalPosition = XMVector3TransformCoord(vPosition.XMVector(), InverMat);
	

	_uint minX, maxX, minZ, maxZ;

	minX = _uint(vLocalPosition.x - fRadius);
	maxX = _uint(vLocalPosition.x + fRadius);
	minZ = _uint(vLocalPosition.z - fRadius);
	maxZ = _uint(vLocalPosition.z + fRadius);

	CGameInstance* pInstance = GetSingle(CGameInstance);

	for (_uint i = minX; i <= maxX; i++)
	{
		for (_uint j = minZ; j < maxZ; j++)
		{
			_float2 Temp = { _float(i),_float(j) };
			_float fDist = Temp.Get_Distance(_float2(vLocalPosition.x, vLocalPosition.z).XMVector());

			if (fDist < fRadius)
			{

				_float OldY = m_pVIBufferCom->Get_NowValueY(Temp);

				if(OldY <= NOT_EXIST_FLOAT)
					continue;

				_float ValueY = pInstance->Easing(eEasingType, fTargetHeight, OldY, fDist, fRadius);

				FAILED_CHECK(Change_VertexBuffer(Temp, ValueY));
			}
		}
	}
	return S_OK;
}

HRESULT CWireTerrain::Erasing_TerrainBuffer(_float3 vPosition, _float fRadius)
{
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);



	_Matrix InverMat = m_InverseWorldMat.XMatrix();
	_float3 vLocalPosition = XMVector3TransformCoord(vPosition.XMVector(), InverMat);

	_uint minX, maxX, minZ, maxZ;

	minX = _uint(vLocalPosition.x - fRadius);
	maxX = _uint(vLocalPosition.x + fRadius);
	minZ = _uint(vLocalPosition.z - fRadius);
	maxZ = _uint(vLocalPosition.z + fRadius);

	CGameInstance* pInstance = GetSingle(CGameInstance);

	for (_uint i = minX; i <= maxX; i++)
	{
		for (_uint j = minZ; j < maxZ; j++)
		{
			_float2 Temp = { _float(i),_float(j) };
			_float fDist = Temp.Get_Distance(_float2(vLocalPosition.x, vLocalPosition.z).XMVector());

			if (fDist < fRadius)
			{

				_float OldY = m_pVIBufferCom->Get_NowValueY(Temp);

				if (OldY <= NOT_EXIST_FLOAT)
					continue;

				FAILED_CHECK(m_pVIBufferCom->Erase_VertexBuffer(Temp));
			}
		}
	}

	return S_OK;
}

HRESULT CWireTerrain::Save_HeightMap(const _tchar * FileFullpath)
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);

	FAILED_CHECK(m_pVIBufferCom->Save_HeightMap(FileFullpath));

	//FAILED_CHECK(SaveWICTextureToFile(m_pDeviceContext, m_pFilterTexture, GUID_ContainerFormatPng, FileFullpath, &GUID_WICPixelFormat32bppBGRA));
	//FAILED_CHECK(SaveWICTextureToFile(m_pDeviceContext, m_pFilterTexture, GUID_ContainerFormatBmp, FileFullpath, &GUID_WICPixelFormat32bppBGRA));
	return S_OK;
}


HRESULT CWireTerrain::Draw_FilterMap(_uint iFilterMapIndex, _float3 vPosition, _float fTargetHeight, _float fRadius, _bool bEasing)
{
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);


	_Matrix InverMat = m_InverseWorldMat.XMatrix();
	_float3 vLocalPosition = XMVector3TransformCoord(vPosition.XMVector(), InverMat);


	_uint minX, maxX, minZ, maxZ;

	minX = _uint(vLocalPosition.x - fRadius);
	maxX = _uint(vLocalPosition.x + fRadius);
	minZ = _uint(vLocalPosition.z - fRadius);
	maxZ = _uint(vLocalPosition.z + fRadius);

	CGameInstance* pInstance = GetSingle(CGameInstance);




	for (_uint i = minX; i <= maxX; i++)
	{
		for (_uint j = minZ; j < maxZ; j++)
		{
			_float2 Temp = { _float(i),_float(j) };
			_float fDist = Temp.Get_Distance(_float2(vLocalPosition.x, vLocalPosition.z).XMVector());

			if (fDist < fRadius)
			{

				_float OldY = m_pVIBufferCom->Get_NowValueY(Temp);
				if (OldY <= NOT_EXIST_FLOAT)
					continue;

				_ulong iIndex = (j)* m_tTextureDesc.Width + i;
				
				_ulong OldColor;
				switch (iFilterMapIndex)
				{
				case 0:
					OldColor = _ulong((m_pFilterPixel[iIndex] & 0xff000000) >> 24);
					break;
				case 1:
					OldColor = _ulong((m_pFilterPixel[iIndex] & 0x00ff0000) >> 16);
					break;
				case 2:
					OldColor = _ulong((m_pFilterPixel[iIndex] & 0x0000ff00) >> 8);
					break;
				case 3:
					OldColor = _ulong((m_pFilterPixel[iIndex] & 0x000000ff));
					break;

				default:
					break;
				}
				
				

				_float ValueY = (pInstance->Easing(TYPE_Linear, fTargetHeight, _float(OldColor), fDist, fRadius));

				if (ValueY > 255) ValueY = 255;
				if (ValueY < 0)  ValueY = 0;
				_ulong Temp = 0;
				switch (iFilterMapIndex)
				{
				case 0:
					Temp = m_pFilterPixel[iIndex] & 0x00ffffff;
					m_pFilterPixel[iIndex] = Temp | D3D11COLOR_ARGB(_ulong(ValueY), 0, 0, 0);
					break;
				case 1:
					Temp = m_pFilterPixel[iIndex] & 0xff00ffff;
					m_pFilterPixel[iIndex] = Temp | D3D11COLOR_ARGB(0, _ulong(ValueY), 0, 0);
					break;
				case 2:
					Temp = m_pFilterPixel[iIndex] & 0xffff00ff;
					m_pFilterPixel[iIndex] = Temp | D3D11COLOR_ARGB(0, 0, _ulong(ValueY), 0);
					break;
				case 3:
					Temp = m_pFilterPixel[iIndex] & 0xffffff00;
					m_pFilterPixel[iIndex] = Temp | D3D11COLOR_ARGB(0, 0, 0, _ulong(ValueY));
					break;

				default:
					break;
				}


				m_bIsFilterChaged = true;
			}
		}
	}

	return S_OK;
}




HRESULT CWireTerrain::SetUp_Components()
{


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNT), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_VIBuffer_Terrain), TAG_COM(Com_VIBuffer), (CComponent**)&m_pVIBufferCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Texture_Edit_Terrain), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));

	m_pTextureCom->Change_TextureLayer(L"Diffuse");
	
	CTransform::TRANSFORMDESC tDesc = {};

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));
	m_InverseWorldMat = m_pTransformCom->Get_InverseWorldMatrix();
	

	return S_OK;
}

HRESULT CWireTerrain::Create_FilterMap()
{
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);

	if (m_pFilterMap != nullptr) Safe_Release(m_pFilterMap);
	if (m_pFilterTexture != nullptr) Safe_Release(m_pFilterTexture);
	if (m_pFilterPixel != nullptr)Safe_Delete_Array(m_pFilterPixel);

	_float2 VertexXZ = m_pVIBufferCom->Get_NumVerticesXY();


	ZeroMemory(&m_tTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	m_tTextureDesc.Width = _uint(VertexXZ.x);
	m_tTextureDesc.Height = _uint(VertexXZ.y);
	m_tTextureDesc.MipLevels = 1;
	m_tTextureDesc.ArraySize = 1;
	m_tTextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	m_tTextureDesc.SampleDesc.Count = 1;
	m_tTextureDesc.SampleDesc.Quality = 0;
	m_tTextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_tTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	m_tTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA			SubResourceData;
	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	m_pFilterPixel = new _ulong[m_tTextureDesc.Width * m_tTextureDesc.Height];
	ZeroMemory(m_pFilterPixel, sizeof(_ulong) * m_tTextureDesc.Width * m_tTextureDesc.Height);


	SubResourceData.pSysMem = m_pFilterPixel;
	SubResourceData.SysMemPitch = sizeof(_ulong) * m_tTextureDesc.Width;

	if (FAILED(m_pDevice->CreateTexture2D(&m_tTextureDesc, &SubResourceData, &m_pFilterTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pFilterTexture, nullptr, &m_pFilterMap)))
		return E_FAIL;


	return S_OK;
}

HRESULT CWireTerrain::Create_FilterMap_byLoad(const char * pFileFullPath)
{
	if (m_pFilterMap != nullptr) Safe_Release(m_pFilterMap);
	if (m_pFilterTexture != nullptr) Safe_Release(m_pFilterTexture);
	if (m_pFilterPixel != nullptr)Safe_Delete_Array(m_pFilterPixel);


	char szFileFath[MAX_PATH] = "../bin/Resources/Textures/Terrain/Filter/";
	strcat_s(szFileFath, pFileFullPath);

	_tchar szWidePath[MAX_PATH] = L"";

	MultiByteToWideChar(CP_UTF8, 0, szFileFath, -1, szWidePath, sizeof(szWidePath));


	HANDLE		hFile = CreateFile(szWidePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile)
	{
		__debugbreak();
		return E_FAIL;
	}

	_ulong					dwByte = 0;
	BITMAPFILEHEADER		fh;
	BITMAPINFOHEADER		ih;

	

	ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	ZeroMemory(&m_tTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	m_tTextureDesc.Width = ih.biWidth;
	m_tTextureDesc.Height = ih.biHeight;
	m_tTextureDesc.MipLevels = 1;
	m_tTextureDesc.ArraySize = 1;
	m_tTextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	m_tTextureDesc.SampleDesc.Count = 1;
	m_tTextureDesc.SampleDesc.Quality = 0;
	m_tTextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_tTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	m_tTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	m_pFilterPixel = new _ulong[m_tTextureDesc.Width * m_tTextureDesc.Height];
	//ZeroMemory(m_pFilterPixel, sizeof(_ulong) * m_tTextureDesc.Width * m_tTextureDesc.Height);
	ReadFile(hFile, m_pFilterPixel, sizeof(_ulong) * m_tTextureDesc.Width  * m_tTextureDesc.Height, &dwByte, nullptr);


	CloseHandle(hFile);


	D3D11_SUBRESOURCE_DATA			SubResourceData;
	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));


	SubResourceData.pSysMem = m_pFilterPixel;
	SubResourceData.SysMemPitch = sizeof(_ulong) * m_tTextureDesc.Width;

	FAILED_CHECK(m_pDevice->CreateTexture2D(&m_tTextureDesc, &SubResourceData, &m_pFilterTexture));

	FAILED_CHECK(m_pDevice->CreateShaderResourceView(m_pFilterTexture, nullptr, &m_pFilterMap));


	return S_OK;
}

HRESULT CWireTerrain::Delete_FilterMap()
{

	Safe_Release(m_pFilterMap);
	Safe_Release(m_pFilterTexture);
	Safe_Delete_Array(m_pFilterPixel);

	return S_OK;
}

HRESULT CWireTerrain::Save_FilterMap(const _tchar* FileFullpath)
{

	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);
	NULL_CHECK_RETURN(m_pFilterTexture, E_FAIL);

	//FAILED_CHECK(SaveWICTextureToFile(m_pDeviceContext, m_pFilterTexture, GUID_ContainerFormatPng, FileFullpath, &GUID_WICPixelFormat32bppBGRA));
	FAILED_CHECK(SaveWICTextureToFile(m_pDeviceContext, m_pFilterTexture, GUID_ContainerFormatBmp, FileFullpath, &GUID_WICPixelFormat32bppBGRA));



	return S_OK;
}

HRESULT CWireTerrain::Renew_FilterMap()
{
	if (!m_bIsFilterChaged) return S_FALSE;
	if(m_pFilterPixel == nullptr)return S_FALSE;

	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);

	if (m_pFilterMap != nullptr) Safe_Release(m_pFilterMap);
	if (m_pFilterTexture != nullptr) Safe_Release(m_pFilterTexture);

	D3D11_SUBRESOURCE_DATA			SubResourceData;
	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));


	SubResourceData.pSysMem = m_pFilterPixel;
	SubResourceData.SysMemPitch = sizeof(_ulong) * m_tTextureDesc.Width;

	FAILED_CHECK(m_pDevice->CreateTexture2D(&m_tTextureDesc, &SubResourceData, &m_pFilterTexture));

	FAILED_CHECK(m_pDevice->CreateShaderResourceView(m_pFilterTexture, nullptr, &m_pFilterMap));
	
	m_bIsFilterChaged = false;
	return S_OK;
}

CWireTerrain * CWireTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CWireTerrain*	pInstance = new CWireTerrain(pDevice,pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CWireTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWireTerrain::Clone(void * pArg)
{
	CWireTerrain*	pInstance = new CWireTerrain(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CWireTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWireTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);

	Safe_Release(m_pFilterMap);
	Safe_Release(m_pFilterTexture);
	Safe_Delete_Array(m_pFilterPixel);
}
