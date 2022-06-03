#pragma once

#include "Component.h"


BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CRenderer final :public CComponent
{
public:
	enum RENDERGROUP 
	{
		RENDER_PRIORITY, RENDER_NONBLEND, RENDER_PRIORITYBLEND, RENDER_POSTPROCESSING, RENDER_NONBLEND_NOLIGHT, RENDER_BLEND, RENDER_AFTEROBJ, RENDER_UI, RENDER_END
	};

private:
	explicit CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual ~CRenderer() = default;

private:
	virtual HRESULT Initialize_Prototype(void * pArg)override;
	virtual HRESULT Initialize_Clone(void * pArg)override;


public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderID ,CGameObject* pGameObject);
	HRESULT Add_TrailGroup(class CSwordTrail* pComponent);
	HRESULT Add_DebugGroup(class CComponent* pComponent);
	HRESULT Render_RenderGroup();
	HRESULT Clear_RenderGroup_forSceneChaging();



private:
	list<CGameObject*>				m_RenderObjectList[RENDER_END];
	typedef list<CGameObject*>		RENDEROBJECTS;

	list<class CSwordTrail*>					m_TrailObjectList;
	typedef list<class CSwordTrail*>			TRAILOBJECT;

private:
	list<class CComponent*>					m_DebugObjectList;
	typedef list<class CComponent*>			DEBUGOBJECT;

private:
	class CRenderTargetMgr*					m_pRenderTargetMgr = nullptr;
	class CLightMgr*						m_pLightMgr = nullptr;
	class CGraphic_Device*					m_pGraphicDevice= nullptr;

	class CVIBuffer_Rect*					m_pVIBuffer = nullptr;
	class CShader*							m_pShader = nullptr;


private:
	MATRIXWVP					m_WVPmat;

	MATRIXWVP					m_LightWVPmat;
	ID3D11DepthStencilView*		m_LightDepthStencil = nullptr;
	ID3D11DepthStencilView*		m_DownScaledDepthStencil[5] = { nullptr };
	_float						m_BlurLuminence = 0.192f;


private:
	HRESULT Render_Priority();

	HRESULT Render_ShadowMap();

	HRESULT Render_NonAlpha();
	HRESULT Render_PriorityAlpha();

	HRESULT Render_BlurShadow();
	HRESULT Render_Lights();
	HRESULT Render_DeferredTexture();

	HRESULT Render_PostProcessing();
	HRESULT Render_BlurLuminence();

	HRESULT Render_NonAlpha_NoDeferrd();
	

	HRESULT Render_Alpha();
	HRESULT Render_AfterObj();
	HRESULT Render_UI();

#ifdef _DEBUG
	HRESULT Render_Debug();
#endif // _DEBUG


public:
	static CRenderer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg = nullptr);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END	