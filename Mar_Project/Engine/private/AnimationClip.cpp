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

HRESULT CAnimationClip::Update_TransformMatrices_byClipBones(_bool* bIsFinished,_double TimeDelta)
{
	/* 현재 내 애니메이션이 진행 정도. */
	m_PlayTimeAcc += m_UpdatePerSecond * TimeDelta;


	if (m_PlayTimeAcc >= m_Duration)
	{
		m_PlayTimeAcc = 0.0;
		for (_uint i = 0 ; i <m_iNumClipBones ;++ i)
			m_vecClipBones[i]->Set_CurrentKeyFrame(0);
		
		*bIsFinished = m_bIsFinished = true;
	}


	//애니메이션 진행정도에 따른 크자이 결과 행렬
	_Vector			vScale, vRotation, vPosition;

	//모든 뼈들을 돌면서
	for (_uint i = 0; i < m_iNumClipBones; ++i)
	{
		//해당 뼈들의 키프레임들을 받아오고
		const vector<KEYFRAME*>*	pvecKeyFrames = m_vecClipBones[i]->Get_KeyFrameContainor();

		NULL_CHECK_RETURN(pvecKeyFrames, E_FAIL);

		_uint iNumKeyFrame = _uint(pvecKeyFrames->size());
		_uint iCurrentKeyFrameIndex = m_vecClipBones[i]->Get_CurrentKeyFrame();

		/* 현재 애니메이션 진행도가 마지막 키프레임의 시간을 넘어가면. */
		if (m_PlayTimeAcc >= (*pvecKeyFrames)[iNumKeyFrame - 1]->Time)
		{
			//마지막 키프레임으로 고정해준다.
			vScale = XMLoadFloat3(&(*pvecKeyFrames)[iNumKeyFrame - 1]->vScale);
			vRotation = XMLoadFloat4(&(*pvecKeyFrames)[iNumKeyFrame - 1]->vRotation);
			vPosition = XMLoadFloat3(&(*pvecKeyFrames)[iNumKeyFrame - 1]->vPosition);
		}

		else
		{
			//만약 진행도가 마지막을 넘지 않았다면 현재 진행도 이후로 얼마나 더 진행됬는지를 확인하여 설정하고
			while (m_PlayTimeAcc >= (*pvecKeyFrames)[iCurrentKeyFrameIndex + 1]->Time)
				m_vecClipBones[i]->Set_CurrentKeyFrame(++iCurrentKeyFrameIndex);
			

			//현재 진행 상황이 다음 프레임과 현재 프레임 사이에 얼마나 진행되어있는지를 파악하고
			_double		Ratio = (m_PlayTimeAcc - (*pvecKeyFrames)[iCurrentKeyFrameIndex]->Time)
				/ ((*pvecKeyFrames)[iCurrentKeyFrameIndex + 1]->Time - (*pvecKeyFrames)[iCurrentKeyFrameIndex]->Time);


			//두 크자이를 받아와서 선형보간을 해준다.
			_Vector		vSourScale, vDestScale;
			_Vector		vSourRotation, vDestRotation;
			_Vector		vSourPosition, vDestPosition;


			vSourScale = XMLoadFloat3(&(*pvecKeyFrames)[iCurrentKeyFrameIndex]->vScale);
			vSourRotation = XMLoadFloat4(&(*pvecKeyFrames)[iCurrentKeyFrameIndex]->vRotation);
			vSourPosition = XMLoadFloat3(&(*pvecKeyFrames)[iCurrentKeyFrameIndex]->vPosition);

			vDestScale = XMLoadFloat3(&(*pvecKeyFrames)[iCurrentKeyFrameIndex + 1]->vScale);
			vDestRotation = XMLoadFloat4(&(*pvecKeyFrames)[iCurrentKeyFrameIndex + 1]->vRotation);
			vDestPosition = XMLoadFloat3(&(*pvecKeyFrames)[iCurrentKeyFrameIndex + 1]->vPosition);


			//보간 할때 주의할 점은 로테이션은 쿼터니언으로 일반적인 러프 보간이 아닌 쿼터니언 슬러프 보간을 해줘야한다.
			vScale = XMVectorLerp(vSourScale, vDestScale, _float(Ratio));
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, _float(Ratio));
			vPosition = XMVectorLerp(vSourPosition, vDestPosition, _float(Ratio));

		}
		//포지션은 w값이 1이여야하니까 채워주고
		vPosition = XMVectorSetW(vPosition, 1.f);

		//마지막 프레임으로 받아왔던, 두키프레임 사이에 보간을 했던 스자이를 만들었으면 이를 통해 행렬을 만들고
		_Matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
		//이 행렬을 클립 본과 매칭되는 하이어럴키 노드의 Transformation에 저장해준다.
		m_vecClipBones[i]->Set_TransformationMatrix_ToHierarchyNode(TransformationMatrix);

	}


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
