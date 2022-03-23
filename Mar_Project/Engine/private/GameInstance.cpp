#include "..\Public\GameInstance.h"
#include "TimeMgr.h"
#include "SceneMgr.h"
#include "ThreadMgr.h"
#include "ObjectMgr.h"
#include "GameObject.h"
#include "EasingMgr.h"
#include "FrustumMgr.h"
#include "SoundMgr.h"

IMPLEMENT_SINGLETON(CGameInstance);


CGameInstance::CGameInstance()
	:m_pThreadMgr(GetSingle(CThreadMgr)), 
	m_pTimerMgr(GetSingle(CTimeMgr)),
	m_pGraphicDevice(GetSingle(CGraphic_Device)),
	m_pSceneMgr(GetSingle(CSceneMgr)),
	m_pObjectMgr(GetSingle(CObjectMgr)),
	m_pComponenetMgr(GetSingle(CComponentMgr)),
	m_pInputDevice(GetSingle(CInput_Device)),
	m_pEasingMgr(GetSingle(CEasingMgr)),
	m_pFrustumMgr(GetSingle(CFrustumMgr)),
	m_pSoundMgr(GetSingle(CSoundMgr))

{
	m_pThreadMgr->AddRef();
	m_pTimerMgr->AddRef();
	m_pGraphicDevice->AddRef();
	m_pSceneMgr->AddRef();
	m_pObjectMgr->AddRef();
	m_pComponenetMgr->AddRef();
	m_pInputDevice->AddRef();
	m_pEasingMgr->AddRef();
	m_pFrustumMgr->AddRef();
	m_pSoundMgr->AddRef();
	
}


HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, const CGraphic_Device::GRAPHICDESC & GraphicDesc, _uint iMaxSceneNum, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut,
	ID3D11RenderTargetView** ppBackBufferRTV, ID3D11DepthStencilView** ppDepthStencilView, IDXGISwapChain**	ppSwapChain, _double fDoubleInterver)
{
	if (m_pGraphicDevice == nullptr || m_pObjectMgr == nullptr || m_pComponenetMgr == nullptr ||
		m_pSceneMgr == nullptr || m_pFrustumMgr == nullptr || m_pSoundMgr == nullptr)
	{
		__debugbreak();
		return E_FAIL;
	}

	//if (FAILED(m_pSeverMgr->ConnectSever()))
	//{
	//	MSGBOX("Failed to Connecting Sever");
	//}

	FAILED_CHECK(m_pGraphicDevice->Initialize_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinCX, GraphicDesc.iWinCY, ppDeviceOut, ppDeviceContextOut, ppBackBufferRTV, ppDepthStencilView, ppSwapChain));

	FAILED_CHECK(m_pObjectMgr->Reserve_Container(iMaxSceneNum));

	FAILED_CHECK(m_pInputDevice->Ready_Input_Device(hInst, GraphicDesc.hWnd, fDoubleInterver));

	FAILED_CHECK(m_pComponenetMgr->Reserve_Container(iMaxSceneNum));



	FAILED_CHECK(m_pFrustumMgr->Initialize_FrustumMgr(*ppDeviceOut, *ppDeviceContextOut));

	FAILED_CHECK(m_pSoundMgr->Initialize_FMOD());



	return S_OK;
}


_int CGameInstance::Update_Engine(_double fDeltaTime)
{
	if (m_pSceneMgr == nullptr || m_pObjectMgr == nullptr || m_pSoundMgr == nullptr)
		return -1;


	FAILED_CHECK(m_pInputDevice->SetUp_InputDeviceState(fDeltaTime));

	FAILED_CHECK(m_pSoundMgr->Update_FMOD(fDeltaTime));

	if (m_pSceneMgr->Update(fDeltaTime) < 0)
	{
		__debugbreak();
		return -1;
	}

	if (m_pObjectMgr->Update(fDeltaTime) < 0)
	{
		__debugbreak();
		return -1;
	}

	FAILED_CHECK(SetUp_WorldFrustumPlane());


	return 0;
}

_int CGameInstance::LateUpdate_Engine(_double fDeltaTime)
{
	if (m_pObjectMgr->LateUpdate(fDeltaTime) < 0)
	{
		__debugbreak();
		return -1;
	}
	if (m_pSceneMgr->LateUpdate(fDeltaTime) < 0)
	{
		__debugbreak();
		return -1;
	}

	return 0;
}


