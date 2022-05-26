#include "..\public\LightMgr.h"
#include "Light.h"
#include "RenderTargetMgr.h"
#include "PipeLineMgr.h"

IMPLEMENT_SINGLETON(CLightMgr);

CLightMgr::CLightMgr()
{
}

HRESULT CLightMgr::Initialize_LightMgr(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	return S_OK;
}

HRESULT CLightMgr::Render(CShader * pShader, CVIBuffer_Rect * pViBuffer, MATRIXWVP* pWVPMat)
{
	NULL_CHECK_RETURN(pShader, E_FAIL);
	NULL_CHECK_RETURN(pViBuffer, E_FAIL);


	CRenderTargetMgr*		pRenderTargetMgr = GetSingle(CRenderTargetMgr);
	CPipeLineMgr*		pPipeLineMgr = GetSingle(CPipeLineMgr);


	FAILED_CHECK(pShader->Set_Texture("g_NormalTexture", pRenderTargetMgr->Get_SRV(TEXT("Target_Normal"))));
	FAILED_CHECK(pShader->Set_Texture("g_DepthTexture", pRenderTargetMgr->Get_SRV(TEXT("Target_Depth"))));

	FAILED_CHECK(pShader->Set_RawValue("g_WorldMatrix", &(pWVPMat->WorldMatrix), sizeof(_float4x4)));
	FAILED_CHECK(pShader->Set_RawValue("g_ViewMatrix", &(pWVPMat->ViewMatrix), sizeof(_float4x4)));
	FAILED_CHECK(pShader->Set_RawValue("g_ProjMatrix", &(pWVPMat->ProjMatrix), sizeof(_float4x4)));

	_float4x4		ViewMatrixInv, ProjMatrixInv;

	XMStoreFloat4x4(&ViewMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLineMgr->Get_Transform_Matrix(PLM_VIEW))));
	XMStoreFloat4x4(&ProjMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLineMgr->Get_Transform_Matrix(PLM_PROJ))));


	FAILED_CHECK(pShader->Set_RawValue("g_ViewMatrixInv", &ViewMatrixInv, sizeof(_float4x4)));
	FAILED_CHECK(pShader->Set_RawValue("g_ProjMatrixInv", &ProjMatrixInv, sizeof(_float4x4)));
	FAILED_CHECK(pShader->Set_RawValue("g_vCamPosition", &pPipeLineMgr->Get_TargetPostion_float4(PLV_CAMERA), sizeof(_float4)));



	for (_uint i = 0; i < LIGHTDESC::TYPE_END; i++)
	{
		for (auto& pLight : m_ArrLightList[i])
		{
			if (nullptr != pLight)
				pLight->Render(pShader, pViBuffer);

		}

	}


	return S_OK;
}

const LIGHTDESC * CLightMgr::Get_LightDesc(LIGHTDESC::TYPE eLightType, _uint iIndex) const
{
	if (iIndex >= m_ArrLightList[eLightType].size())
		return nullptr;

	auto	iter = m_ArrLightList[eLightType].begin();


	for (_uint i = 0; i < iIndex; ++i)
		++iter;
	return (*iter)->Get_LightDesc();
}

HRESULT CLightMgr::EasingDiffuseLightDesc(LIGHTDESC::TYPE eLightType, _uint iIndex, _fVector vTargetDiffuse, _float MixRate)
{
	if (iIndex >= m_ArrLightList[eLightType].size())
		return E_FAIL;

	auto	iter = m_ArrLightList[eLightType].begin();


	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	LIGHTDESC* LightDesc = ((*iter)->Get_LightDesc());

	NULL_CHECK_RETURN(LightDesc, E_FAIL);


	LightDesc->vDiffuse = XMVectorLerp(XMVectorSet(1,1,1,1), vTargetDiffuse, MixRate);


	return S_OK;
}



HRESULT CLightMgr::Add_Light(const LIGHTDESC & LightDesc)
{
	CLight*		pLight = CLight::Create(m_pDevice, m_pDeviceContext, LightDesc);

	NULL_CHECK_RETURN(pLight, E_FAIL);

	m_ArrLightList[LightDesc.eLightType].push_back(pLight);

	return S_OK;
}


void CLightMgr::Free()
{

	for (_uint i = 0; i < LIGHTDESC::TYPE_END; i++)
	{
		for (auto& pLight : m_ArrLightList[i])
			Safe_Release(pLight);

		m_ArrLightList[i].clear();
	}

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
