#include "..\public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "AnimationClip.h"
#include "ClipBone.h"



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
	m_vecHierarchyNode(rhs.m_vecHierarchyNode),
	m_DefaultPivotMatrix(rhs.m_DefaultPivotMatrix),
	//////////////////////////////////////////////////////////////////////////
	m_vecAnimator(rhs.m_vecAnimator),
	m_iNumAnimationClip(rhs.m_iNumAnimationClip)
{
	for (auto& pAnimationClip : m_vecAnimator)
		Safe_AddRef(pAnimationClip);
	//////////////////////////////////////////////////////////////////////////
	for (auto& pHierarchyNode : m_vecHierarchyNode)
		Safe_AddRef(pHierarchyNode);
	

	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		for (auto& pMeshcontainer : m_vecMeshContainerArr[i])
			Safe_AddRef(pMeshcontainer);
	}

	Safe_AddRef(m_MeshMaterialDesc.pTexture);
}

HRESULT CModel::Initialize_Prototype(MODELTYPE eModelType, const char * pModelFilePath, const char * pModelFileName, _fMatrix DefaultPivotMatrix, _uint iAnimCount)
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
		//�ִϸ��̼��� ����ϴ� �𵨵��� �������� �޾ƿ;���
		FAILED_CHECK(Ready_HierarchyNodes(m_pScene->mRootNode));

		sort(m_vecHierarchyNode.begin(), m_vecHierarchyNode.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
		{
			return pSour->Get_Depth() < pDest->Get_Depth();
		});


#ifdef _DEBUG
		string szLog = "Mesh Name : " + string(pModelFileName) + "		Num Bones : " + to_string(m_vecHierarchyNode.size()) + "\n";
		wstring DebugLog;
		DebugLog.assign(szLog.begin(), szLog.end());

		OutputDebugStringW(DebugLog.c_str());
#endif

	}
	FAILED_CHECK(Ready_MeshContainers(DefaultPivotMatrix));

	FAILED_CHECK(Ready_Materials(szFilePath));


	if (TYPE_ANIM == m_eModelType)
	{
		//�� ������ �Ž��� ���û��¿� ���� ���� �� �ֱ� ������ �������ִ� ��Ʈ������ �������� ����
		FAILED_CHECK(Ready_OffsetMatrices());

		FAILED_CHECK(Ready_Animation());

		if (iAnimCount != 1)
			FAILED_CHECK(Ready_MoreAnimation(szFullPath, iAnimCount, iFlag));

		string ttszLog = "Num AnimationClip: " + to_string(m_iNumAnimationClip) + "\n";
		wstring ttDebugLog;
		ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

		OutputDebugStringW(ttDebugLog.c_str());
	}
	
	return S_OK;
}

HRESULT CModel::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(nullptr));


	return S_OK;
}

HRESULT CModel::Change_AnimIndex(_uint iAnimIndex)
{
	if (iAnimIndex >= m_iNumAnimationClip)
		return E_FAIL;

	m_iCurrentAnimIndex = iAnimIndex;

	return S_OK;
}