HRESULT CGameInstance::Clear_Scene_Resource(_uint eSceneNum)
{
	if (m_pObjectMgr == nullptr || m_pComponenetMgr == nullptr)
	{
		__debugbreak();
		return E_FAIL;
	}


	FAILED_CHECK(m_pComponenetMgr->Clear_Scene_Componenets(eSceneNum));

	FAILED_CHECK(m_pObjectMgr->Clear_Scene_GameObjects(eSceneNum));

	return S_OK;
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	NULL_CHECK_BREAK(m_pGraphicDevice);


	return m_pGraphicDevice->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	NULL_CHECK_BREAK(m_pGraphicDevice);


	return m_pGraphicDevice->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	NULL_CHECK_BREAK(m_pGraphicDevice);


	return m_pGraphicDevice->Present();
}

HRESULT CGameInstance::Add_GameObject_Prototype(const _tchar * tagPrototype, CGameObject * pPrototype)
{
	if (m_pObjectMgr == nullptr)
	{
		__debugbreak();
		return E_FAIL;
	}

	return	m_pObjectMgr->Add_GameObject_Prototype(tagPrototype, pPrototype);
}

HRESULT CGameInstance::Add_GameObject_To_Layer(_uint eSceneNum, const _tchar * tagLayer, const _tchar * tagPrototype, void * pArg)
{
	NULL_CHECK_BREAK(m_pObjectMgr);



	return	m_pObjectMgr->Add_GameObject_To_Layer(eSceneNum, tagLayer, tagPrototype, pArg);
}

CComponent* CGameInstance::Get_Commponent_By_LayerIndex(_uint eSceneNum, const _tchar * tagLayer, const _tchar* tagComponet, _uint iLayerIndex)
{
	if (tagComponet == nullptr || tagLayer == nullptr || m_pObjectMgr == nullptr)
	{
		__debugbreak();
		return nullptr;
	}

	return m_pObjectMgr->Get_Commponent_By_LayerIndex(eSceneNum, tagLayer, tagComponet,iLayerIndex);
}

CGameObject * CGameInstance::Get_GameObject_By_LayerIndex(_uint eSceneNum, const _tchar * tagLayer, _uint iLayerIndex)
{
	if (tagLayer == nullptr || m_pObjectMgr == nullptr)
	{
		__debugbreak();
		return nullptr;
	}

	return m_pObjectMgr->Get_GameObject_By_LayerIndex(eSceneNum, tagLayer,iLayerIndex);
}

list<CGameObject*>* CGameInstance::Get_ObjectList_from_Layer(_uint eSceneNum, const _tchar * tagLayer)
{
	if (tagLayer == nullptr || m_pObjectMgr == nullptr)
	{
		__debugbreak();
		return nullptr;
	}

	return m_pObjectMgr->Get_ObjectList_from_Layer(eSceneNum, tagLayer);
}


HRESULT CGameInstance::Delete_GameObject_To_Layer_Index(_uint eSceneNum, const _tchar * tagLayer, _uint index)
{
	if (tagLayer == nullptr || m_pObjectMgr == nullptr)
	{

		__debugbreak();
		return E_FAIL;
	}
	

	return Delete_GameObject_To_Layer_Index(eSceneNum, tagLayer, index);
}

HRESULT CGameInstance::Delete_GameObject_To_Layer_Object(_uint eSceneNum, const _tchar * tagLayer, CGameObject * obj)
{
	if (tagLayer == nullptr || m_pObjectMgr == nullptr)
	{
		__debugbreak();
		return E_FAIL;
	}

	return Delete_GameObject_To_Layer_Object(eSceneNum, tagLayer, obj);
}



_double CGameInstance::Get_DeltaTime(const _tchar * tagTimer)
{
	NULL_CHECK_BREAK(m_pTimerMgr);


	return m_pTimerMgr->Get_DeltaTime(tagTimer);
}

HRESULT CGameInstance::Add_Timer(const _tchar * tagTimer)
{
	NULL_CHECK_BREAK(m_pTimerMgr);

	return m_pTimerMgr->Add_Timer(tagTimer);
}



