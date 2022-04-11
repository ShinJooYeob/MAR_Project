#include "..\public\ClipBone.h"
#include "HierarchyNode.h"



CClipBone::CClipBone()
{
}

void CClipBone::Set_CurrentKeyFrame(_uint iKeyFrameIndex)
{
	if (iKeyFrameIndex >= m_vecKeyFrames.size()) __debugbreak();

	m_iCurrentKeyFrame = iKeyFrameIndex;

}

void CClipBone::Set_TransformationMatrix_ToHierarchyNode(_fMatrix TransformationMatrix)
{
	NULL_CHECK_BREAK(m_pHierarchyNode);

	m_pHierarchyNode->Set_Transformation(TransformationMatrix);

}

HRESULT CClipBone::Initialize_ClipBone(const char * pClipBoneName, CHierarchyNode* pHierarchyNode)
{
	m_szClipBoneName = pClipBoneName;
	m_pHierarchyNode = pHierarchyNode;
	Safe_AddRef(m_pHierarchyNode);
	return S_OK;
}

CClipBone * CClipBone::Create(const char * pClipBoneName, CHierarchyNode* pHierarchyNode)
{
	CClipBone*	pInstance = new CClipBone();

	if (FAILED(pInstance->Initialize_ClipBone(pClipBoneName, pHierarchyNode)))
	{
		MSGBOX("Failed to Created CClipBone");
		Safe_Release(pInstance);
	}
	return pInstance;;
}

void CClipBone::Free()
{
	for (auto& pKeyFrame : m_vecKeyFrames)
		Safe_Delete(pKeyFrame);

	m_vecKeyFrames.clear();

	Safe_Release(m_pHierarchyNode);
}

