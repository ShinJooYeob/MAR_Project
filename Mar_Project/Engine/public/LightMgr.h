#pragma once


#include "Base.h"
BEGIN(Engine)

class CLightMgr final : public CBase
{
	DECLARE_SINGLETON(CLightMgr);

private:
	explicit CLightMgr();
	virtual ~CLightMgr() = default;


public:
	HRESULT Initialize_LightMgr(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

public:
	const LIGHTDESC* Get_LightDesc(LIGHTDESC::TYPE eLightType, _uint iIndex) const;
	HRESULT Add_Light(const LIGHTDESC& LightDesc);

private:
	list<class CLight*>			m_ArrLightList[LIGHTDESC::TYPE_END];
	typedef list<class CLight*>	LIGHTS;

	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

public:
	virtual void Free() override;
};

END