HRESULT CModel::Change_AnimIndex_Wait(_uint iAnimIndex)
{
	if (iAnimIndex >= m_iNumAnimationClip)
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Change_AnimIndex_ReturnTo(_uint iAnimIndex, _uint iReturnIndex)
{
	if (iAnimIndex >= m_iNumAnimationClip|| iReturnIndex >= m_iNumAnimationClip)
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Change_AnimIndex_ReturnToWait(_uint iAnimIndex, _uint iReturnIndex)
{
	if (iAnimIndex >= m_iNumAnimationClip || iReturnIndex >= m_iNumAnimationClip)
		return E_FAIL;

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

HRESULT CModel::Update_AnimationClip(_double fDeltaTime)
{

	_bool TestBool = false;
	//�ش� �ִϸ��̼��� ���� Ʈ��������Ʈ������ �������ְ�
	FAILED_CHECK(m_vecAnimator[m_iCurrentAnimIndex]->Update_TransformMatrices_byClipBones(&TestBool,fDeltaTime));
	if (TestBool)
	{
		m_iCurrentAnimIndex++;
		if (m_iCurrentAnimIndex >= m_iNumAnimationClip)
			m_iCurrentAnimIndex = 0;
	}

	//���ŵ� ��Ʈ������ ���� �Ĺ��ε� ��Ʈ������ ������Ʈ ���ش�.
	for (auto& pHierarchyNode : m_vecHierarchyNode)
		pHierarchyNode->Update_CombinedMatrix();

	return S_OK;
}

HRESULT CModel::Render(CShader * pShader, _uint iPassIndex,_uint iMaterialIndex, const char* szBoneValueName)
{
	if (iMaterialIndex >= m_iNumMaterials || nullptr == m_MeshMaterialDesc.pTexture)
		return E_FAIL;

	if (TYPE_ANIM == m_eModelType)
	{
		NULL_CHECK_RETURN(szBoneValueName, E_FAIL);
		_float4x4		BoneMatrices[128];
		_Matrix matDefualtPivot = m_DefaultPivotMatrix.XMatrix();
		for (auto& pMeshContainer : m_vecMeshContainerArr[iMaterialIndex])
		{
			FAILED_CHECK(pMeshContainer->Bind_AffectingBones_OnShader(pShader, matDefualtPivot,BoneMatrices, szBoneValueName));


			m_MeshMaterialDesc.pTexture->Change_TextureLayer(to_wstring(iMaterialIndex).c_str());
			FAILED_CHECK(m_MeshMaterialDesc.pTexture->NullCheckBinedTextureLayer());
			m_MeshMaterialDesc.pTexture->Bind_OnShader(pShader, "g_DiffuseTexture", 1);
			pMeshContainer->Render(pShader, iPassIndex);
		}
	}
	else
	{
		for (auto& pMeshContainer : m_vecMeshContainerArr[iMaterialIndex])
		{
			m_MeshMaterialDesc.pTexture->Change_TextureLayer(to_wstring(iMaterialIndex).c_str());

			FAILED_CHECK(m_MeshMaterialDesc.pTexture->NullCheckBinedTextureLayer());

			m_MeshMaterialDesc.pTexture->Bind_OnShader(pShader, "g_DiffuseTexture", 1);

			pMeshContainer->Render(pShader, iPassIndex);
		}


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

	for (_uint i = 0 ; i < m_iNumMaterials; i++)
	{
		for (auto& pMeshContainer : m_vecMeshContainerArr[i])
		{
			_uint iNumAffectingBones = pMeshContainer->Get_NumAffectingBones();
			aiMesh*		pAIMesh = pMeshContainer->Get_AiMesh();

			NULL_CHECK_RETURN(pAIMesh, E_FAIL);

			//������ �Ž����� ������ �޴� ��� ������ ��ȸ�ϸ鼭
			for (_uint j = 0 ; j < iNumAffectingBones; j++)
			{

				//Ư�� �Ž��� ������ �޴� ���� �� j��° ���� 
				//�̸��� ���� ���� ���������� ã�Ƽ�
				CHierarchyNode*		pHierarchyNode = Find_HierarchyNode(pAIMesh->mBones[j]->mName.data);

				NULL_CHECK_RETURN(pHierarchyNode, E_FAIL);

				_float4x4		OffsetMatrix;
				memcpy(&OffsetMatrix, &(pAIMesh->mBones[j]->mOffsetMatrix), sizeof(_float4x4));

				//�������� ������ ��Ʈ������ ��������
				pHierarchyNode->Set_OffsetMatrix(&OffsetMatrix);
				pMeshContainer->Add_AffectingBone(pHierarchyNode);

			}

		}
	}


	//for (_uint i = 0; i < m_iNumMeshContainers; ++i)
	//{
	//	//������ �Ž����� ������ �޴� ��� ������ ��ȸ�ϸ鼭
	//	for (_uint j = 0; j < m_pScene->mMeshes[i]->mNumBones; ++j)
	//	{
	//		//Ư�� �Ž��� ������ �޴� ���� �� j��° ���� 
	//		//�̸��� ���� ���� ���������� ã�Ƽ�
	//		aiBone*		pBone = m_pScene->mMeshes[i]->mBones[j];
	//		CHierarchyNode*		pHierarchyNode = Find_HierarchyNode(pBone->mName.data);


	//		NULL_CHECK_RETURN(pHierarchyNode, E_FAIL);

	//		_float4x4		OffsetMatrix;
	//		memcpy(&OffsetMatrix, &pBone->mOffsetMatrix, sizeof(_float4x4));

	//		//�������� ������ ��Ʈ������ ��������
	//		pHierarchyNode->Set_OffsetMatrix(OffsetMatrix);
	//	}
	//}




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

HRESULT CModel::Ready_Animation()
{
	NULL_CHECK_RETURN(m_pScene, E_FAIL);

	//�ش� �𵨿� �����ش� �� �ִϸ��̼��� ����
	m_iNumAnimationClip = m_pScene->mNumAnimations;

	//��ŭ ��� �ִϸ��̼��� ��ȸ�ϸ鼭
	for (_uint i = 0 ; i < m_iNumAnimationClip; i++)
	{
		//�ش� �𵨿� �����ϴ� i��° �ִϸ��̼�
		aiAnimation* paiAnimation = m_pScene->mAnimations[i];

		//�� ������ �ִϸ��̼� Ŭ���� �ϳ� �����
		CAnimationClip* pAinmationClip = CAnimationClip::Create(paiAnimation->mName.data, paiAnimation->mDuration, paiAnimation->mTicksPerSecond);
		NULL_CHECK_RETURN(pAinmationClip,E_FAIL);

		//���� �ִϸ��̼� Ŭ���� �ִϸ��̼� �� ������ŭ ������ Ȯ���ϰ�
		pAinmationClip->Reserve(paiAnimation->mNumChannels);
		

		for (_uint j = 0 ; j < paiAnimation->mNumChannels; j++)
		{
			//�ش� �ִϸ��̼ǿ��� ������ �޴� j��° ���� ������ �̿��ؼ�
			aiNodeAnim*	pAIChannel = paiAnimation->mChannels[j];

			//�ش� ���� ���� �̸��� ���̾Ű ��带 ã�Ƽ�
			CHierarchyNode*		pHierarchyNode = Find_HierarchyNode(pAIChannel->mNodeName.data);
			NULL_CHECK_RETURN(pHierarchyNode, E_FAIL);

			//�ش� ���� �����
			CClipBone* pClipBone = CClipBone::Create(pAIChannel->mNodeName.data, pHierarchyNode);
			NULL_CHECK_RETURN(pClipBone,E_FAIL);

			//�ش� ���� �ִ� Ű������(�ش� ���� �ִϸ��̼� ������� ���������ϴ� ����)�� ���ؼ�
			_uint		iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
			iNumKeyFrames = max(iNumKeyFrames, pAIChannel->mNumPositionKeys);

			//�ش� ������ Ű������ ������ Ȯ���س���
			pClipBone->Reserve(iNumKeyFrames);

			//���� �������� ���� ���������� ä���ֱ� ���� ����
			//ex �� 5�������� 3�����ӹۿ� ���� �ִϸ��̼��� ���� ���� �������� �����ϴ� �뵵
			_float3		vScale;
			_float4		vRotation;
			_float3		vPosition;
			_double		Time;

			//��� Ű�������� ��ȸ�ϸ鼭
			for (_uint k = 0 ; k < iNumKeyFrames; k++)
			{
				//Ű�������� �����Ѵ�
				KEYFRAME*			pKeyFrame = new KEYFRAME;
				ZeroMemory(pKeyFrame, sizeof(KEYFRAME));


				//���� �������� ���� ���������� ä���ֱ� ���� ���� �˻�
				if (pAIChannel->mNumScalingKeys > k)
				{
					vScale = _float3(pAIChannel->mScalingKeys[k].mValue);
					Time = pAIChannel->mScalingKeys[k].mTime;
				}
				if (pAIChannel->mNumRotationKeys > k)
				{
					vRotation = _float4(pAIChannel->mRotationKeys[k].mValue);
					Time = pAIChannel->mRotationKeys[k].mTime;
				}
				if (pAIChannel->mNumPositionKeys > k)
				{
					vPosition = _float3(pAIChannel->mPositionKeys[k].mValue);
					Time = pAIChannel->mPositionKeys[k].mTime;
				}

				//���� �˻縦 ��ģ ������ ������ Ű�����ӿ� �־��ְ�
				pKeyFrame->vScale = vScale;
				pKeyFrame->vRotation = vRotation;
				pKeyFrame->vPosition = vPosition;
				pKeyFrame->Time = Time;

				//Ű�������� ��������� ���� �ش� Ű������(������)�� �־��ش�
				pClipBone->Add_KeyFrame(pKeyFrame);
			}

			//Ű�����ӱ��� �� ä���� ���� �����Ǿ����� �ش� ���� �ִϸ��̼� Ŭ���� �־��ְ�
			pAinmationClip->Add_ClipBone(pClipBone);
		}

		//���������� ���� ���� ������ ��� �� ������ �ִϸ��̼��� ���ϸ����Ϳ� �־��ش�.
		m_vecAnimator.push_back(pAinmationClip);
	}


	return S_OK;
}

HRESULT CModel::Ready_MoreAnimation(const char * szFileFullPath, _uint iAnimCount, _uint iFlag )
{
	char FullPath[MAX_PATH] = "";

	strcpy_s(FullPath, string(szFileFullPath).substr(0,strlen(szFileFullPath) - 4).c_str());


	for (_uint AnimIndex = 0; AnimIndex< iAnimCount - 1; AnimIndex++)
	{
		char LoopFileFullPath[MAX_PATH] = "";
		char LoopFilePath[MAX_PATH] = "";
		sprintf_s(LoopFilePath, "_%d.FBX", AnimIndex);

		strcpy_s(LoopFileFullPath, FullPath);
		strcat_s(LoopFileFullPath, LoopFilePath);


		const aiScene*				pScene = nullptr;
		Importer				Importer;

		pScene = Importer.ReadFile(LoopFileFullPath, iFlag);


		NULL_CHECK_RETURN(pScene, E_FAIL);

		//�ش� �𵨿� �����ش� �� �ִϸ��̼��� ����
		_uint iNumAnimClip = pScene->mNumAnimations;
		m_iNumAnimationClip += iNumAnimClip;

		//��ŭ ��� �ִϸ��̼��� ��ȸ�ϸ鼭
		for (_uint i = 0; i < iNumAnimClip; i++)
		{
			//�ش� �𵨿� �����ϴ� i��° �ִϸ��̼�
			aiAnimation* paiAnimation = pScene->mAnimations[i];

			//�� ������ �ִϸ��̼� Ŭ���� �ϳ� �����
			CAnimationClip* pAinmationClip = CAnimationClip::Create(paiAnimation->mName.data, paiAnimation->mDuration, paiAnimation->mTicksPerSecond);
			NULL_CHECK_RETURN(pAinmationClip, E_FAIL);

			//���� �ִϸ��̼� Ŭ���� �ִϸ��̼� �� ������ŭ ������ Ȯ���ϰ�
			pAinmationClip->Reserve(paiAnimation->mNumChannels);


			for (_uint j = 0; j < paiAnimation->mNumChannels; j++)
			{
				//�ش� �ִϸ��̼ǿ��� ������ �޴� j��° ���� ������ �̿��ؼ�
				aiNodeAnim*	pAIChannel = paiAnimation->mChannels[j];

				//�ش� ���� ���� �̸��� ���̾Ű ��带 ã�Ƽ�
				CHierarchyNode*		pHierarchyNode = Find_HierarchyNode(pAIChannel->mNodeName.data);
				NULL_CHECK_RETURN(pHierarchyNode, E_FAIL);

				//�ش� ���� �����
				CClipBone* pClipBone = CClipBone::Create(pAIChannel->mNodeName.data, pHierarchyNode);
				NULL_CHECK_RETURN(pClipBone, E_FAIL);

				//�ش� ���� �ִ� Ű������(�ش� ���� �ִϸ��̼� ������� ���������ϴ� ����)�� ���ؼ�
				_uint		iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
				iNumKeyFrames = max(iNumKeyFrames, pAIChannel->mNumPositionKeys);

				//�ش� ������ Ű������ ������ Ȯ���س���
				pClipBone->Reserve(iNumKeyFrames);

				//���� �������� ���� ���������� ä���ֱ� ���� ����
				//ex �� 5�������� 3�����ӹۿ� ���� �ִϸ��̼��� ���� ���� �������� �����ϴ� �뵵
				_float3		vScale;
				_float4		vRotation;
				_float3		vPosition;
				_double		Time;

				//��� Ű�������� ��ȸ�ϸ鼭
				for (_uint k = 0; k < iNumKeyFrames; k++)
				{
					//Ű�������� �����Ѵ�
					KEYFRAME*			pKeyFrame = new KEYFRAME;
					ZeroMemory(pKeyFrame, sizeof(KEYFRAME));


					//���� �������� ���� ���������� ä���ֱ� ���� ���� �˻�
					if (pAIChannel->mNumScalingKeys > k)
					{
						vScale = _float3(pAIChannel->mScalingKeys[k].mValue);
						Time = pAIChannel->mScalingKeys[k].mTime;
					}
					if (pAIChannel->mNumRotationKeys > k)
					{
						vRotation = _float4(pAIChannel->mRotationKeys[k].mValue);
						Time = pAIChannel->mRotationKeys[k].mTime;
					}
					if (pAIChannel->mNumPositionKeys > k)
					{
						vPosition = _float3(pAIChannel->mPositionKeys[k].mValue);
						Time = pAIChannel->mPositionKeys[k].mTime;
					}

					//���� �˻縦 ��ģ ������ ������ Ű�����ӿ� �־��ְ�
					pKeyFrame->vScale = vScale;
					pKeyFrame->vRotation = vRotation;
					pKeyFrame->vPosition = vPosition;
					pKeyFrame->Time = Time;

					//Ű�������� ��������� ���� �ش� Ű������(������)�� �־��ش�
					pClipBone->Add_KeyFrame(pKeyFrame);
				}

				//Ű�����ӱ��� �� ä���� ���� �����Ǿ����� �ش� ���� �ִϸ��̼� Ŭ���� �־��ְ�
				pAinmationClip->Add_ClipBone(pClipBone);
			}

			//���������� ���� ���� ������ ��� �� ������ �ִϸ��̼��� ���ϸ����Ϳ� �־��ش�.
			m_vecAnimator.push_back(pAinmationClip);
		}


		Importer.FreeScene();
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

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, MODELTYPE eModelType, const char * pModelFilePath, const char * pModelFileName, _fMatrix TransformMatrix, _uint iAnimCount)
{
	CModel*	pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, pModelFileName, TransformMatrix, iAnimCount)))
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
	
	for (auto& pAnimationClip : m_vecAnimator)
	{
		Safe_Release(pAnimationClip);
	}
	m_vecAnimator.clear();

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
