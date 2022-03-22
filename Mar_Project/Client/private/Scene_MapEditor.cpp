#include "stdafx.h"
#include "..\Public\Scene_MapEditor.h"
#include "Scene_Loading.h"
#include "Camera_Main.h"


CScene_MapEditor::CScene_MapEditor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CScene(pDevice,pDeviceContext)
{
}

HRESULT CScene_MapEditor::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	Prevent_Order = false;
	FAILED_CHECK(Ready_Layer_MainCamera(TAG_LAY(Layer_Camera_Main)));




	return S_OK;
}

_int CScene_MapEditor::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	if (m_bIsNeedToSceneChange)
		return Change_to_NextScene();
	
	if (GETIMGUI->Update_ImguiMgr(fDeltaTime) >=0)
	{
		FAILED_CHECK(Update_First_Frame(fDeltaTime, "FirstBar"));


		Prevent_Order = true;
	}
	return 0;
}

_int CScene_MapEditor::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsNeedToSceneChange)
		return Change_to_NextScene();

	return 0;
}

_int CScene_MapEditor::Render()
{
	if (__super::Render() < 0)
		return -1;




#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("MapEditScene"));
#endif // _DEBUG


	return 0;
}

_int CScene_MapEditor::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

#ifdef USE_IMGUI
	if (Prevent_Order)
	{
		GETIMGUI->Render_ImguiMgr();
		Prevent_Order = false;
	}
#endif // USE_IMGUI


	return 0;
}

_int CScene_MapEditor::Change_to_NextScene()
{

	switch (m_eNextScene)
	{

	case SCENEID::SCENE_LOBY:
	{
		FAILED_CHECK(GetSingle(CGameInstance)->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENEID::SCENE_LOBY), SCENEID::SCENE_LOADING));
		break;
	}


	}
	return _int();
}

HRESULT CScene_MapEditor::Update_First_Frame(_double fDeltatime, const char * szFrameBarName)
{
	GETIMGUI->Begin_Update_Frame(fDeltatime, szFrameBarName);

		static float f = 0.0f;
		static int counter = 0;


		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	GETIMGUI->End_Update_Frame();
	return S_OK;
}


HRESULT CScene_MapEditor::Ready_Layer_MainCamera(const _tchar * pLayerTag)
{



	
	return S_OK;
}


CScene_MapEditor * CScene_MapEditor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CScene_MapEditor* pTempGame = new CScene_MapEditor(pDevice, pDeviceContext);

	if (FAILED(pTempGame->Initialize()))
	{
		Safe_Release(pTempGame);
		MSGBOX("Failed to Creating Scene_Stage1");
		return nullptr;
	}

	return pTempGame;
}

void CScene_MapEditor::Free()
{
	__super::Free();

}
