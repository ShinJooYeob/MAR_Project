#pragma once

namespace Engine {



	typedef struct tagCameraDesc
	{
		_bool		bIsOrtho = false;
		_float3		vWorldRotAxis = _float3(0, 0, 0);
		_float3		vEye, vAt, vAxisY;
		_float		fFovy, fAspect, fNear, fFar;
		//CTransform::TRANSFORMDESC TransformDesc;
	}CAMERADESC;


	typedef struct tagArg
	{
		void* pArg;
		CRITICAL_SECTION*	CriSec;
		_bool* IsClientQuit;

	}THREADARG;

}