HRESULT CGameInstance::Scene_Change(CScene * pScene, _int iNextSceneIdx)
{

	if (m_pSceneMgr == nullptr || m_pObjectMgr == nullptr)
	{
		__debugbreak();
		return E_FAIL;
	}

	if(FAILED(m_pSceneMgr->Scene_Chage(pScene, iNextSceneIdx)))
		return E_FAIL;

	if(FAILED(m_pObjectMgr->Reset_StaticObject_NowSceneNum(iNextSceneIdx)))
		return E_FAIL;

	return S_OK;
}

_int CGameInstance::Render_Scene()
{
	NULL_CHECK_BREAK(m_pSceneMgr);

	if (m_pSceneMgr->Render() < 0)
	{
		__debugbreak();
		return -1;
	}

	if (m_pSceneMgr->LateRender() < 0)
	{
		__debugbreak();
		return -1;
	}

	return 0;
}

_uint CGameInstance::Get_NowSceneNum()
{
	NULL_CHECK_BREAK(m_pSceneMgr);

	return m_pSceneMgr->Get_NowSceneNum();
}


CScene * CGameInstance::Get_NowScene()
{
	NULL_CHECK_BREAK(m_pSceneMgr);

	return m_pSceneMgr->Get_NowScene();
}

HRESULT CGameInstance::Set_SceneChanging_to_Scene(_uint _INextScene)
{
	NULL_CHECK_BREAK(m_pSceneMgr);

	return m_pSceneMgr->Set_SceneChanging_to_Scene(_INextScene);
}

HRESULT CGameInstance::Add_Component_Prototype(_uint eSceneIdx, const _tchar * tagPrototypeComponent, CComponent * pComponenet)
{
	NULL_CHECK_BREAK(m_pComponenetMgr);


	return m_pComponenetMgr->Add_Component_Prototype(eSceneIdx, tagPrototypeComponent,pComponenet);
}

CComponent * CGameInstance::Clone_Component(_uint eSceneIdx, const _tchar * tagPrototypeComponent, void * pArg)
{
	NULL_CHECK_BREAK(m_pComponenetMgr);


	return m_pComponenetMgr->Clone_Component(eSceneIdx, tagPrototypeComponent, pArg);
}

HRESULT CGameInstance::PlayThread(void * _ThreadFunc, void * _pArg)
{
	NULL_CHECK_BREAK(m_pThreadMgr);

	return m_pThreadMgr->PlayThread(_ThreadFunc, _pArg);
}

_byte CGameInstance::Get_DIKeyState(_ubyte eKeyID)
{
	NULL_CHECK_BREAK(m_pInputDevice);

	return m_pInputDevice->Get_DIKeyState(eKeyID);
}

_long CGameInstance::Get_DIMouseMoveState(CInput_Device::MOUSEMOVESTATE eMouseMoveState)
{
	NULL_CHECK_BREAK(m_pInputDevice);


	return m_pInputDevice->Get_DIMouseMoveState(eMouseMoveState);
}

_byte CGameInstance::Get_DIMouseButtonState(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState)
{
	NULL_CHECK_BREAK(m_pInputDevice);

	return m_pInputDevice->Get_DIMouseButtonState(eMouseButtonState);
}

