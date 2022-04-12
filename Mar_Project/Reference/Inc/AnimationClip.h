#pragma once


#include "Base.h"

BEGIN(Engine)

//하나의 애니메이션
class CAnimationClip final : public CBase
{
private:
	CAnimationClip();
	virtual ~CAnimationClip() = default;


public:
	void Reserve(_uint iNumClipBones) { m_vecClipBones.reserve(iNumClipBones);	m_iNumClipBones = iNumClipBones; }
	void Add_ClipBone(class CClipBone* pClipBone) { m_vecClipBones.push_back(pClipBone); };

public:
	HRESULT Initialize_AnimationClip(const char* pName, _double Duration, _double UpdatePerSecond);
	HRESULT Update_TransformMatrices_byClipBones(_bool* bIsFinished,_double TimeDelta);


private:
	string		m_szAnimationClipName = "";
	_double		m_UpdatePerSecond = 0.0;
	_uint		m_iNumClipBones = 0;
	_double		m_Duration = 0.0;

	/* 현재 내 애니메이션이 진행 위치. */
	_double		m_PlayTimeAcc = 0.0;
	_bool		m_bIsFinished = false;

	vector<class CClipBone*>			m_vecClipBones;
	typedef vector<class CClipBone*>	CLIPBONES;




public:
	static CAnimationClip* Create(const char* pAnimationClipName, _double Duration, _double UpdatePerSecond);
	virtual void Free() override;

};

END	