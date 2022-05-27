#pragma once
#include "GameObject.h"



BEGIN(Client)

class CLobyAlice final : public CGameObject
{
private:
	CLobyAlice(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CLobyAlice(const CLobyAlice& rhs);
	virtual ~CLobyAlice() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModel = nullptr;
	CTransform*			m_pTransformCom = nullptr;


private:
	HRESULT SetUp_Components();

public:
	static CLobyAlice* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
