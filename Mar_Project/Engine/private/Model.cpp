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
		//애니메이션을 써야하는 모델들은 뼈구조를 받아와야함
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
		//뼈 구조가 매쉬의 로컬상태와 맞지 않을 수 있기 떄문에 보정해주는 메트릭스를 계층뼈에 저장
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
	//해당 애니메이션을 따라서 트렌스폼매트릭스를 갱신해주고
	FAILED_CHECK(m_vecAnimator[m_iCurrentAnimIndex]->Update_TransformMatrices_byClipBones(&TestBool,fDeltaTime));
	if (TestBool)
	{
		m_iCurrentAnimIndex++;
		if (m_iCurrentAnimIndex >= m_iNumAnimationClip)
			m_iCurrentAnimIndex = 0;
	}

	//갱신된 매트릭스를 따라서 컴바인드 메트릭스를 업데이트 해준다.
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

			//각각의 매쉬들이 영향을 받는 모든 뼈들을 순회하면서
			for (_uint j = 0 ; j < iNumAffectingBones; j++)
			{

				//특정 매쉬가 영향을 받는 뼈들 중 j번째 뼈와 
				//이름이 같은 뼈를 계층뼈에서 찾아서
				CHierarchyNode*		pHierarchyNode = Find_HierarchyNode(pAIMesh->mBones[j]->mName.data);

				NULL_CHECK_RETURN(pHierarchyNode, E_FAIL);

				_float4x4		OffsetMatrix;
				memcpy(&OffsetMatrix, &(pAIMesh->mBones[j]->mOffsetMatrix), sizeof(_float4x4));

				//계층뼈에 오프셋 매트릭스를 저장하자
				pHierarchyNode->Set_OffsetMatrix(&OffsetMatrix);
				pMeshContainer->Add_AffectingBone(pHierarchyNode);

			}

		}
	}


	//for (_uint i = 0; i < m_iNumMeshContainers; ++i)
	//{
	//	//각각의 매쉬들이 영향을 받는 모든 뼈들을 순회하면서
	//	for (_uint j = 0; j < m_pScene->mMeshes[i]->mNumBones; ++j)
	//	{
	//		//특정 매쉬가 영향을 받는 뼈들 중 j번째 뼈와 
	//		//이름이 같은 뼈를 계층뼈에서 찾아서
	//		aiBone*		pBone = m_pScene->mMeshes[i]->mBones[j];
	//		CHierarchyNode*		pHierarchyNode = Find_HierarchyNode(pBone->mName.data);


	//		NULL_CHECK_RETURN(pHierarchyNode, E_FAIL);

	//		_float4x4		OffsetMatrix;
	//		memcpy(&OffsetMatrix, &pBone->mOffsetMatrix, sizeof(_float4x4));

	//		//계층뼈에 오프셋 매트릭스를 저장하자
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

	//해당 모델에 존자해는 총 애니메이션의 갯수
	m_iNumAnimationClip = m_pScene->mNumAnimations;

	//만큼 모든 애니메이션을 순회하면서
	for (_uint i = 0 ; i < m_iNumAnimationClip; i++)
	{
		//해당 모델에 존재하는 i번째 애니메이션
		aiAnimation* paiAnimation = m_pScene->mAnimations[i];

		//을 가지고 애니메이션 클립을 하나 만든다
		CAnimationClip* pAinmationClip = CAnimationClip::Create(paiAnimation->mName.data, paiAnimation->mDuration, paiAnimation->mTicksPerSecond);
		NULL_CHECK_RETURN(pAinmationClip,E_FAIL);

		//만든 애니메이션 클립에 애니메이션 뼈 개수만큼 공간을 확보하고
		pAinmationClip->Reserve(paiAnimation->mNumChannels);
		

		for (_uint j = 0 ; j < paiAnimation->mNumChannels; j++)
		{
			//해당 애니메이션에서 영향을 받는 j번째 뼈의 정보를 이용해서
			aiNodeAnim*	pAIChannel = paiAnimation->mChannels[j];

			//해당 뼈와 같은 이름의 하이어러키 노드를 찾아서
			CHierarchyNode*		pHierarchyNode = Find_HierarchyNode(pAIChannel->mNodeName.data);
			NULL_CHECK_RETURN(pHierarchyNode, E_FAIL);

			//해당 뼈를 만든다
			CClipBone* pClipBone = CClipBone::Create(pAIChannel->mNodeName.data, pHierarchyNode);
			NULL_CHECK_RETURN(pClipBone,E_FAIL);

			//해당 뼈의 최대 키프래임(해당 뼈가 애니메이션 재생도중 움직여야하는 정보)을 구해서
			_uint		iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
			iNumKeyFrames = max(iNumKeyFrames, pAIChannel->mNumPositionKeys);

			//해당 뼈에게 키프레임 공간을 확보해놓고
			pClipBone->Reserve(iNumKeyFrames);

			//없는 프레임을 이전 프레임으로 채워주기 위한 변수
			//ex 총 5프레임중 3프레임밖에 없는 애니메이션을 위해 이전 프레임을 저장하는 용도
			_float3		vScale;
			_float4		vRotation;
			_float3		vPosition;
			_double		Time;

			//모든 키프레임을 순회하면서
			for (_uint k = 0 ; k < iNumKeyFrames; k++)
			{
				//키프레임을 생성한다
				KEYFRAME*			pKeyFrame = new KEYFRAME;
				ZeroMemory(pKeyFrame, sizeof(KEYFRAME));


				//없는 프레임을 이전 프레임으로 채워주기 위한 조건 검사
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

				//조건 검사를 마친 값들을 생성한 키프레임에 넣어주고
				pKeyFrame->vScale = vScale;
				pKeyFrame->vRotation = vRotation;
				pKeyFrame->vPosition = vPosition;
				pKeyFrame->Time = Time;

				//키프레임을 만들었으면 뼈에 해당 키프레임(움직임)을 넣어준다
				pClipBone->Add_KeyFrame(pKeyFrame);
			}

			//키프레임까지 다 채워진 뼈가 생성되었으면 해당 뼈를 애니메이션 클립에 넣어주고
			pAinmationClip->Add_ClipBone(pClipBone);
		}

		//움직여야할 뼈에 대한 정보를 모두 다 저장한 애니메이션은 에니메이터에 넣어준다.
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

		//해당 모델에 존자해는 총 애니메이션의 갯수
		_uint iNumAnimClip = pScene->mNumAnimations;
		m_iNumAnimationClip += iNumAnimClip;

		//만큼 모든 애니메이션을 순회하면서
		for (_uint i = 0; i < iNumAnimClip; i++)
		{
			//해당 모델에 존재하는 i번째 애니메이션
			aiAnimation* paiAnimation = pScene->mAnimations[i];

			//을 가지고 애니메이션 클립을 하나 만든다
			CAnimationClip* pAinmationClip = CAnimationClip::Create(paiAnimation->mName.data, paiAnimation->mDuration, paiAnimation->mTicksPerSecond);
			NULL_CHECK_RETURN(pAinmationClip, E_FAIL);

			//만든 애니메이션 클립에 애니메이션 뼈 개수만큼 공간을 확보하고
			pAinmationClip->Reserve(paiAnimation->mNumChannels);


			for (_uint j = 0; j < paiAnimation->mNumChannels; j++)
			{
				//해당 애니메이션에서 영향을 받는 j번째 뼈의 정보를 이용해서
				aiNodeAnim*	pAIChannel = paiAnimation->mChannels[j];

				//해당 뼈와 같은 이름의 하이어러키 노드를 찾아서
				CHierarchyNode*		pHierarchyNode = Find_HierarchyNode(pAIChannel->mNodeName.data);
				NULL_CHECK_RETURN(pHierarchyNode, E_FAIL);

				//해당 뼈를 만든다
				CClipBone* pClipBone = CClipBone::Create(pAIChannel->mNodeName.data, pHierarchyNode);
				NULL_CHECK_RETURN(pClipBone, E_FAIL);

				//해당 뼈의 최대 키프래임(해당 뼈가 애니메이션 재생도중 움직여야하는 정보)을 구해서
				_uint		iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
				iNumKeyFrames = max(iNumKeyFrames, pAIChannel->mNumPositionKeys);

				//해당 뼈에게 키프레임 공간을 확보해놓고
				pClipBone->Reserve(iNumKeyFrames);

				//없는 프레임을 이전 프레임으로 채워주기 위한 변수
				//ex 총 5프레임중 3프레임밖에 없는 애니메이션을 위해 이전 프레임을 저장하는 용도
				_float3		vScale;
				_float4		vRotation;
				_float3		vPosition;
				_double		Time;

				//모든 키프레임을 순회하면서
				for (_uint k = 0; k < iNumKeyFrames; k++)
				{
					//키프레임을 생성한다
					KEYFRAME*			pKeyFrame = new KEYFRAME;
					ZeroMemory(pKeyFrame, sizeof(KEYFRAME));


					//없는 프레임을 이전 프레임으로 채워주기 위한 조건 검사
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

					//조건 검사를 마친 값들을 생성한 키프레임에 넣어주고
					pKeyFrame->vScale = vScale;
					pKeyFrame->vRotation = vRotation;
					pKeyFrame->vPosition = vPosition;
					pKeyFrame->Time = Time;

					//키프레임을 만들었으면 뼈에 해당 키프레임(움직임)을 넣어준다
					pClipBone->Add_KeyFrame(pKeyFrame);
				}

				//키프레임까지 다 채워진 뼈가 생성되었으면 해당 뼈를 애니메이션 클립에 넣어주고
				pAinmationClip->Add_ClipBone(pClipBone);
			}

			//움직여야할 뼈에 대한 정보를 모두 다 저장한 애니메이션은 에니메이터에 넣어준다.
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
