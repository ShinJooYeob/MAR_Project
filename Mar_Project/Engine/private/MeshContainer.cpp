#include "MeshContainer.h"
#include "HierarchyNode.h"
#include "Shader.h"



CMeshContainer::CMeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CMeshContainer::CMeshContainer(const CMeshContainer & rhs)
	: CVIBuffer(rhs),
	m_MaterialIndex(rhs.m_MaterialIndex)
{
}

HRESULT CMeshContainer::Initialize_Prototype(CModel::MODELTYPE eMeshtype, aiMesh * pAIMesh, _fMatrix TransformMatrix)
{
	m_pAIMesh = pAIMesh;

#pragma region VERTICES
	m_iNumIndicesPerPrimitive = 3;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumVertexBuffers = 1;
	
	HRESULT			hr = 0;

	if (CModel::TYPE_NONANIM == eMeshtype)
		hr = Ready_NonAnimMeshContainer(pAIMesh, TransformMatrix);
	else
		hr = Ready_AnimMeshContainer(pAIMesh);

	m_MaterialIndex = pAIMesh->mMaterialIndex;

#pragma endregion


#pragma region INDICES
	m_iNumPrimitive = pAIMesh->mNumFaces;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(FACEINDICES32) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		pIndices[i]._0 = pAIMesh->mFaces[i].mIndices[0];
		pIndices[i]._1 = pAIMesh->mFaces[i].mIndices[1];
		pIndices[i]._2 = pAIMesh->mFaces[i].mIndices[2];
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);


#pragma endregion



	return S_OK;
}

HRESULT CMeshContainer::Initialize_Clone(void * pArg)
{
	return S_OK;
}

HRESULT CMeshContainer::Bind_AffectingBones_OnShader(CShader* pShader, _fMatrix DefultPivotMatrix, _float4x4* pBoneMatrices, const char* szBoneName)
{
	ZeroMemory(pBoneMatrices, sizeof(_float4x4) * 128);
	_uint	iIndex = 0;

	for (auto& pHierarchyNode : m_vecAffectingBones)
	{
		_Matrix UpdatedMatrix = pHierarchyNode->Get_UpdatedMatrix();

		XMStoreFloat4x4(&pBoneMatrices[iIndex++], XMMatrixTranspose(UpdatedMatrix*DefultPivotMatrix));
		//XMStoreFloat4x4(&pBoneMatrices[iIndex++], XMMatrixTranspose(UpdatedMatrix));
	}

	FAILED_CHECK(pShader->Set_RawValue(szBoneName, pBoneMatrices, sizeof(_float4x4) * 128));

	return S_OK;
}

_uint CMeshContainer::Get_MaterialIndex()
{
	return m_MaterialIndex;
}

HRESULT CMeshContainer::Add_AffectingBone(CHierarchyNode * pHierarchyNode)
{
	m_vecAffectingBones.push_back(pHierarchyNode);

	Safe_AddRef(pHierarchyNode);

	return S_OK;
}

HRESULT CMeshContainer::Ready_NonAnimMeshContainer(aiMesh * pAIMesh, _fMatrix TransformMatrix)
{

	m_VBDesc.ByteWidth = sizeof(VTXMODEL) * m_iNumVertices;
	m_VBDesc.StructureByteStride = sizeof(VTXMODEL);
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	VTXMODEL*	pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), TransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), TransformMatrix));

		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);


	return S_OK;
}

HRESULT CMeshContainer::Ready_AnimMeshContainer(aiMesh * pAIMesh)
{
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = sizeof(VTXANIMMODEL) * m_iNumVertices;
	m_VBDesc.StructureByteStride = sizeof(VTXANIMMODEL);
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	VTXANIMMODEL*	pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	FAILED_CHECK(Ready_SkinnedInfo(pAIMesh, pVertices));


	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	FAILED_CHECK(Create_VertexBuffer());

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMeshContainer::Ready_SkinnedInfo(aiMesh* pAIMesh, VTXANIMMODEL * pVertices)
{
	NULL_CHECK_RETURN(pAIMesh, E_FAIL);

	
	m_iNumAffectingBones = pAIMesh->mNumBones;

	//이 매쉬에 영향을 끼치는 모든 뼈들을 순차적으로 돌면서
	for (_uint i = 0 ; i< m_iNumAffectingBones; i++)
	{
		aiBone*	 pAffectingBone = pAIMesh->mBones[i];

		//해당 뼈 하나가 영향을 끼치는 모든 정점들을 순회하면서 값을 채워줌

		for (_uint j = 0 ;  j < pAffectingBone->mNumWeights; j++)
		{
			//이 뼈가 영향을 끼치는 j 번째 정점에게 얼마만큼의 영향을 주는지  -> pAffectingBone->mWeights[j].mWeight;
			// j  번째 뼈는 이 매쉬의 몇번째 정점인지							->pAffectingBone->mWeights[j].mVertexId
			
			if (0.0f == pVertices[pAffectingBone->mWeights[j].mVertexId].vBlendWeight.x)
			{
				pVertices[pAffectingBone->mWeights[j].mVertexId].vBlendIndex.x = i;
				pVertices[pAffectingBone->mWeights[j].mVertexId].vBlendWeight.x = pAffectingBone->mWeights[j].mWeight;
			}
			else if (0.0f == pVertices[pAffectingBone->mWeights[j].mVertexId].vBlendWeight.y)
			{
				pVertices[pAffectingBone->mWeights[j].mVertexId].vBlendIndex.y = i;
				pVertices[pAffectingBone->mWeights[j].mVertexId].vBlendWeight.y = pAffectingBone->mWeights[j].mWeight;
			}
			else if (0.0f == pVertices[pAffectingBone->mWeights[j].mVertexId].vBlendWeight.z)
			{
				pVertices[pAffectingBone->mWeights[j].mVertexId].vBlendIndex.z = i;
				pVertices[pAffectingBone->mWeights[j].mVertexId].vBlendWeight.z = pAffectingBone->mWeights[j].mWeight;
			}
			else if (0.0f == pVertices[pAffectingBone->mWeights[j].mVertexId].vBlendWeight.w)
			{
				pVertices[pAffectingBone->mWeights[j].mVertexId].vBlendIndex.w = i;
				pVertices[pAffectingBone->mWeights[j].mVertexId].vBlendWeight.w = pAffectingBone->mWeights[j].mWeight;
			}


		}

	}



	return S_OK;
}

CMeshContainer * CMeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, 
	CModel::MODELTYPE eMeshtype, aiMesh * pAIMesh, _fMatrix TransformMatrix)
{
	CMeshContainer*	pInstance = new CMeshContainer(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(eMeshtype, pAIMesh, TransformMatrix)))
	{
		MSGBOX("Failed to Created CMeshContainer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CMeshContainer::Clone(void * pArg)
{
	CMeshContainer*	pInstance = new CMeshContainer(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CMeshContainer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMeshContainer::Free()
{
	__super::Free();

	for (auto& pHierarchyNode : m_vecAffectingBones)
		Safe_Release(pHierarchyNode);

	m_vecAffectingBones.clear();

}
