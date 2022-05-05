#include "stdafx.h"
#include "..\public\Terrain.h"



CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext) 
	: CGameObject(pDevice, pDeviceContext)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CTerrain::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());

	return S_OK;
}

_int CTerrain::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	//m_InverseWorldMat = m_pTransformCom->Get_InverseWorldMatrix();





	return _int();
}

_int CTerrain::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this));

	return _int();
}

_int CTerrain::Render()
{
	if (__super::Render() < 0)
		return -1;
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	CGameInstance* pInstance = GetSingle(CGameInstance);


	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamPosition", &pInstance->Get_TargetPostion_float4(PLV_CAMERA), sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamLookDir", &pInstance->Get_TargetPostion_float4(PLV_CAMLOOK), sizeof(_float4)));


	const LIGHTDESC* pLightDesc = pInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
	NULL_CHECK_RETURN(pLightDesc, -1);


	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightVector", &(pLightDesc->vVector), sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4)));

	_float fMinMap = m_pVIBufferCom->Get_MinMapSize();
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_fMimMapSize", &fMinMap, sizeof(_float)));

	FAILED_CHECK(m_pTextureCom->Change_TextureLayer(L"Diffuse"));
	FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_SourDiffuseTexture", 0));
	FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DestDiffuseTexture1", 1));
	FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DestDiffuseTexture2", 2));
	FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DestDiffuseTexture3", 3));
	FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DestDiffuseTexture4", 4));

	if (m_pFilterMap != nullptr)
		FAILED_CHECK(m_pShaderCom->Set_Texture("g_FilterTexture", m_pFilterMap));

#ifdef _DEBUG
	static _bool	IsDebug = false;
	
	if (g_pGameInstance->Get_DIKeyState(DIK_F6)&DIS_Down)IsDebug = !IsDebug;

	if (IsDebug)
	{
		FAILED_CHECK(m_pVIBufferCom->Bind_HeightMapOnShader(m_pShaderCom, "g_HeightMapTexture"));

		FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, 2));
	}
	else
#endif // _DEBUG
	FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, 0));

	return _int();
}

_int CTerrain::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;




	return _int();
}

_float3 CTerrain::PutOnTerrain(_bool* pbIsObTerrain,_fVector ObjectWorldPos, _fVector ObjectOldWorldPos, _float3* vOutPlaneNormalVec, _uint* eNowTile )
{

	_Matrix InverMat = m_InverseWorldMat.XMatrix();

	_Vector CaculatedFloat3 = m_pVIBufferCom->Caculate_TerrainY(pbIsObTerrain,
		(XMVector3TransformCoord(ObjectWorldPos, InverMat)), (XMVector3TransformCoord(ObjectOldWorldPos, InverMat)), vOutPlaneNormalVec, eNowTile);

	if (*eNowTile == Tile_None)
	{
		if (vOutPlaneNormalVec != nullptr)
			*vOutPlaneNormalVec = XMVector3TransformNormal(vOutPlaneNormalVec->XMVector(), m_pTransformCom->Get_WorldMatrix());

		return XMVector3TransformCoord(CaculatedFloat3, m_pTransformCom->Get_WorldMatrix());
	}

	if (XMVectorGetY(ObjectOldWorldPos) < XMVectorGetY(ObjectWorldPos))
		return ObjectWorldPos;

	if (*pbIsObTerrain)
	{
		if (vOutPlaneNormalVec != nullptr)
			*vOutPlaneNormalVec = XMVector3TransformNormal(vOutPlaneNormalVec->XMVector(), m_pTransformCom->Get_WorldMatrix());

		return XMVector3TransformCoord(CaculatedFloat3, m_pTransformCom->Get_WorldMatrix());
	}

	return ObjectWorldPos;
}

