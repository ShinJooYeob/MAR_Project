#include "stdafx.h"
#include "..\public\BreakablePiece.h"
#include "Terrain.h"



CBreakablePiece::CBreakablePiece(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CBreakablePiece::CBreakablePiece(const CBreakablePiece & rhs)
	: CMapObject(rhs)
{
}

HRESULT CBreakablePiece::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CBreakablePiece::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
		m_iKindsOfPiece = _uint((*(_float4*)pArg).w);
	

	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
	{
		_float3 vPos = (*(_float3*)pArg);
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, vPos);
	}

	m_fStartTimer = 0;

	CUtilityMgr* pUtilMgr = GetSingle(CUtilityMgr);
	m_vSpoutDir = (pUtilMgr->RandomFloat3(-10000.f, 10000.f)).Get_Nomalize();
	m_fRandPower = (pUtilMgr->RandomFloat(1, 3.f));

	Add_JumpPower(pUtilMgr->RandomFloat(3, 6.f));
	m_fTurningTime = 0;
	//m_vRotAxis = m_vSpoutDir;
	return S_OK;
}

_int CBreakablePiece::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	m_fTurningTime += fDeltaTime;

	if (m_fTurningTime > 0.07)
	{

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
			_uint eTileKinds = Tile_End;
			pTerrain->PutOnTerrain(&bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector(),nullptr, &eTileKinds);

				m_pTransformCom->Turn_CW(m_vSpoutDir.XMVector(), fDeltaTime * 7);

			if (bIsOn)
			{
				m_bIsSpout = true;
				Set_IsDead();
			}

		}
	}


	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));

		
	return _int();
}

_int CBreakablePiece::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	//if (!m_bIsPlayerCloser) return _int();



	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CBreakablePiece::Render()
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

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 0, i));
	}


	return _int();
}

_int CBreakablePiece::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

	return _int();
}

HRESULT CBreakablePiece::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));


	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(COMPONENTPROTOTYPEID(m_iKindsOfPiece)), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	

	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = GetSingle(CUtilityMgr)->RandomFloat(0.06f, 0.08f);
	tDesc.fRotationPerSec = XMConvertToRadians(GetSingle(CUtilityMgr)->RandomFloat(50,70));
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0.3f, 0);


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));




	return S_OK;
}

void CBreakablePiece::Add_JumpPower(_float power)
{

	m_fJumpPower = m_fMaxJumpPower = power;

}

CBreakablePiece * CBreakablePiece::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CBreakablePiece*	pInstance = new CBreakablePiece(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CBreakablePiece");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBreakablePiece::Clone(void * pArg)
{
	CBreakablePiece*	pInstance = new CBreakablePiece(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CBreakablePiece");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBreakablePiece::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pShaderCom);

}
