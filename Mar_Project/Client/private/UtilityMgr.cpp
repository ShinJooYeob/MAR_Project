#include "stdafx.h"
#include "..\public\UtilityMgr.h"
#include "ParticleObject.h"
#include "MainApp.h"
#include "Camera_Main.h"
#include "FadeEffect.h"

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

	FAILED_CHECK(g_pGameInstance->Add_Component_Prototype(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Texture_ScreenEffectUI),
		CTexture::Create(m_pDevice, m_pDeviceContext, L"ScreenEffectUI.txt")));

	FAILED_CHECK(g_pGameInstance->Add_GameObject_Prototype(TAG_OP(Prototype_ScreenEffectUI), CFadeEffect::Create(m_pDevice, m_pDeviceContext)));
	FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENEID::SCENE_STATIC, TAG_LAY(Layer_ScreenEffect), TAG_OP(Prototype_ScreenEffectUI)));
	m_pFadeEffect = (CFadeEffect*)g_pGameInstance->Get_GameObject_By_LayerLastIndex(SCENEID::SCENE_STATIC, TAG_LAY(Layer_ScreenEffect));

	NULL_CHECK_RETURN(m_pFadeEffect, E_FAIL);

	FAILED_CHECK(Ready_InstanceParticleDesc());

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
	_uint iDigit = CountDigit(RandNum);
	while (!iDigit)
	{
		RandNum = rand();
		iDigit = CountDigit(RandNum);
	}
	_uint Power = _uint(pow(10, iDigit - 1));
	//_float f = (RandNum) / _float(Power);
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

	case Client::Particle_Cone:
		FAILED_CHECK(GetSingle(CGameInstance)->Add_GameObject_To_Layer(eSceneID, TAG_LAY(Layer_Particle), TEXT("ProtoType_GameObject_Object_particle_Cone"), &tParticleDesc));
		break;

	case Client::Particle_Fixed_LookFree:
		FAILED_CHECK(GetSingle(CGameInstance)->Add_GameObject_To_Layer(eSceneID, TAG_LAY(Layer_Particle), TEXT("ProtoType_GameObject_Object_particle_Fixed_LookFree"), &tParticleDesc));
		break;
	case Client::Particle_Spread:
		FAILED_CHECK(GetSingle(CGameInstance)->Add_GameObject_To_Layer(eSceneID, TAG_LAY(Layer_Particle), TEXT("ProtoType_GameObject_Object_particle_Spread"), &tParticleDesc));
		break;


		
		/*
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

_Vector CUtilityMgr::ReflectVector(_Vector vDir, _Vector vVerticleVector)
{
	_Vector vVerticVec = XMVector3Normalize(vVerticleVector);
	vVerticVec = vVerticVec  * XMVector3Dot(-vDir, vVerticVec);

	return vDir + (vVerticVec * 2.f);
}

_Vector CUtilityMgr::SlideVector(_Vector vDir, _Vector vVerticleVector)
{
	_Vector vVerticVec = XMVector3Normalize(vVerticleVector);
	vVerticVec = vVerticVec  * XMVector3Dot(-vDir, vVerticVec);

	return vDir+ vVerticVec;
}

HRESULT CUtilityMgr::Start_ScreenEffect(ScreenEffectID eEffectType, _double EffectDuration, _float4 AdditionalParameter)
{
	switch (eEffectType)
	{
	case Client::CUtilityMgr::ScreenEffect_FadeIn:
	{
		if (m_pFadeEffect->Start_FadeEffect(CFadeEffect::FadeID_FadeIn, EffectDuration, AdditionalParameter))
		m_pFadeEffect->Chage_TextureIndex();
	}
	break;
	case Client::CUtilityMgr::ScreenEffect_FadeOut:
	{
		if (m_pFadeEffect->Start_FadeEffect(CFadeEffect::FadeID_FadeOut, EffectDuration, AdditionalParameter))
		m_pFadeEffect->Chage_TextureIndex();
	}
	break;
	case Client::CUtilityMgr::ScreenEffect_FadeInOut:
	{
		if (m_pFadeEffect->Start_FadeEffect(CFadeEffect::FadeID_FadeInOut, EffectDuration, AdditionalParameter))
		m_pFadeEffect->Chage_TextureIndex();
	}
	break;
	case Client::CUtilityMgr::ScreenEffect_FadeOutIn:
	{
		if (m_pFadeEffect->Start_FadeEffect(CFadeEffect::FadeID_FadeOutIn, EffectDuration, AdditionalParameter))
			m_pFadeEffect->Chage_TextureIndex();
		
	}
	break;
	case Client::CUtilityMgr::ScreenEffect_CamShaking:
	{
		CCamera_Main* pMainCam = (CCamera_Main*)g_pGameInstance->Get_GameObject_By_LayerLastIndex(SCENE_STATIC, TAG_LAY(Layer_Camera_Main));
		NULL_CHECK_RETURN(pMainCam, E_FAIL);
		pMainCam->Start_CameraShaking_Thread(EffectDuration, AdditionalParameter.x);
	}
	break;
	case Client::CUtilityMgr::ScreenEffect_HitEffect:
	{ 
		if (m_pFadeEffect->Start_FadeEffect(CFadeEffect::FadeID_FadeOutIn, EffectDuration * 2, AdditionalParameter)) 
			m_pFadeEffect->Chage_TextureIndex(rand() % 5 + 1);
		

		CCamera_Main* pMainCam = (CCamera_Main*)g_pGameInstance->Get_GameObject_By_LayerLastIndex(SCENE_STATIC, TAG_LAY(Layer_Camera_Main));
		NULL_CHECK_RETURN(pMainCam, E_FAIL);
		pMainCam->Start_CameraShaking_Thread(EffectDuration, 0.2f);
	}
	break;
	default:
		break;
	}


	return S_OK;
}

HRESULT CUtilityMgr::Start_InstanceParticle(_uint eNowSceneNum, _float3 vPosition, _uint iParticleDescIndex)
{
	if (iParticleDescIndex >= m_vecInstanceParticleDesc.size())return E_FAIL;

	m_vecInstanceParticleDesc[iParticleDescIndex].vWorldPosition = vPosition;

	return g_pGameInstance->Add_GameObject_To_Layer(eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_Instance_Particle_Ball),&m_vecInstanceParticleDesc[iParticleDescIndex]);;
}



HRESULT CUtilityMgr::Clear_RenderGroup_forSceneChange()
{
	NULL_CHECK_RETURN(m_pRenderer, E_FAIL);
	g_pGameInstance->Clear_CollisionGroup();
	FAILED_CHECK(m_pRenderer->Clear_RenderGroup_forSceneChaging());
	m_pFadeEffect->Add_RenderGroup_ForSceneChanging();
	return  S_OK;
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

HRESULT CUtilityMgr::Ready_InstanceParticleDesc()
{
	m_vecInstanceParticleDesc.reserve(4);

	INSTPARTICLEDESC tDesc;

	tDesc.ColorChangeFrequency = 1;
	tDesc.vStartColor = _float4(1, 0.64313725f, 0.141176470f, 1);
	//tDesc.vTargetColor = _float4(0.7333333f, 0.31372549f, 0.f, 0.2f);
	tDesc.vTargetColor = _float4(1,1,1, 0.2f);

	tDesc.SizeChangingEndRate = 0.7f;
	tDesc.vStartSize = _float3(0.01f, 0.2f, 1);
	tDesc.vTargetSize = _float3(0.01f);
	m_vecInstanceParticleDesc.push_back(tDesc);



	
	tDesc.ColorChangeFrequency = 4;
	tDesc.vStartColor = _float4(0, 0.066666f, 0.72156862f, 1);
	//tDesc.vTargetColor = _float4(0.7333333f, 0.31372549f, 0.f, 0.2f);
	tDesc.vTargetColor = _float4(1, 1, 1, 0.2f);

	tDesc.SizeChangingEndRate = 0.7f;
	tDesc.vStartSize = _float3(0.01f, 0.2f, 1);
	tDesc.vTargetSize = _float3(0.01f);
	m_vecInstanceParticleDesc.push_back(tDesc);



	tDesc.ColorChangeFrequency = 1;
	tDesc.vStartColor = _float4(1, 0.64313725f, 0.141176470f, 1);
	tDesc.vTargetColor = _float4(1, 1, 1, 0.2f);

	tDesc.SizeChangingEndRate = 0.7f;
	tDesc.vStartSize = _float3(0.015f, 2.f, 1);
	tDesc.vTargetSize = _float3(0.015f);
	m_vecInstanceParticleDesc.push_back(tDesc);

	tDesc.ColorChangeFrequency = 4;
	tDesc.vStartColor = _float4(0.72156862f, 0.066666f,0 , 1);
	tDesc.vTargetColor = _float4(1, 1, 1, 0.2f);
	m_vecInstanceParticleDesc.push_back(tDesc);



	return S_OK;
}

void CUtilityMgr::Free()
{
	Safe_Release(m_pRenderer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
