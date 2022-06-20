#include "stdafx.h"
#include "..\public\RoseObj.h"
#include "Player.h"
#include "Terrain.h"


CRoseObj::CRoseObj(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CRoseObj::CRoseObj(const CRoseObj & rhs)
	: CMapObject(rhs)
{
}

HRESULT CRoseObj::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CRoseObj::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
	{
		_float3 vPos = *(_float3*)pArg;
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, vPos);
		m_pTransformCom->Scaled_All(_float3(0.5f));
	}



	CUtilityMgr* pUtilMgr = GetSingle(CUtilityMgr);
	m_vSpoutDir = (pUtilMgr->RandomFloat3(-10000.f, 10000.f)).Get_Nomalize();
	m_fRandPower = (pUtilMgr->RandomFloat(1, 5.f));

	Add_JumpPower(pUtilMgr->RandomFloat(3, 6.f));
	return S_OK;
}

_int CRoseObj::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;




	//static float testFloat = 0.25;


	//if (g_pGameInstance->Get_DIKeyState(DIK_UP) & DIS_Down)
	//{
	//	testFloat += 0.01f;
	//	m_pTransformCom->Set_Pivot(_float3(0, testFloat, 0));


	//	string ttszLog = "Monster Speed: " + to_string(testFloat) + "\n";
	//	wstring ttDebugLog;
	//	ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

	//	OutputDebugStringW(ttDebugLog.c_str());
	//}
	//else if (g_pGameInstance->Get_DIKeyState(DIK_DOWN) & DIS_Down)
	//{
	//	testFloat -= 0.01f;
	//	m_pTransformCom->Set_Pivot(_float3(0, testFloat, 0));

	//	string ttszLog = "Monster Speed: " + to_string(testFloat) + "\n";
	//	wstring ttDebugLog;
	//	ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

	//	OutputDebugStringW(ttDebugLog.c_str());

	//}







	m_fStartTimer += _float(fDeltaTime);

	if (!m_bIsSpout)
	{
		_float fGravity = 0;
		if (m_fJumpPower > 0)
		{
			m_fJumpPower = _float(m_fMaxJumpPower * (m_fStartTimer - 1.f)* (m_fStartTimer - 1.f));
			fGravity = m_fJumpPower;

			if (m_fJumpPower <= 2.0f)
			{
				m_fJumpPower = 0;
				m_fStartTimer = _float(fDeltaTime);
			}
		}
		else
		{
			fGravity = _float((m_fStartTimer) * (m_fStartTimer) * -49.0f);
		}

		m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);
		m_pTransformCom->MovetoDir_bySpeed(m_vSpoutDir.XMVector(), m_fRandPower, fDeltaTime);

		CGameInstance* pInstance = GetSingle(CGameInstance);

		CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

		_bool bIsOn = false;
		_uint eNowTile = Tile_End;
		_float3 CacultedPos = pTerrain->PutOnTerrain(&bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector(),nullptr,&eNowTile);


		m_pTransformCom->Turn_CW(XMVectorSet(0, 1, 0, 0), fDeltaTime * 7);
		if (eNowTile == Tile_None || eNowTile == Tile_DynamicNoneTile)
		{
			_float3 New = m_vOldPos;
			New.y = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).y;
			m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, New);
		}
		if (m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).y < 1)
		{
			Set_IsDead();
		}


		m_pTransformCom->Turn_CW(XMVectorSet(0, 1, 0, 0), fDeltaTime * 7);
		if (bIsOn)
		{
			m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CacultedPos);
			m_bIsSpout = true;
		}

	}

	else {
		_Vector ToPlayerDir = m_pPlayerTransform->Get_MatrixState(CTransform::STATE_POS)
			- m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);

		m_pTransformCom->Turn_CW(XMVectorSet(0, 1, 0, 0), fDeltaTime);

		if (m_fStartTimer < 1)
			m_pTransformCom->MovetoDir(XMVectorSet(0, 1, 0, 0), fDeltaTime);
		else if (m_fStartTimer < 2)
			m_pTransformCom->MovetoDir(XMVectorSet(0, -1, 0, 0), fDeltaTime);
		else
			m_fStartTimer = 0;
		


		_float fBetweenLength = XMVectorGetX(XMVector3Length(ToPlayerDir));

		if (fBetweenLength > m_fRangeRadius)
		{
			m_bIsPlayerCloser = false;
			if (m_fStartTimer <m_fTargetTime)
				m_pTransformCom->MovetoDir(XMVectorSet(0, 1, 0, 0), fDeltaTime);
			else if (m_fStartTimer < m_fTargetTime* 2)
				m_pTransformCom->MovetoDir(XMVectorSet(0, -1, 0, 0), fDeltaTime);
			else
			{
				m_fStartTimer = 0;
				m_fTargetTime = GetSingle(CUtilityMgr)->RandomFloat(0.75, 1.25);
			}
			return _int();
		}

		if (abs(fBetweenLength) < 0.3f)
		{
			{
				SOUNDDESC tSoundDesc;

				tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
				tSoundDesc.vMinMax = _float2(0, 25);
				tSoundDesc.fTargetSound = 0.30f;
				wstring SoundTrack = L"";
				SoundTrack = L"MapObject_pickups_rose01.ogg";

				//SoundTrack = L"MapObject_shrinkflower_open.ogg";

				g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
			}

			{
				SOUNDDESC tSoundDesc;

				tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
				tSoundDesc.vMinMax = _float2(0, 25);
				tSoundDesc.fTargetSound = 0.20f;
				wstring SoundTrack = L"";
				SoundTrack = L"MapObject_pickups_rose_magic0" + to_wstring(rand() % 2 + 1) + L".ogg";

				//SoundTrack = L"MapObject_shrinkflower_open.ogg";

				g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
			}


			m_pPlayer->Heal_to_Player(rand() % 4 + 4);

			Set_IsDead();

		}
		m_pTransformCom->MovetoDir_bySpeed(ToPlayerDir, (m_fRangeRadius - fBetweenLength) * m_fRangeRadius, fDeltaTime);

	}

	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));
	return _int();
}

_int CRoseObj::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	//if (!m_bIsPlayerCloser) return _int();


	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CRoseObj::Render()
{
	if (__super::Render() < 0)
		return -1;

	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader_ApplyPivot(m_pShaderCom, "g_WorldMatrix"));


	FAILED_CHECK(__super::SetUp_ConstTable(m_pShaderCom));


	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 15, i));
	}


	return _int();
}

_int CRoseObj::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;
	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));



	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{
		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 9, i));
	}

	return _int();
}

HRESULT CRoseObj::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_HealthRose), TAG_COM(Com_Model), (CComponent**)&m_pModel));


	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = GetSingle(CUtilityMgr)->RandomFloat(0.06f, 0.08f);
	tDesc.fRotationPerSec = XMConvertToRadians(GetSingle(CUtilityMgr)->RandomFloat(50, 70));
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0.3f, 0);


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));


	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);


	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);


	return S_OK;
}

void CRoseObj::Add_JumpPower(_float power)
{
	m_fJumpPower = m_fMaxJumpPower = power;
}

CRoseObj * CRoseObj::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CRoseObj*	pInstance = new CRoseObj(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CRoseObj");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRoseObj::Clone(void * pArg)
{
	CRoseObj*	pInstance = new CRoseObj(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CRoseObj");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRoseObj::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pShaderCom);

}
