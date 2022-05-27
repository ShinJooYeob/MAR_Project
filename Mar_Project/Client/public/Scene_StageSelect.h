#pragma once

#include "Scene.h"

BEGIN(Client)
class CScene_StageSelect final : public CScene
{
private:
	explicit CScene_StageSelect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual ~CScene_StageSelect() = default;

public:
	virtual HRESULT Initialize();
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

	//�����̰� �ǵ�
	virtual _int Change_to_NextScene() override;

private:
	HRESULT Ready_Layer_MainCamera(const _tchar* pLayerTag);




public:
	static CScene_StageSelect* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual void Free() override;
};
END