_float CGameInstance::Easing(_uint eEasingType, _float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	NULL_CHECK_BREAK(m_pEasingMgr);

	return m_pEasingMgr->Easing(eEasingType, fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
}

_float3 CGameInstance::Easing_Vector(_uint eEasingType, _float3 fStartPoint, _float3 fTargetPoint, _float fPassedTime, _float fTotalTime)
{

	NULL_CHECK_BREAK(m_pEasingMgr);

	return m_pEasingMgr->Easing_Vector(eEasingType, fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
}

HRESULT CGameInstance::SetUp_WorldFrustumPlane()
{
	NULL_CHECK_BREAK(m_pFrustumMgr);

	return m_pFrustumMgr->SetUp_WorldFrustumPlane();
}

_bool CGameInstance::IsNeedToRender(_float3 vWorldPosition, _float fLenth)
{
	NULL_CHECK_MSG(m_pFrustumMgr,L"Not Have FrustumMgr");

	return m_pFrustumMgr->IsNeedToRender(vWorldPosition, fLenth);
}

_int CGameInstance::Channel_VolumeUp(CHANNELID eID, _float _vol)
{

	NULL_CHECK_MSG(m_pSoundMgr, L"Not Have m_pSoundMgr");
	return m_pSoundMgr->Channel_VolumeUp(eID, _vol);
}

_int CGameInstance::Channel_VolumeDown(CHANNELID eID, _float _vol)
{
	NULL_CHECK_MSG(m_pSoundMgr, L"Not Have m_pSoundMgr");

	return m_pSoundMgr->Channel_VolumeDown(eID,_vol);
}

_int CGameInstance::Channel_Pause(CHANNELID eID)
{
	NULL_CHECK_MSG(m_pSoundMgr, L"Not Have m_pSoundMgr");

	return m_pSoundMgr->Channel_Pause(eID);
}

HRESULT CGameInstance::PlaySound(TCHAR * pSoundKey, CHANNELID eID ,_float fLouderMultiple)
{
	NULL_CHECK_MSG(m_pSoundMgr, L"Not Have m_pSoundMgr");

	return m_pSoundMgr->PlaySound(pSoundKey, eID, fLouderMultiple);
}

HRESULT CGameInstance::PlayBGM(TCHAR * pSoundKey, _float fLouderMultiple)
{
	NULL_CHECK_MSG(m_pSoundMgr, L"Not Have m_pSoundMgr");

	return m_pSoundMgr->PlayBGM(pSoundKey, fLouderMultiple);
}

void CGameInstance::Stop_ChannelSound(CHANNELID eID)
{
	NULL_CHECK_MSG(m_pSoundMgr, L"Not Have m_pSoundMgr");

	m_pSoundMgr->Stop_ChannelSound(eID);

	return;
}

void CGameInstance::Stop_AllChannel()
{
	NULL_CHECK_MSG(m_pSoundMgr, L"Not Have m_pSoundMgr");
	m_pSoundMgr->Stop_AllChannel();
	return;
}

_float CGameInstance::Get_Channel_Volume(CHANNELID eID)
{
	NULL_CHECK_MSG(m_pSoundMgr, L"Not Have m_pSoundMgr");
	return 	m_pSoundMgr->Get_Channel_Volume(eID);
}

_bool CGameInstance::Get_Channel_IsPaused(CHANNELID eID)
{
	NULL_CHECK_MSG(m_pSoundMgr, L"Not Have m_pSoundMgr");
	return 	m_pSoundMgr->Get_Channel_IsPaused(eID);
}




void CGameInstance::Release_Engine()
{


	if (0 != GetSingle(CGameInstance)->DestroyInstance())
		MSGBOX("Failed to Release Com GameInstance");

	if (0 != GetSingle(CThreadMgr)->DestroyInstance())
		MSGBOX("Failed to Release Com ThreadMgr ");

	if (0 != GetSingle(CEasingMgr)->DestroyInstance())
		MSGBOX("Failed to Release Com EasingMgr ");

	if (0 != GetSingle(CObjectMgr)->DestroyInstance())
		MSGBOX("Failed to Release Com ObjectMgr ");

	if (0 != GetSingle(CSceneMgr)->DestroyInstance())
		MSGBOX("Failed to Release Com SceneMgr ");

	if (0 != GetSingle(CComponentMgr)->DestroyInstance())
		MSGBOX("Failed to Release Com ComponentMgr");

	if (0 != GetSingle(CFrustumMgr)->DestroyInstance())
		MSGBOX("Failed to Release Com FrustumMgr ");

	if (0 != GetSingle(CInput_Device)->DestroyInstance())
		MSGBOX("Failed to Release Com CInput_Device ");

	if (0 != GetSingle(CTimeMgr)->DestroyInstance())
		MSGBOX("Failed to Release Com TimeMgr ");

	if (0 != GetSingle(CSoundMgr)->DestroyInstance())
		MSGBOX("Failed to Release Com CSoundMgr ");
	

	if (0 != GetSingle(CGraphic_Device)->DestroyInstance())
		MSGBOX("Failed to Release Com Graphic_Device ");

}

void CGameInstance::Free()
{
	Safe_Release(m_pSoundMgr);
	Safe_Release(m_pThreadMgr);
	Safe_Release(m_pObjectMgr);
	Safe_Release(m_pSceneMgr);
	Safe_Release(m_pComponenetMgr);
	Safe_Release(m_pGraphicDevice);
	Safe_Release(m_pInputDevice);
	Safe_Release(m_pTimerMgr);
	Safe_Release(m_pEasingMgr);
	Safe_Release(m_pFrustumMgr);
	
}
