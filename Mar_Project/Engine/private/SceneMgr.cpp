#include "..\Public\SceneMgr.h"
#include "Scene.h"
#include "GameInstance.h"



IMPLEMENT_SINGLETON(CSceneMgr)

CSceneMgr::CSceneMgr()
	:m_pScene(nullptr), m_iCurrentSceneIdx(0)
{
}

HRESULT CSceneMgr::Scene_Chage(CScene * _pScene , _int iNextSceneIndex)
{
	if (_pScene == nullptr)
		return E_FAIL;


	if (m_iCurrentSceneIdx && FAILED(GetSingle(CGameInstance)->Clear_Scene_Resource(m_iCurrentSceneIdx)))
		return E_FAIL;
	


	Safe_Release(m_pScene);
	m_pScene = _pScene;
	m_iCurrentSceneIdx = iNextSceneIndex;

	return S_OK;
}

_int CSceneMgr::Update(_double fDeltaTime)
{
	if (m_pScene == nullptr)
		return -1;

	if (m_pScene->Get_bIsNeedToSceneChange())
		return m_pScene->Change_to_NextScene();

	return m_pScene->Update(fDeltaTime);
}

_int CSceneMgr::LateUpdate(_double fDeltaTime)
{
	if (m_pScene == nullptr)
		return -1;

	if (m_pScene->Get_bIsNeedToSceneChange())
		return m_pScene->Change_to_NextScene();

	return m_pScene->LateUpdate(fDeltaTime);
	
}

_int CSceneMgr::Render()
{
	if (m_pScene == nullptr)
		return -1;
	return m_pScene->Render();
}

_int CSceneMgr::LateRender()
{
	if (m_pScene == nullptr)
		return -1;
	return m_pScene->LateRender();
}

CScene * CSceneMgr::Get_NowScene()
{
	return m_pScene;
}

HRESULT CSceneMgr::Set_SceneChanging_to_Scene(_uint _INextScene)
{
	if (m_pScene == nullptr)
		return E_FAIL;

	return m_pScene->Set_SceneChanging(_INextScene);
}


void CSceneMgr::Free()
{
	Safe_Release(m_pScene);
}