_bool CTerrain::Check_Movable_Terrain(_bool * pbIsMovable, _fVector ObjectNowPos, _fVector CheckPos, _float fMovableHeight)
{
	_Matrix InverMat = m_InverseWorldMat.XMatrix();

	_uint eKinds = m_pVIBufferCom->Get_TerrainKinds(pbIsMovable, XMVector3TransformCoord(CheckPos, InverMat));

	if (eKinds != Tile_Movable)
	{
		*pbIsMovable = false;
		return false;
	}


	_float NowTerrainHeight = m_pVIBufferCom->Get_TerrainHeight(pbIsMovable, XMVector3TransformCoord(ObjectNowPos, InverMat));
	_float TargetHeight = m_pVIBufferCom->Get_TerrainHeight(pbIsMovable, XMVector3TransformCoord(CheckPos, InverMat));

	if (*pbIsMovable && abs(NowTerrainHeight - TargetHeight) < fMovableHeight)
	{
		*pbIsMovable = true;
		return true;
	}

	*pbIsMovable = false;
	return false;

}

HRESULT CTerrain::Chage_TileKindsNHeight(_fMatrix WorldPointsMat)
{

	_float4x4 PointsOnLocal = WorldPointsMat * m_pTransformCom->Get_InverseWorldMatrix();


	RECT Points;

	_float Height = PointsOnLocal._12;

	_float LowHight = (min(min(min(PointsOnLocal._12, PointsOnLocal._22), PointsOnLocal._32), PointsOnLocal._42));
	_float MaxHight = (max(max(max(PointsOnLocal._12, PointsOnLocal._22), PointsOnLocal._32), PointsOnLocal._42));



	Points.left = _uint(min(min(min(PointsOnLocal._11, PointsOnLocal._21), PointsOnLocal._31), PointsOnLocal._41) -1);
	Points.bottom = _uint(min(min(min(PointsOnLocal._13, PointsOnLocal._23), PointsOnLocal._33), PointsOnLocal._43) -1);
	Points.right = _uint(max(max(max(PointsOnLocal._11, PointsOnLocal._21), PointsOnLocal._31), PointsOnLocal._41) +1);
	Points.top = _uint(max(max(max(PointsOnLocal._13, PointsOnLocal._23), PointsOnLocal._33), PointsOnLocal._43) +1);

	_float HeightIndterver = (MaxHight - LowHight) / (Points.top - Points.bottom -2 ); 
	
		
	for (_uint i = (_uint)Points.left; i <= (_uint)Points.right; i++)
	{
		for (_uint j = (_uint)Points.bottom; j <= (_uint)Points.top; j++)
		{

			if (i == Points.left || i == Points.right || j == Points.bottom || j == Points.top)
				m_pVIBufferCom->Chage_TileKindsNHeight(Tile_JumpMovable, _float3((_float)i, -1, (_float)j));

			else
				m_pVIBufferCom->Chage_TileKindsNHeight(Tile_Movable, _float3((_float)i, LowHight + ((j - Points.bottom -1) * HeightIndterver), (_float)j));

		}

	}


	return S_OK;
}

HRESULT CTerrain::Chage_TileKindsMovableNZero(_fMatrix WorldPointsMat)
{
	_float4x4 PointsOnLocal = WorldPointsMat * m_pTransformCom->Get_InverseWorldMatrix();


	RECT Points;

	//_float Height = PointsOnLocal._12;

	//_float LowHight = (min(min(min(PointsOnLocal._12, PointsOnLocal._22), PointsOnLocal._32), PointsOnLocal._42));
	//_float MaxHight = (max(max(max(PointsOnLocal._12, PointsOnLocal._22), PointsOnLocal._32), PointsOnLocal._42));



	Points.left = _uint(min(min(min(PointsOnLocal._11, PointsOnLocal._21), PointsOnLocal._31), PointsOnLocal._41) - 1);
	Points.bottom = _uint(min(min(min(PointsOnLocal._13, PointsOnLocal._23), PointsOnLocal._33), PointsOnLocal._43) - 1);
	Points.right = _uint(max(max(max(PointsOnLocal._11, PointsOnLocal._21), PointsOnLocal._31), PointsOnLocal._41) + 1);
	Points.top = _uint(max(max(max(PointsOnLocal._13, PointsOnLocal._23), PointsOnLocal._33), PointsOnLocal._43) + 1);

	//_float HeightIndterver = (MaxHight - LowHight) / (Points.top - Points.bottom - 2);


	for (_uint i = (_uint)Points.left; i <= (_uint)Points.right; i++)
	{
		for (_uint j = (_uint)Points.bottom; j <= (_uint)Points.top; j++)
		{

			//if (i == Points.left || i == Points.right || j == Points.bottom || j == Points.top)
			//	m_pVIBufferCom->Chage_TileKindsNHeight(Tile_JumpMovable, _float3((_float)i, -1, (_float)j));

			//else
				m_pVIBufferCom->Chage_TileKindsNHeight(Tile_Movable, _float3((_float)i, 0, (_float)j));

		}

	}


	return S_OK;
}

