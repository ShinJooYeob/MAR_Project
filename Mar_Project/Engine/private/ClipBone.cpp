#include "..\public\ClipBone.h"



CClipBone::CClipBone()
{
}

HRESULT CClipBone::Initialize_ClipBone(const char * pClipBoneName)
{
	m_szClipBoneName = pClipBoneName;
	return S_OK;
}

CClipBone * CClipBone::Create(const char * pClipBoneName)
{
	CClipBone*	pInstance = new CClipBone();

	if (FAILED(pInstance->Initialize_ClipBone(pClipBoneName)))
	{
		MSGBOX("Failed to Created CClipBone");
		Safe_Release(pInstance);
	}
	return pInstance;;
}

void CClipBone::Free()
{
	for (auto& pKeyFrame : m_KeyFrames)
		Safe_Delete(pKeyFrame);

	m_KeyFrames.clear();
}

