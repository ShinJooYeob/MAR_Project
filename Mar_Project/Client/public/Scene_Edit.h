#pragma once

#include "Scene.h"
#include "GameObject.h"


#ifdef USE_IMGUI
#include "ImguiMgr.h"
#endif // USE_IMGUI


//const char* DataFilePath = "../bin/Resources/Data/";
//const _tchar* MeshFilePath = L"../bin/Resources/Mesh/";


BEGIN(Client)
class CESCursor;
class CScene_Edit final : public CScene
{

#ifdef USE_IMGUI


	enum eDATATYPE
	{
		Data_Map,
		Data_UI,
		Data_Particle,
		Data_CameraAction,
		Data_HeightMap,
		Data_FilterMap,
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
	virtual _int LightRender()override;


	virtual _int Change_to_NextScene() override;

//imgui ???? ????


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
	_float	m_fPickingedPosition[3] = {0};
#pragma endregion MapTab

#pragma region UITab


	HRESULT Ready_Layer_RendererEditUI(const _tchar* pLayerTag);

	HRESULT Update_UITab(_double fDeltatime);

	HRESULT Widget_LoadUISRVTexture(_double fDeltatime);
	HRESULT Widget_CreateNBatchUI(_double fDeltatime);
	HRESULT Widget_DeleteBatchedUI(_double fDeltatime);



	HRESULT LoadTextureByAssimp(const _tchar* FileFullPath);

	ID3D11ShaderResourceView*  m_TargetSRV = nullptr;
	vector<EDITUI>		m_vecBatchedUI;
	_uint					m_iBatchedUIIndex = 0;
	char			m_szTexturPath[MAX_PATH] = "";
	class	CRendererEditSceneUI* m_pRendererEditUI = nullptr;

	UIDESC				m_fUIDesc ;
	FLOATRECT			m_fUIRect ;
	_float				m_fUIAngle = 0;
	_float				m_fDepth = 0;
	_bool				m_bIsRect = false;


	_bool				m_bModifyBatchedUI = false;
	_int				m_iModifyUIIndex= 0;

#pragma endregion UITab

#pragma region ParticleTab

	HRESULT Update_ParticleTab(_double fDeltatime);
	HRESULT Widget_SettingParticleDesc(_double fDeltatime);

	PARTICLEDESC m_tParticleDesc;

#pragma endregion ParticleTab

#pragma region CamTab
	HRESULT Update_CameraActionTab(_double fDeltatime);

	HRESULT Widget_CursorBatch(_double fDeltatime);
	HRESULT Widget_CreatedCamPosListBox(_double fDeltatime);
	HRESULT Widget_Play(_double fDeltatime);

	HRESULT Widget_SaveLoadCamAction(_double fDeltatime);

	
	CAMACTDESC CamDesc = { 0 };

	_int iPickKinds = 0;
	_uint iCamPosIndex = 0;
	_uint iCamLookIndex = 0;
	
	CESCursor*		m_pCamCursor = nullptr;
	vector<CESCursor*> m_vecCamPosBatchedObj;
	vector<CESCursor*> m_vecLookBatchedObj;


	vector<CAMACTDESC>		 m_vecCamPositions;
	vector<CAMACTDESC>		 m_vecLookPostions;


#pragma endregion CamTab


#pragma region HeightMap


	HRESULT Update_HeightMap(_double fDeltatime);
	HRESULT Widget_CreateDeleteHeightMap(_double fDeltatime);
	HRESULT Widget_ChangeValue(_double fDeltatime);

	HRESULT Widget_TextureSaveNLoad(_double fDeltatime);


private:
	_int		m_iMapSize[2];
	_int		m_iEditingKinds = 0;
	_int		m_iKindsOfFilter = 0;
	class CWireTerrain* m_pCreatedTerrain = nullptr;

	_int		m_PickingEasingType = 0;
	_float		m_fPickingRadius = 3.f;

	_float		m_fPickingHeight = 3.f;
	_float		m_fRandomHeightRange[2] = {0,1};

	_float		m_fEasingSharpness = 0;

	_bool		m_bIsBlockPick = false;
	_bool		m_bIsRandomHeight = false;


	_float		m_MovableHeight = 0.5f;
#pragma endregion HeightMap





#endif // USE_IMGUI


private:
	class CCamera_Editor*				m_pEditorCam = nullptr;

private:
	HRESULT Ready_Layer_MainCamera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_ParticleDesc();
	HRESULT Ready_CamActionCursor(const _tchar* pLayerTag);


public:
	static CScene_Edit* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual void Free() override;
};
END
