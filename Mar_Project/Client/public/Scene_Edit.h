#pragma once

#include "Scene.h"
#include "GameObject.h"


#ifdef USE_IMGUI
#include "ImguiMgr.h"
#endif // USE_IMGUI


//const char* DataFilePath = "../bin/Resources/Data/";
//const _tchar* MeshFilePath = L"../bin/Resources/Mesh/";


BEGIN(Client)
class CScene_Edit final : public CScene
{

#ifdef USE_IMGUI
	typedef struct tagObjectElement
	{
		_tchar			ObjectID[128] = L"EditorCursor";
		_tchar			MeshID[128] = L"Mesh_None";
		_tchar			TexturePath[128] = L"TPath_None";
		_tchar			TextureKey[128] = L"TKey_None";
		_uint			PassIndex = 0;
		_float4x4		matSRT;
		_float4x4		matTransform;
		CGameObject*	pObject = nullptr;

	}OBJELEMENT;

	enum eDATATYPE
	{
		Data_Map,
		Data_UI,
		Data_Particle,
		Data_CameraAction,
		Datatype_End
	};
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

#pragma region Total

	HRESULT Update_First_Frame(_double fDeltatime, const char* szFrameBarName);


	void Make_HelpWidget(const char* szString);
	void Make_VerticalSpacing(_uint count);
	HRESULT Sava_Data(const char* szFileName, eDATATYPE iKinds);
	HRESULT Load_Data(const char* szFileName, eDATATYPE iKinds);
	HRESULT Input_KeyBoard(_double fDeltaTime);


private:
	CGameInstance* m_pGameInstance = nullptr;
	_uint m_iNowTab = 0;
	_bool	Prevent_Order = false;
	_bool bArrWindowFlag[10];
	_uint ibClickChecker = 0;
	list<string>		m_FilePathList;
#pragma endregion Total

#pragma region MapTab
	HRESULT Update_MapTab(_double fDeltatime);
	HRESULT Widget_SRT(_double fDeltatime);
	HRESULT Widget_BatchedObjectList(_double fDeltatime);

	HRESULT Widget_CreateDeleteObject(_double fDeltatime);
	HRESULT Widget_SaveLoadMapData(_double fDeltatime);

	HRESULT	RenewElenmetTransform(OBJELEMENT* pObjElement);

private:
	_int m_bIsModelMove = 0;
	_int m_iKindsOfMoving = 0;
	_int m_iSelectedXYZ = 0;
	_int m_iPassIndex = 0;
	_float m_ArrBuffer[4];
	_float4x4* m_SelectedObjectSRT = nullptr;

	_uint m_iBatchedVecIndex = 0;
	vector<OBJELEMENT>		m_vecBatchedObject;

	_uint m_iSelectedObjectNMesh[2];
#pragma endregion MapTab

#pragma region UITab

	HRESULT Update_UITab(_double fDeltatime);


#pragma endregion UITab

#pragma region ParticleTab

	HRESULT Update_ParticleTab(_double fDeltatime);


#pragma endregion ParticleTab

#pragma region CamTab

	HRESULT Update_CameraActionTab(_double fDeltatime);


#pragma endregion CamTab


#pragma region HeightMap


	HRESULT Update_HeightMap(_double fDeltatime);
	HRESULT Widget_CreateDeleteHeightMap(_double fDeltatime);

private:
	_int		m_iMapSize[2];
	class CWireTerrain* m_pCreatedTerrain = nullptr;

#pragma endregion HeightMap





#endif // USE_IMGUI


private:
	class CCamera_Editor*				m_pEditorCam = nullptr;
	class CWireTerrain*					m_pWireTerrain= nullptr;
private:
	HRESULT Ready_Layer_MainCamera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_WireTerrain(const _tchar* pLayerTag);


public:
	static CScene_Edit* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual void Free() override;
};
END