HRESULT CTerrain::SetUp_Components()
{


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNT), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_VIBuffer_Terrain), TAG_COM(Com_VIBuffer), (CComponent**)&m_pVIBufferCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Texture_Terrain), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));


	CTransform::TRANSFORMDESC tDesc = {};

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));


	FAILED_CHECK(Ready_FilterMap());

	m_InverseWorldMat = m_pTransformCom->Get_InverseWorldMatrix();



	_float2 vTerrainSize = m_pVIBufferCom->Get_NumVerticesXY();
	_float fCrossSize = vTerrainSize.Get_Lenth();
	_float m_fMimMapSize = fCrossSize / 5;

	return S_OK;
}

HRESULT CTerrain::Ready_FilterMap()
{
	if (m_pFilterMap != nullptr) Safe_Release(m_pFilterMap);

	char FileName[MAX_PATH];

	switch (m_eNowSceneNum)
	{
	case SCENE_STATIC:
		break;
	case SCENE_LOBY:
		strcpy_s(FileName, "Filter_0.bmp");
		break;
	case SCENE_LOADING:
		break;
	case SCENE_STAGESELECT:
		strcpy_s(FileName, "Filter_0.bmp");
		break;
	case SCENE_STAGE1:
		strcpy_s(FileName, "Filter_0.bmp");
		break;
	case SCENE_EDIT:
		break;
	case SCENE_END:
		break;
	default:
		break;
	}


	char szFileFath[MAX_PATH] = "../bin/Resources/Textures/Terrain/Filter/";
	strcat_s(szFileFath, FileName);

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

	D3D11_TEXTURE2D_DESC TextureDesc;

	ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = ih.biWidth;
	TextureDesc.Height = ih.biHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	_ulong* m_pFilterPixel = new _ulong[TextureDesc.Width * TextureDesc.Height];
	//ZeroMemory(m_pFilterPixel, sizeof(_ulong) * m_tTextureDesc.Width * m_tTextureDesc.Height);
	ReadFile(hFile, m_pFilterPixel, sizeof(_ulong) * TextureDesc.Width  * TextureDesc.Height, &dwByte, nullptr);


	CloseHandle(hFile);



	D3D11_SUBRESOURCE_DATA			SubResourceData;
	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));


	SubResourceData.pSysMem = m_pFilterPixel;
	SubResourceData.SysMemPitch = sizeof(_ulong) * TextureDesc.Width;

	ID3D11Texture2D* pTexture = nullptr;

	FAILED_CHECK(m_pDevice->CreateTexture2D(&TextureDesc, &SubResourceData, &pTexture));

	FAILED_CHECK(m_pDevice->CreateShaderResourceView(pTexture, nullptr, &m_pFilterMap));


	Safe_Release(pTexture);
	Safe_Delete_Array(m_pFilterPixel);

	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CTerrain*	pInstance = new CTerrain(pDevice,pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*	pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);


	Safe_Release(m_pFilterMap);
}
