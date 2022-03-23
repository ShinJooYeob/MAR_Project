#pragma once

#include "Scene.h"
#include "GameObject.h"


#ifdef USE_IMGUI
#include "ImguiMgr.h"


#endif // USE_IMGUI





BEGIN(Client)
class CScene_Edit final : public CScene
{

#ifdef USE_IMGUI
	typedef struct tagObjectElement
	{
		_uint			ObjectID = Prototype_TestObject;
		_uint			MeshID = Mesh_TestMesh;
		_Matrix			matSRT;
		CGameObject*	pObject;

	}OBJELEMENT;


#endif



private:
	explicit CScene_Edit(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual ~CScene_Edit() = default;

public:
	virtual HRESULT Initialize();
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;


	virtual _int Change_to_NextScene() override;

//imgui 세팅 관련


#ifdef USE_IMGUI
private:
	HRESULT Input_KeyBoard(_double fDeltaTime);


	HRESULT Update_First_Frame(_double fDeltatime, const char* szFrameBarName);
	HRESULT Widget_SRT(_double fDeltatime);
	HRESULT Widget_BatchedObjectList(_double fDeltatime);

	HRESULT Widget_CreateDeleteObject(_double fDeltatime);
	HRESULT Widget_SaveLoadMapData(_double fDeltatime);
	

	HRESULT Update_Second_Frame(_double fDeltatime, const char* szFrameBarName);


	void Make_HelpWidget(const char* szString);
	void Make_VerticalSpacing(_uint count);

private:
	_bool bArrWindowFlag[10];
	_int m_bIsModelMove = 0;
	_int m_iKindsOfMoving = 0;
	_int m_iSelectedXYZ = 0;
	_float m_ArrBuffer[4];
	_Matrix* m_SelectedObjectSRT = nullptr;

	_uint m_iBatchedVecIndex = 0;
	vector<OBJELEMENT>		m_vecBatchedObject;

	_uint m_iSelectedObjectNMesh[2];


	_uint ibClickChecker = 0;
	_bool	Prevent_Order = false;
#endif // USE_IMGUI

private:
	HRESULT Ready_Layer_MainCamera(const _tchar* pLayerTag);


public:
	static CScene_Edit* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual void Free() override;
};
END
