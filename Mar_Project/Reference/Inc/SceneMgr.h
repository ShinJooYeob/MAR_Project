#pragma once
#include "Base.h"

BEGIN(Engine)
class CScene;


class CSceneMgr :public CBase
{
	DECLARE_SINGLETON(CSceneMgr)

private:
	explicit CSceneMgr();
	virtual ~CSceneMgr() = default;


public:
	HRESULT Scene_Chage(CScene * _pScene, _int iNextSceneIndex);
	_uint	Get_NowSceneNum() {	return (_uint)m_iCurrentSceneIdx;}
public:
	_int Update(_double fDeltaTime);
	_int LateUpdate(_double fDeltaTime);
	_int Render();
	_int LateRender();

public:
	CScene*				Get_NowScene();
	HRESULT				Set_SceneChanging_to_Scene(_uint _INextScene);

private:
	_int				m_iCurrentSceneIdx = 0;

	CScene*				m_pScene = nullptr;
public:
	virtual void Free()override;
};


END