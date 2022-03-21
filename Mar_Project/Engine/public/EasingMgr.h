#pragma once
#include "Base.h"

BEGIN(Engine)

class CEasingMgr :public CBase
{
	DECLARE_SINGLETON(CEasingMgr);

private:
	explicit CEasingMgr();
	virtual ~CEasingMgr() = default;

public:
	_float	Easing(_uint eEasingType,_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);


private:
	_float Linear(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	_float QuadIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	_float QuadOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	_float QuadInOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	_float CubicIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	_float CubicOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	_float CubicInOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	_float QuarticIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	_float QuarticOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	_float QuarticInOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	_float QuinticIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	_float QuinticOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	_float QuinticInOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	_float SinIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	_float sinfOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	_float sinfInOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	_float ExpoIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	_float ExpoOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	_float ExpoInOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	_float CircularIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	_float CircularOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	_float CircularInOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	_float ElasticEaseIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	_float ElasticEaseOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	_float ElasticEaseInOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	_float BounceEaseIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	_float BounceEaseOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);



public:
	virtual void Free() override;

};

END

