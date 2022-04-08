#pragma once



#include "Base.h"


//특정 애니메이션 클립에서 사용되는 뼈
BEGIN(Engine)

class CClipBone final : public CBase
{
private:
	CClipBone();
	virtual ~CClipBone() = default;

public:
	void Reserve(_uint iNumKeyFrames) { m_KeyFrames.reserve(iNumKeyFrames);	m_iNumKeyFrames = iNumKeyFrames; };
	void Add_KeyFrame(KEYFRAME* pKeyFrame) { m_KeyFrames.push_back(pKeyFrame); };

public:
	HRESULT Initialize_ClipBone(const char* pClipBoneName);


private:
	string						m_szClipBoneName = "";

	_uint						m_iNumKeyFrames = 0;
	vector<KEYFRAME*>			m_KeyFrames;
	typedef vector<KEYFRAME*>	KEYFRAME;

public:
	static CClipBone* Create(const char* pClipBoneName);
	virtual void Free() override;
};


END