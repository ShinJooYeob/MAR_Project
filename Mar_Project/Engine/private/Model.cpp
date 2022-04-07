#include "..\public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "HierarchyNode.h"



CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs),
	m_vecMeshContainerArr(rhs.m_vecMeshContainerArr),
	m_MeshMaterialDesc(rhs.m_MeshMaterialDesc),
	m_eModelType(rhs.m_eModelType),
	m_iNumMeshContainers(rhs.m_iNumMeshContainers),
	m_iNumMaterials(rhs.m_iNumMaterials),
	m_vecHierarchyNode(rhs.m_vecHierarchyNode)
{
	for (auto& pHierarchyNode : m_vecHierarchyNode)
		Safe_AddRef(pHierarchyNode);
	

	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		for (auto& pMeshcontainer : m_vecMeshContainerArr[i])
			Safe_AddRef(pMeshcontainer);
	}

	Safe_AddRef(m_MeshMaterialDesc.pTexture);
}

HRESULT CModel::Initialize_Prototype(MODELTYPE eModelType, const char * pModelFilePath, const char * pModelFileName, _fMatrix DefaultPivotMatrix)
{
	FAILED_CHECK(__super::Initialize_Prototype(nullptr));

	ZeroMemory(&m_MeshMaterialDesc, sizeof(MESHMATERIALDESC));

	m_eModelType = eModelType;
	m_DefaultPivotMatrix = DefaultPivotMatrix;

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

	if (TYPE_ANIM == m_eModelType)
	{
		//애니메이션을 써야하는 모델들은 뼈구조를 받아와야함
		FAILED_CHECK(Ready_HierarchyNodes(m_pScene->mRootNode));

		sort(m_vecHierarchyNode.begin(), m_vecHierarchyNode.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
		{
			return pSour->Get_Depth() < pDest->Get_Depth();
		});

		//뼈 구조가 매쉬의 로컬상태와 맞지 않을 수 있기 떄문에 보정해주는 메트릭스를 계층뼈에 저장
		FAILED_CHECK(Ready_OffsetMatrices());

	}
	FAILED_CHECK(Ready_MeshContainers(DefaultPivotMatrix));

	FAILED_CHECK(Ready_Materials(szFilePath));


	return S_OK;
}

HRESULT CModel::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(nullptr));


	return S_OK;
}

HRESULT CModel::Bind_OnShader(CShader * pShader, _uint iMaterialIndex, _uint eTextureType, const char * pHlslConstValueName)
{
	if (iMaterialIndex  >= m_iNumMaterials|| pShader == nullptr || m_MeshMaterialDesc.pTexture == nullptr)
		return E_FAIL;

	FAILED_CHECK(m_MeshMaterialDesc.pTexture->Change_TextureLayer(to_wstring(iMaterialIndex).c_str()));

	if (FAILED(m_MeshMaterialDesc.pTexture->NullCheckTexture(eTextureType)))
		return S_FALSE;

	if (FAILED(m_MeshMaterialDesc.pTexture->Bind_OnShader(pShader, pHlslConstValueName, eTextureType)))
	{
#ifdef  _DEBUG
		OutputDebugStringW(L"Failed to Bind 3D Model Texture on Shader\n");
#endif //  _DEBUG
		return S_FALSE;
	}


	return S_OK;
}

HRESULT CModel::Render(CShader * pShader, _uint iPassIndex,_uint iMaterialIndex)
{


	for (auto& pMeshContainer : m_vecMeshContainerArr[iMaterialIndex])
	{
		m_MeshMaterialDesc.pTexture->Change_TextureLayer(to_wstring(iMaterialIndex).c_str());

		FAILED_CHECK(m_MeshMaterialDesc.pTexture->NullCheckBinedTextureLayer());

		m_MeshMaterialDesc.pTexture->Bind_OnShader(pShader, "g_DiffuseTexture", 1);

		pMeshContainer->Render(pShader, iPassIndex);
	}



	return S_OK;
}

