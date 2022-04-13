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

HRESULT CAnimationClip::Update_TransformMatrices_byClipBones(_bool* bIsFinished,_double TimeDelta, _double* PlayTimeAcc, const vector<CHierarchyNode*>* pVecHierarchyNodes,  vector<_uint>* pVecCurrentKeyFrameIndex)
{
	/* ���� �� �ִϸ��̼��� ���� ����. */
	(*PlayTimeAcc) += m_UpdatePerSecond * TimeDelta;
	(*bIsFinished) = false;

	if ((*PlayTimeAcc) >= m_Duration)
	{
		(*PlayTimeAcc) = 0.0;
		
		*bIsFinished =  true;
	}


	//�ִϸ��̼� ���������� ���� ũ���� ��� ���
	_Vector			vScale, vRotation, vPosition;

	//��� ������ ���鼭
	for (_uint i = 0; i < m_iNumClipBones; ++i)
	{

		if (*bIsFinished)
			(*pVecCurrentKeyFrameIndex)[i] = 0;


		//�ش� ������ Ű�����ӵ��� �޾ƿ���
		const vector<KEYFRAME*>*	pvecKeyFrames = m_vecClipBones[i]->Get_KeyFrameContainor();

		NULL_CHECK_RETURN(pvecKeyFrames, E_FAIL);

		_uint iNumKeyFrame = _uint(pvecKeyFrames->size());
		_uint iCurrentKeyFrameIndex = (*pVecCurrentKeyFrameIndex)[i];

		/* ���� �ִϸ��̼� ���൵�� ������ Ű�������� �ð��� �Ѿ��. */
		if ((*PlayTimeAcc) >= (*pvecKeyFrames)[iNumKeyFrame - 1]->Time)
		{
			//������ Ű���������� �������ش�.
			vScale = XMLoadFloat3(&(*pvecKeyFrames)[iNumKeyFrame - 1]->vScale);
			vRotation = XMLoadFloat4(&(*pvecKeyFrames)[iNumKeyFrame - 1]->vRotation);
			vPosition = XMLoadFloat3(&(*pvecKeyFrames)[iNumKeyFrame - 1]->vPosition);
		}

		else
		{
			//���� ���൵�� �������� ���� �ʾҴٸ� ���� ���൵ ���ķ� �󸶳� �� ����������� Ȯ���Ͽ� �����ϰ�
			while ((*PlayTimeAcc) >= (*pvecKeyFrames)[iCurrentKeyFrameIndex + 1]->Time)
				(*pVecCurrentKeyFrameIndex)[i] = iCurrentKeyFrameIndex = iCurrentKeyFrameIndex + 1;
			

			//���� ���� ��Ȳ�� ���� �����Ӱ� ���� ������ ���̿� �󸶳� ����Ǿ��ִ����� �ľ��ϰ�
			_double		Ratio = ((*PlayTimeAcc) - (*pvecKeyFrames)[iCurrentKeyFrameIndex]->Time)
				/ ((*pvecKeyFrames)[iCurrentKeyFrameIndex + 1]->Time - (*pvecKeyFrames)[iCurrentKeyFrameIndex]->Time);


			//�� ũ���̸� �޾ƿͼ� ���������� ���ش�.
			_Vector		vSourScale, vDestScale;
			_Vector		vSourRotation, vDestRotation;
			_Vector		vSourPosition, vDestPosition;


			vSourScale = XMLoadFloat3(&(*pvecKeyFrames)[iCurrentKeyFrameIndex]->vScale);
			vSourRotation = XMLoadFloat4(&(*pvecKeyFrames)[iCurrentKeyFrameIndex]->vRotation);
			vSourPosition = XMLoadFloat3(&(*pvecKeyFrames)[iCurrentKeyFrameIndex]->vPosition);

			vDestScale = XMLoadFloat3(&(*pvecKeyFrames)[iCurrentKeyFrameIndex + 1]->vScale);
			vDestRotation = XMLoadFloat4(&(*pvecKeyFrames)[iCurrentKeyFrameIndex + 1]->vRotation);
			vDestPosition = XMLoadFloat3(&(*pvecKeyFrames)[iCurrentKeyFrameIndex + 1]->vPosition);


			//���� �Ҷ� ������ ���� �����̼��� ���ʹϾ����� �Ϲ����� ���� ������ �ƴ� ���ʹϾ� ������ ������ ������Ѵ�.
			vScale = XMVectorLerp(vSourScale, vDestScale, _float(Ratio));
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, _float(Ratio));
			vPosition = XMVectorLerp(vSourPosition, vDestPosition, _float(Ratio));

		}
		//�������� w���� 1�̿����ϴϱ� ä���ְ�
		vPosition = XMVectorSetW(vPosition, 1.f);

		//������ ���������� �޾ƿԴ�, ��Ű������ ���̿� ������ �ߴ� �����̸� ��������� �̸� ���� ����� �����
		_Matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
		//�� ����� Ŭ�� ���� ��Ī�Ǵ� ���̾Ű ����� Transformation�� �������ش�.
		m_vecClipBones[i]->Set_TransformationMatrix_ToHierarchyNode(TransformationMatrix, pVecHierarchyNodes);

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
