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
		_float3 Pos = *(_float3*)pArg;
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, Pos);
	}
	m_pTransformCom->Scaled_All(_float3(0.3f));



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
		pTerrain->PutOnTerrain(&bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector());

		if (bIsOn)
			m_bIsSpout = true;

	}

	else {
		_Vector ToPlayerDir = m_pPlayerTransform->Get_MatrixState(CTransform::STATE_POS)
			- m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);

		_float fBetweenLength = XMVectorGetX(XMVector3Length(ToPlayerDir));

		if (fBetweenLength > m_fRangeRadius)
		{
			m_bIsPlayerCloser = false;
			return _int();
		}

		if (abs(fBetweenLength) < 0.3f)
		{
			Set_IsDead();
			FAILED_CHECK(g_pGameInstance->Add_GameObject_To_Layer(SCENE_LOBY, TAG_LAY(Layer_RoseObj), TAG_OP(Prototype_RoseObj), &_float3(12, 3, 5)));

		}
		m_pTransformCom->MovetoDir_bySpeed(ToPlayerDir, (m_fRangeRadius - fBetweenLength) * m_fRangeRadius, fDeltaTime);

	}

		
	return _int();
}

_int CRoseObj::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	//if (!m_bIsPlayerCloser) return _int();


	if (g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS)))
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CRoseObj::Render()
{
	if (__super::Render() < 0)
		return -1;

	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	CGameInstance* pInstance = g_pGameInstance;

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));

	FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture",2));


	FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, 0));

	return _int();
}

_int CRoseObj::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

	return _int();
}

HRESULT CRoseObj::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VCT), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_VIBuffer_Cube), TAG_COM(Com_VIBuffer), (CComponent**)&m_pVIBufferCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Texture_SkyBox), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));


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
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
