#pragma once

#include "GameObject.h"

BEGIN(Client)

class CRendererEditSceneUI final :public CGameObject
{
private:
	CRendererEditSceneUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CRendererEditSceneUI(const CRendererEditSceneUI& rhs);
	virtual ~CRendererEditSceneUI() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;

	void Set_VecEditUI(vector<EDITUI>* pVec);

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Cube*		m_pVIBufferCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;

	vector<EDITUI>*		m_pVecEditUI = nullptr;
private:
	_float4x4		m_ProjMatrix = XMMatrixTranspose(XMMatrixOrthographicLH(_int(g_iWinCX), _int(g_iWinCY), -1.f, 1.f));

private:
	HRESULT SetUp_Components();

	HRESULT Apply_UIDesc_To_Transform(UIDESC tDesc);
	HRESULT Apply_Rect_To_Transform(FLOATRECT tRect);

public:
	static CRendererEditSceneUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END