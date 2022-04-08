#include "..\public\AnimationClip.h"
#include "ClipBone.h"

CAnimationClip::CAnimationClip()
{
}

HRESULT CAnimationClip::Initialize_AnimationClip(const char * pName, _double Duration, _double UpdatePerSecond)
{

	m_szAnimationClipName = pName;

	m_Duration = Duration;
	m_UpdatePerSecond = UpdatePerSecond;

	return S_OK;
}


CAnimationClip * CAnimationClip::Create(const char * pAnimationClipName, _double Duration, _double UpdatePerSecond)
{
	CAnimationClip*	pInstance = new CAnimationClip();

	if (FAILED(pInstance->Initialize_AnimationClip(pAnimationClipName, Duration, UpdatePerSecond)))
	{
		MSGBOX("Failed to Created CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimationClip::Free()
{

	for (auto& pClipBone : m_vecClipBones)
		Safe_Release(pClipBone);

	m_vecClipBones.clear();
	
}
