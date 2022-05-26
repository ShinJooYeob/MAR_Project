#include "..\public\Light.h"



CLight::CLight(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CLight::Initilize_Protoype(const LIGHTDESC & LightDesc)
{
	memcpy(&m_LightDesc, &LightDesc, sizeof(LIGHTDESC));
	return S_OK;
}

HRESULT CLight::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	_uint		iPassIndex = 0;

	if (LIGHTDESC::TYPE_DIRECTIONAL == m_LightDesc.eLightType)
	{
		pShader->Set_RawValue("g_vLightDir", &m_LightDesc.vVector, sizeof(_float4));
		iPassIndex = 1;
	}
	else
	{
		pShader->Set_RawValue("g_vLightPos", &m_LightDesc.vVector, sizeof(_float4));
		pShader->Set_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float));
		iPassIndex = 2;
	}

	pShader->Set_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4));
	pShader->Set_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4));
	pShader->Set_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4));

	return pVIBuffer->Render(pShader, iPassIndex);
}

CLight * CLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	CLight*	pInstance = new CLight(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initilize_Protoype(LightDesc)))
	{
		MSGBOX("Failed to Created CLight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
