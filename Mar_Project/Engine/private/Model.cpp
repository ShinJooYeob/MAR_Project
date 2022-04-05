#include "..\public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"



CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs),
	m_vecMeshContainer(rhs.m_vecMeshContainer),
	m_MeshMaterialDesc(rhs.m_MeshMaterialDesc),
	m_eModelType(rhs.m_eModelType),
	m_iNumMeshContainers(rhs.m_iNumMeshContainers)
{
	for (auto& pMeshContainer : m_vecMeshContainer)
		Safe_AddRef(pMeshContainer);

	Safe_AddRef(m_MeshMaterialDesc.pTexture);
}

HRESULT CModel::Initialize_Prototype(MODELTYPE eModelType, const char * pModelFilePath, const char * pModelFileName, _fMatrix TransformMatrix)
{
	FAILED_CHECK(__super::Initialize_Prototype(nullptr));

	ZeroMemory(&m_MeshMaterialDesc, sizeof(MESHMATERIALDESC));

	m_eModelType = eModelType;
	m_TransformMatrix = TransformMatrix;

	char	szFullPath[MAX_PATH] = "../bin/Resources/Mesh/";
	char	szFilePath[MAX_PATH] = "";

	if (TYPE_NONANIM == m_eModelType)
		strcat_s(szFullPath, "StaticMesh/");
	else
		strcat_s(szFullPath, "DynamicMesh/");


	strcat_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, "/");
	strcpy_s(szFilePath, szFullPath);
	strcat_s(szFullPath, pModelFileName);


	_uint		iFlag = 0;

	if (TYPE_NONANIM == m_eModelType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace;

	m_pScene = m_Importer.ReadFile(szFullPath, iFlag);

	NULL_CHECK_RETURN(m_pScene, E_FAIL);

	FAILED_CHECK(Ready_MeshContainers(TransformMatrix));

	FAILED_CHECK(Ready_Materials(szFilePath));


	return S_OK;
}

HRESULT CModel::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(nullptr));


	return S_OK;
}

HRESULT CModel::Render(CShader * pShader, _uint iPassIndex)
{
	for (auto& pMeshContainer : m_vecMeshContainer)
	{
		if (nullptr != pMeshContainer)
		{	
			m_MeshMaterialDesc.pTexture->Bind_OnShader(pShader,"g_DiffuseTexture", 1);
			pMeshContainer->Render(pShader, iPassIndex);
		}
	}
	return S_OK;
}

HRESULT CModel::Ready_MeshContainers(_fMatrix TransformMatrix)
{

	NULL_CHECK_RETURN(m_pScene, E_FAIL);

	m_iNumMeshContainers = m_pScene->mNumMeshes;

	m_vecMeshContainer.reserve(m_iNumMeshContainers);

	for (_uint i = 0; i < m_iNumMeshContainers; ++i)
	{
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pDeviceContext, m_eModelType, m_pScene->mMeshes[i], TransformMatrix);
	
		NULL_CHECK_RETURN(pMeshContainer, E_FAIL);

		m_vecMeshContainer.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char * pModelFilePath)
{
	NULL_CHECK_RETURN(m_pScene, E_FAIL);

	m_iNumMaterials = m_pScene->mNumMaterials;

	//MESHMATERIALDESC		pMeshMaterialDesc;
	//ZeroMemory(&pMeshMaterialDesc, sizeof(MESHMATERIALDESC));

	m_MeshMaterialDesc.pTexture = CTexture::Create(m_pDevice, m_pDeviceContext);

	NULL_CHECK_RETURN(m_MeshMaterialDesc.pTexture, E_FAIL);

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{

		FAILED_CHECK(m_MeshMaterialDesc.pTexture->Insert_Empty_TextureLayer((_tchar*)(to_wstring(i).c_str())));

		for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString	TexturePath;

			if (FAILED(m_pScene->mMaterials[i]->GetTexture(aiTextureType(j), 0, &TexturePath)))
				continue;

			char		szFullPath[MAX_PATH] = "";
			char		szFileName[MAX_PATH] = "";
			char		szExt[MAX_PATH] = "";

			_splitpath_s(TexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szFullPath, pModelFilePath);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar		szTextureFullPath[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szTextureFullPath, MAX_PATH);

			FAILED_CHECK(m_MeshMaterialDesc.pTexture->Insert_Texture_On_BindedLayer(j,szTextureFullPath));
		
		}

		//m_Materials.push_back(pMeshMaterialDesc);
	}
	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, MODELTYPE eModelType, const char * pModelFilePath, const char * pModelFileName, _fMatrix TransformMatrix)
{
	CModel*	pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, pModelFileName, TransformMatrix)))
	{
		MSGBOX("Failed to Created CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*	pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pMeshcontainer : m_vecMeshContainer)
		Safe_Release(pMeshcontainer);

	m_vecMeshContainer.clear();

	Safe_Release(m_MeshMaterialDesc.pTexture);

	m_Importer.FreeScene();
}
