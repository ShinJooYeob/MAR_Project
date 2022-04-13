#include "stdafx.h"
#include "..\public\UtilityMgr.h"
#include "MainApp.h"

IMPLEMENT_SINGLETON(CUtilityMgr);

CUtilityMgr::CUtilityMgr()
{
}

HRESULT CUtilityMgr::Initialize_UtilityMgr(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, CMainApp * pMainApp)
{
	if (nullptr == pMainApp || nullptr == pDevice || nullptr == pDeviceContext)
		return E_FAIL;

	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;
	m_pMainApp = pMainApp;


	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	return S_OK;
}

_float CUtilityMgr::RandomFloat(_float Min, _float Max)
{
	if (Min >= Max) // bad input
	{
		_float temp = Min;
		Min = Max;
		Max = temp;
	}


	_uint RandNum = rand();
	_uint Power = _uint(pow(10, CountDigit(RandNum)));
	_float f = (RandNum % Power) / _float(Power);
	return (f * (Max - Min)) + Min;
}

_float3 CUtilityMgr::RandomFloat3(_float Min, _float Max)
{
	if (Min >= Max) // bad input
	{
		_float temp = Min;
		Min = Max;
		Max = temp;
	}
	return _float3(RandomFloat(Min,Max), RandomFloat(Min, Max), RandomFloat(Min, Max));
}

void CUtilityMgr::SlowMotionStart(_float fTargetTime, _float TargetSpeed)
{
	NULL_CHECK_BREAK(m_pMainApp);
	m_pMainApp->SlowMotionStart(fTargetTime, TargetSpeed);
}

HRESULT CUtilityMgr::Clear_RenderGroup_forSceneChange()
{
	NULL_CHECK_RETURN(m_pRenderer, E_FAIL);
	
	return m_pRenderer->Clear_RenderGroup_forSceneChaging();
}

void CUtilityMgr::Set_Renderer(CRenderer * pRenderer)
{
	m_pRenderer = pRenderer;
	Safe_AddRef(m_pRenderer);
}

_uint CUtilityMgr::CountDigit(_uint iNum)
{
	string tmp;
	tmp = to_string(iNum);
	return _uint(tmp.size());
}

void CUtilityMgr::Free()
{
	Safe_Release(m_pRenderer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
