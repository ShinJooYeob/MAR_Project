#include "stdafx.h"
#include "..\public\UtilityMgr.h"
#include "ParticleObject.h"
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

_float3 CUtilityMgr::RandomFloat3(_float3 Min, _float3 Max)
{
	if (Min.x >= Max.x) // bad input
	{
		_float temp = Min.x;
		Min.x = Max.x;
		Max.x = temp;
	}
	if (Min.y >= Max.y) // bad input
	{
		_float temp = Min.y;
		Min.y = Max.y;
		Max.y = temp;
	}

	if (Min.z >= Max.z) // bad input
	{
		_float temp = Min.z;
		Min.z = Max.z;
		Max.z = temp;
	}

	return _float3(RandomFloat(Min.x, Max.x), RandomFloat(Min.y, Max.y), RandomFloat(Min.z, Max.z));
}

void CUtilityMgr::SlowMotionStart(_float fTargetTime, _float TargetSpeed)
{
	NULL_CHECK_BREAK(m_pMainApp);
	m_pMainApp->SlowMotionStart(fTargetTime, TargetSpeed);
}

HRESULT CUtilityMgr::Create_ParticleObject(_uint eSceneID, PARTICLEDESC tParticleDesc)
{

	switch (tParticleDesc.eParticleTypeID)
	{
	case Client::Particle_Straight:
		FAILED_CHECK(GetSingle(CGameInstance)->Add_GameObject_To_Layer(eSceneID, TAG_LAY(Layer_Particle), TEXT("ProtoType_GameObject_Object_particle_Straight"), &tParticleDesc));
		break;

	case Client::Particle_Ball:
		FAILED_CHECK(GetSingle(CGameInstance)->Add_GameObject_To_Layer(eSceneID, TAG_LAY(Layer_Particle), TEXT("ProtoType_GameObject_Object_particle_Ball"), &tParticleDesc));
		break;

	case Client::Particle_Fixed:
		FAILED_CHECK(GetSingle(CGameInstance)->Add_GameObject_To_Layer(eSceneID, TAG_LAY(Layer_Particle), TEXT("ProtoType_GameObject_Object_particle_Fixed"), &tParticleDesc));
		break;

	case Client::Particle_Suck:
		FAILED_CHECK(GetSingle(CGameInstance)->Add_GameObject_To_Layer(eSceneID, TAG_LAY(Layer_Particle), TEXT("ProtoType_GameObject_Object_particle_Suck"), &tParticleDesc));
		break;

		/*
		case Client::Particle_Cone:
		FAILED_CHECK(GetSingle(CGameInstance)->Add_GameObject_To_Layer(eSceneID, TEXT("Layer_Particle"), TEXT("ProtoType_GameObject_Object_particle_Cone"), &tParticleDesc));
		break;

		case Client::Particle_Fountain:
		FAILED_CHECK(GetSingle(CGameInstance)->Add_GameObject_To_Layer(eSceneID, TEXT("Layer_Particle"), TEXT("ProtoType_GameObject_Object_particle_Fountain"), &tParticleDesc));
		break;

		case Client::Particle_Spread:
		FAILED_CHECK(GetSingle(CGameInstance)->Add_GameObject_To_Layer(eSceneID, TEXT("Layer_Particle"), TEXT("ProtoType_GameObject_Object_particle_Spread"), &tParticleDesc));
		break;
		*/
	default:
		return E_FAIL;
		break;
	}
	return S_OK;
}

HRESULT CUtilityMgr::Clear_RenderGroup_forSceneChange()
{
	NULL_CHECK_RETURN(m_pRenderer, E_FAIL);
	g_pGameInstance->Clear_CollisionGroup();
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