HRESULT CModel::Ready_HierarchyNodes(aiNode * pNode, CHierarchyNode * pParent, _uint iDepth)
{
	NULL_CHECK_RETURN(pNode, E_FAIL);

	_float4x4		matTransformation;
	memcpy(&matTransformation, &pNode->mTransformation, sizeof(_float4x4));

	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(pParent, pNode->mName.data, &matTransformation, iDepth);
	NULL_CHECK_RETURN(pHierarchyNode, E_FAIL);

	m_vecHierarchyNode.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
		FAILED_CHECK( Ready_HierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth + 1));
	

	return S_OK;
}

HRESULT CModel::Ready_OffsetMatrices()
{

	for (_uint i = 0; i < m_iNumMeshContainers; ++i)
	{
		//각각의 매쉬들이 영향을 받는 모든 뼈들을 순회하면서
		for (_uint j = 0; j < m_pScene->mMeshes[i]->mNumBones; ++j)
		{
			//특정 매쉬가 영향을 받는 뼈들 중 j번째 뼈와 
			//이름이 같은 뼈를 계층뼈에서 찾아서
			aiBone*		pBone = m_pScene->mMeshes[i]->mBones[j];
			CHierarchyNode*		pHierarchyNode = Find_HierarchyNode(pBone->mName.data);


			NULL_CHECK_RETURN(pHierarchyNode, E_FAIL);

			_float4x4		OffsetMatrix;
			memcpy(&OffsetMatrix, &pBone->mOffsetMatrix, sizeof(_float4x4));

			//계층뼈에 오프셋 매트릭스를 저장하자
			pHierarchyNode->Set_OffsetMatrix(OffsetMatrix);
		}
	}




	return S_OK;
}

HRESULT CModel::Ready_MeshContainers(_fMatrix TransformMatrix)
{

	NULL_CHECK_RETURN(m_pScene, E_FAIL);

	m_iNumMeshContainers = m_pScene->mNumMeshes;
	m_iNumMaterials = m_pScene->mNumMaterials;

	m_vecMeshContainerArr = new MESHCONTAINERS[m_iNumMaterials];

	for (_uint i = 0; i < m_iNumMeshContainers; ++i)
	{
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pDeviceContext, m_eModelType, m_pScene->mMeshes[i], TransformMatrix);
		NULL_CHECK_RETURN(pMeshContainer, E_FAIL);
		m_vecMeshContainerArr[m_pScene->mMeshes[i]->mMaterialIndex].push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char * pModelFilePath)
{
	NULL_CHECK_RETURN(m_pScene, E_FAIL);


	ZeroMemory(&m_MeshMaterialDesc, sizeof(MESHMATERIALDESC));

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


			if (!strcmp(szExt, ".tga"))
			{
				ZeroMemory(szExt, sizeof(char)*MAX_PATH);
				strcpy_s(szExt, ".png");
			}


			strcpy_s(szFullPath, pModelFilePath);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);


			_tchar		szTextureFullPath[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szTextureFullPath, MAX_PATH);

			FAILED_CHECK(m_MeshMaterialDesc.pTexture->Insert_Texture_On_BindedLayer(j, szTextureFullPath));
		
		}
	}
	return S_OK;
}

CHierarchyNode * CModel::Find_HierarchyNode(const char * pName)
{
	auto	iter = find_if(m_vecHierarchyNode.begin(), m_vecHierarchyNode.end(), [&](CHierarchyNode* pNode)
	{
		return !strcmp(pNode->Get_Name(), pName);
	});

	if (iter == m_vecHierarchyNode.end())
		return nullptr;

	return *iter;
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
	
	for (auto& pHierarchyNode : m_vecHierarchyNode)
			Safe_Release(pHierarchyNode);


	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		for (auto& pMeshcontainer : m_vecMeshContainerArr[i])
		{
			CMeshContainer* TempForRelease = pMeshcontainer;
			Safe_Release(TempForRelease);
		}
		if (!m_bIsClone)
			m_vecMeshContainerArr[i].clear();
	}
	if (!m_bIsClone)
		Safe_Delete_Array(m_vecMeshContainerArr);


	Safe_Release(m_MeshMaterialDesc.pTexture);

	m_Importer.FreeScene();
}
