#include "..\public\LightMgr.h"
#include "Light.h"

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

const LIGHTDESC * CLightMgr::Get_LightDesc(LIGHTDESC::TYPE eLightType, _uint iIndex) const
{
	if (iIndex >= m_ArrLightList[eLightType].size())
		return nullptr;

	auto	iter = m_ArrLightList[eLightType].begin();


	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightDesc();
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
