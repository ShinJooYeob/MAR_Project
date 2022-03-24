#include "stdafx.h"
#include "..\Public\Scene_Edit.h"
#include "Scene_Loading.h"
#include "Camera_Main.h"


CScene_Edit::CScene_Edit(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CScene(pDevice, pDeviceContext)
{
}

HRESULT CScene_Edit::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	FAILED_CHECK(Ready_Layer_MainCamera(TAG_LAY(Layer_Camera_Main)));


#ifdef USE_IMGUI


	//터짐 방지용 빈 오브젝트
	OBJELEMENT ObjElement;
	ObjElement.pObject = nullptr;
	ZeroMemory(ObjElement.matSRT.m, sizeof(_float) * 16);
	m_vecBatchedObject.push_back(ObjElement);




	Prevent_Order = false;
	m_SelectedObjectSRT = &(m_vecBatchedObject[0].matSRT);
	m_iBatchedVecIndex = 0;

	ZeroMemory(m_ArrBuffer, sizeof(_float) * 4);
	ZeroMemory(bArrWindowFlag, sizeof(_bool) * 10);
	ZeroMemory(m_iSelectedObjectNMesh, sizeof(_uint) * 2);



	m_ArrBuffer[3] = 0.1f;







#endif // USE_IMGUI
	return S_OK;
}

_int CScene_Edit::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	if (m_bIsNeedToSceneChange)
		return Change_to_NextScene();

#ifdef USE_IMGUI

	FAILED_CHECK(Input_KeyBoard(fDeltaTime));

	if (GETIMGUI->Update_ImguiMgr(fDeltaTime) >= 0)
	{


		FAILED_CHECK(Update_First_Frame(fDeltaTime, "FirstFrame"));


		Prevent_Order = true;
	}

#endif // USE_IMGUI
	return 0;
}

_int CScene_Edit::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsNeedToSceneChange)
		return Change_to_NextScene();

	return 0;
}

_int CScene_Edit::Render()
{
	if (__super::Render() < 0)
		return -1;




#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("MapEditScene"));
#endif // _DEBUG


	return 0;
}

_int CScene_Edit::LateRender()
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

_int CScene_Edit::Change_to_NextScene()
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

#ifdef USE_IMGUI
HRESULT CScene_Edit::Update_First_Frame(_double fDeltatime, const char * szFrameBarName)
{
	_bool demotrue = true;
	ImGui::ShowDemoWindow(&demotrue);

	ImGuiWindowFlags window_flags = 0;
	if (bArrWindowFlag[0])				window_flags |= ImGuiWindowFlags_NoBackground;
	if (bArrWindowFlag[1])				window_flags |= ImGuiWindowFlags_NoMove;
	/*
	if (bArrWindowFlag[2])				window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (bArrWindowFlag[3])				window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (!bArrWindowFlag[4])				window_flags |= ImGuiWindowFlags_MenuBar;
	if (bArrWindowFlag[5])				window_flags |= ImGuiWindowFlags_NoResize;
	if (bArrWindowFlag[6])				window_flags |= ImGuiWindowFlags_NoCollapse;
	if (bArrWindowFlag[7])				window_flags |= ImGuiWindowFlags_NoNav;
	if (bArrWindowFlag[8])				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	if (bArrWindowFlag[9])				window_flags |= ImGuiWindowFlags_UnsavedDocument;
	*/

	static _bool bFirstSetting = false;

	if (!bFirstSetting)
	{
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowSize(ImVec2(center.x * 0.667f, center.y * 2.f), ImGuiCond_None);
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_None, ImVec2(0.0f, 0.0f));
		bFirstSetting = true;
	}
	GETIMGUI->Begin_Update_Frame(fDeltatime, szFrameBarName, window_flags);




	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
	FAILED_CHECK(Widget_SRT(fDeltatime));

	Make_VerticalSpacing(2);
	ImGui::Separator();
	Make_VerticalSpacing(3);

	int open_action = -1;
	if (ImGui::Button("Open all about Object"))
		open_action = 1;
	ImGui::SameLine();
	if (ImGui::Button("Close all about Object"))
		open_action = 0;
	if (open_action != -1)
		ImGui::SetNextItemOpen(open_action != 0);
	FAILED_CHECK(Widget_BatchedObjectList(fDeltatime));

	if (open_action != -1)
		ImGui::SetNextItemOpen(open_action != 0);
	FAILED_CHECK(Widget_CreateDeleteObject(fDeltatime));


	Make_VerticalSpacing(2);
	ImGui::Separator();

	Make_VerticalSpacing(3);

	FAILED_CHECK(Widget_SaveLoadMapData(fDeltatime));






	ImGui::PopStyleVar();
	GETIMGUI->End_Update_Frame();
	return S_OK;

	/*{

	static float f = 0.0f;
	static int counter = 0;


	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	ImGui::Checkbox("Demo Window", &temp);      // Edit bools storing our window open/close state
	ImGui::Checkbox("Another Window", &temp);

	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}*/

}

HRESULT CScene_Edit::Widget_SRT(_double fDeltatime)
{
	ibClickChecker = 0;
	ImGui::RadioButton("Camera Moving", &m_bIsModelMove, 0); ImGui::SameLine();
	ImGui::RadioButton("Model Moving      ", &m_bIsModelMove, 1); ImGui::SameLine();
	ibClickChecker += _uint(ImGui::IsItemClicked());

	if (m_iBatchedVecIndex == 0)
	{
		m_bIsModelMove = 0;
	}
	else
	{
		if (ibClickChecker)
		{
			/////////////////////////////////////모델 정보 받아오기

			m_SelectedObjectSRT = &(m_vecBatchedObject[m_iBatchedVecIndex].matSRT);

		}
	}




	ImGui::TableNextColumn(); ImGui::Checkbox("BG X", &bArrWindowFlag[0]); ImGui::SameLine();
	ImGui::TableNextColumn(); ImGui::Checkbox("Move X\n\n\n", &bArrWindowFlag[1]);
	/*
	ImGui::TableNextColumn(); ImGui::Checkbox("No titlebar", &bArrWindowFlag[2]);
	ImGui::TableNextColumn(); ImGui::Checkbox("No scrollbar", &bArrWindowFlag[3]);
	ImGui::TableNextColumn(); ImGui::Checkbox("No menu", &bArrWindowFlag[4]);
	ImGui::TableNextColumn(); ImGui::Checkbox("No resize", &bArrWindowFlag[5]);
	ImGui::TableNextColumn(); ImGui::Checkbox("No collapse", &bArrWindowFlag[6]);
	ImGui::TableNextColumn(); ImGui::Checkbox("No nav", &bArrWindowFlag[7]);
	ImGui::TableNextColumn(); ImGui::Checkbox("No bring to front", &bArrWindowFlag[8]);
	ImGui::TableNextColumn(); ImGui::Checkbox("Unsaved document", &bArrWindowFlag[9]);
	*/

	if (!m_bIsModelMove)
		ImGui::BeginDisabled(true);

	ibClickChecker = 0;
	ImGui::RadioButton("Position", &m_iKindsOfMoving, 0); ImGui::SameLine();
	ibClickChecker += _uint(ImGui::IsItemClicked());
	ImGui::RadioButton("Rotation", &m_iKindsOfMoving, 1); ImGui::SameLine();
	ibClickChecker += _uint(ImGui::IsItemClicked());
	ImGui::RadioButton("Scale\n\n\n", &m_iKindsOfMoving, 2);
	ibClickChecker += _uint(ImGui::IsItemClicked());

	if (ibClickChecker)
	{
		ZeroMemory(m_ArrBuffer, sizeof(_float) * 4);
		m_ArrBuffer[3] = 0.1f;
	}

	ImGui::DragFloat("Drag Speed", &(m_ArrBuffer[3]), 0.1f, FLT_MIN, FLT_MAX);
	{

		char Label[64];
		if (m_iBatchedVecIndex)
			sprintf_s(Label, "\n  Selected Object Index : %d", m_iBatchedVecIndex);
		else
			sprintf_s(Label, "\n  !!!!!!!!!!!Create New Object!!!!!!!!!");

		ImGui::Text(Label);
	}
	switch (m_iSelectedXYZ)
	{
	case 0:
		ImGui::Text("  Position		| Selected : X");
		break;

	case 1:
		ImGui::Text("  Rotation		| Selected : Y");
		break;

	case 2:
		ImGui::Text("  Scale			| Selected : Z");
		break;

	default:
		break;
	}


	switch (m_iKindsOfMoving)
	{
	case 0://포지션
	{
		memcpy(m_ArrBuffer, &(m_SelectedObjectSRT->m[2]), sizeof(_float) * 3);
		ImGui::DragFloat3(" X Y Z", m_ArrBuffer, m_ArrBuffer[3], -FLT_MAX, FLT_MAX);
		memcpy(&(m_SelectedObjectSRT->m[2]), m_ArrBuffer, sizeof(_float) * 3);
	}
	break;

	case 1://로테이션
	{
		memcpy(m_ArrBuffer, &(m_SelectedObjectSRT->m[1]), sizeof(_float) * 3);
		ImGui::DragFloat3(" X Y Z", m_ArrBuffer, m_ArrBuffer[3], -FLT_MAX, FLT_MAX);
		memcpy(&(m_SelectedObjectSRT->m[1]), m_ArrBuffer, sizeof(_float) * 3);

	}
	break;


	case 2://Scale
	{
		memcpy(m_ArrBuffer, &(m_SelectedObjectSRT->m[0]), sizeof(_float) * 3);
		ImGui::DragFloat3(" X Y Z", m_ArrBuffer, m_ArrBuffer[3], -FLT_MAX, FLT_MAX);
		memcpy(&(m_SelectedObjectSRT->m[0]), m_ArrBuffer, sizeof(_float) * 3);
	}
	break;


	default:
		break;
	}



	/*회전값 용

	D3DXQUATERNION quat_x, quat_y, quat_z, quat_1, quat_2;
	_Matrix matrix;
	D3DXVECTOR3 axis_x(1.0f, 0.0f, 0.0f);
	D3DXVECTOR3 axis_y(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 axis_z(0.0f, 0.0f, 1.0f);
	D3DXQuaternionRotationAxis(&quat_x, &axis_x, rot.x);
	D3DXQuaternionRotationAxis(&quat_y, &axis_y, rot.y);
	D3DXQuaternionRotationAxis(&quat_z, &axis_z, rot.z);

	quat_1 = quat_y * quat_x * quat_z;
	D3DXQuaternionNormalize(&quat_2, &quat_1);
	D3DXMatrixRotationQuaternion(&matrix, &quat_2);

	m_ComTransform->Set_MatrixState(CTransform::STATE_RIGHT, *(_float3*)&matrix.m[0]);
	m_ComTransform->Set_MatrixState(CTransform::STATE_UP, *(_float3*)&matrix.m[1]);
	m_ComTransform->Set_MatrixState(CTransform::STATE_LOOK, *(_float3*)&matrix.m[2]);
	*/

	Make_VerticalSpacing(5);
	if (ImGui::TreeNode("Show All"))
	{

		static bool selected[4] = {};

		if (ImGui::BeginTable("split2", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
		{
			char HederLabel[32];
			sprintf_s(HederLabel, "SRT Type");
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Selectable(HederLabel, &selected[0], ImGuiSelectableFlags_SpanAllColumns);
			ImGui::TableNextColumn();
			ImGui::Text("X");
			ImGui::TableNextColumn();
			ImGui::Text("Y");
			ImGui::TableNextColumn();
			ImGui::Text("Z");

			for (int i = 0; i < 3; i++)
			{
				char label[32];
				switch (i)
				{
				case 0:
					sprintf_s(label, "Position");
					break;
				case 1:
					sprintf_s(label, "Rotation");
					break;
				case 2:
					sprintf_s(label, "Scale");
					break;
				default:
					sprintf_s(label, "ERROR");
					break;
				}

				_float3 vSRT = {};
				memcpy(&vSRT, &(m_SelectedObjectSRT->m[2 - i]), sizeof(_float) * 3);

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Selectable(label, &selected[i + 1], ImGuiSelectableFlags_SpanAllColumns);
				ImGui::TableNextColumn();
				ImGui::Text("%f", vSRT.x);
				ImGui::TableNextColumn();
				ImGui::Text("%f", vSRT.y);
				ImGui::TableNextColumn();
				ImGui::Text("%f", vSRT.z);
			}
			ImGui::EndTable();
		}


		ImGui::TreePop();
	}

	if (!m_bIsModelMove)
		ImGui::EndDisabled();

	return S_OK;
}

HRESULT CScene_Edit::Widget_BatchedObjectList(_double fDeltatime)
{
	Make_VerticalSpacing(2);
	if (ImGui::TreeNode("Batched Object List"))
	{
		vector<string> ObjectLabelIist;

		for (_uint i = 0; i < m_vecBatchedObject.size(); i++)
		{
			char HeaderLabel[64];

			sprintf_s(HeaderLabel, "%d. %ws (Mesh : %ws)", i,
				TAG_OP(OBJECTPROTOTYPEID(m_vecBatchedObject[i].ObjectID)),
				TAG_MESH(MESHTYPEID(m_vecBatchedObject[i].MeshID)));
			//sprintf_s(HederLabel, "%d. %ws (%d)",i, m_vecBatchedObject[i].pObject->Get_NameTag(), m_vecBatchedObject[i].ObjectID);

			ObjectLabelIist.push_back({ HeaderLabel });
		}


		{

			char Label[64];
			sprintf_s(Label, "Selected Index : %d  | Total Object Count : %zd", m_iBatchedVecIndex, ObjectLabelIist.size() - 1);

			ImGui::Text(Label);
		}
		if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{


			for (int n = 1; n < ObjectLabelIist.size(); n++)
			{
				const bool is_selected = (m_iBatchedVecIndex == n);
				if (ImGui::Selectable(ObjectLabelIist[n].c_str(), is_selected))
				{


					m_iBatchedVecIndex = n;
					m_SelectedObjectSRT = &(m_vecBatchedObject[m_iBatchedVecIndex].matSRT);
				}


				if (m_bIsModelMove != 0 && ImGui::IsItemHovered())
				{
					m_bIsModelMove = 0;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}



		Make_VerticalSpacing(2);

		if (ImGui::Button("Delete Object", ImVec2(-FLT_MIN, 0.0f)) && m_iBatchedVecIndex)
		{

			auto iter = m_vecBatchedObject.begin();

			for (_uint i = 0; i < m_iBatchedVecIndex; i++)
				iter++;

			m_vecBatchedObject.erase(iter);

			m_iBatchedVecIndex = 0;
			m_SelectedObjectSRT = &(m_vecBatchedObject[m_iBatchedVecIndex].matSRT);


			m_iBatchedVecIndex = _uint(m_vecBatchedObject.size() - 1);
			m_SelectedObjectSRT = &(m_vecBatchedObject[m_iBatchedVecIndex].matSRT);
		}



		ImGui::TreePop();
	}

	return S_OK;
}

HRESULT CScene_Edit::Widget_CreateDeleteObject(_double fDeltatime)
{

	Make_VerticalSpacing(5);
	if (ImGui::TreeNode("Create Object"))
	{


		ImGui::Text("Selected\n\n");

		{
			char buf[128];
			sprintf_s(buf, "%ws (Mesh : %ws)",
				TAG_OP(OBJECTPROTOTYPEID(m_iSelectedObjectNMesh[0])),
				TAG_MESH(MESHTYPEID(m_iSelectedObjectNMesh[1])));

			ImGui::Text(buf);
		}


		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_None | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar;
			{

				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
				ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 130), true, window_flags);


				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu("ObjectList"))
					{
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();

					//char buf[128];
					//sprintf_s(buf, "%ws\n", TAG_OP(OBJECTPROTOTYPEID(m_iSelectedObjectNMesh[0])));
					//ImGui::Text(buf);

				}

				if (ImGui::BeginTable("split", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
				{
					for (int i = 0; i < Object_Prototype_End; i++)
					{
						char buf[128];
						sprintf_s(buf, "%ws", TAG_OP(OBJECTPROTOTYPEID(i)));
						ImGui::TableNextColumn();

						if (ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f)))
							m_iSelectedObjectNMesh[0] = i;

						if (m_iBatchedVecIndex != 0 && ImGui::IsItemHovered())
						{
							m_iBatchedVecIndex = 0;
							m_SelectedObjectSRT = &(m_vecBatchedObject[m_iBatchedVecIndex].matSRT);
							ZeroMemory(m_iSelectedObjectNMesh, sizeof(_uint) * 2);
						}
					}
					ImGui::EndTable();
				}
				ImGui::EndChild();
				ImGui::PopStyleVar();
			}

			ImGui::SameLine();

			{

				ImGui::BeginChild("ChildR", ImVec2(0, 130), true, window_flags);


				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu("Mesh List"))
					{
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();

					//char buf[128];
					//sprintf_s(buf, "%ws\n", TAG_MESH(MESHTYPEID(m_iSelectedObjectNMesh[1])));
					//ImGui::Text(buf);
				}

				if (ImGui::BeginTable("split", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
				{



					for (int i = 0; i < MeshID_End; i++)
					{
						char buf[128];
						sprintf_s(buf, "%ws", TAG_MESH(MESHTYPEID(i)));
						ImGui::TableNextColumn();
						if (ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f)))
							m_iSelectedObjectNMesh[1] = i;

						if (m_iBatchedVecIndex != 0 && ImGui::IsItemHovered())
						{
							m_iBatchedVecIndex = 0;
							m_SelectedObjectSRT = &(m_vecBatchedObject[m_iBatchedVecIndex].matSRT);
							ZeroMemory(m_iSelectedObjectNMesh, sizeof(_uint) * 2);
						}
					}
					ImGui::EndTable();
				}
				ImGui::EndChild();
			}
		}

		Make_VerticalSpacing(2);
		if (ImGui::Button("Create Object", ImVec2(-FLT_MIN, 0.0f)))
		{
			///오브젝트 생성 코드 집어넣기

			OBJELEMENT ObjElement;

			ObjElement.pObject = nullptr;
			ZeroMemory(ObjElement.matSRT.m, sizeof(_float) * 16);
			ObjElement.ObjectID = m_iSelectedObjectNMesh[0];

			ObjElement.matSRT = m_vecBatchedObject[m_iBatchedVecIndex].matSRT;

			ObjElement.MeshID = m_iSelectedObjectNMesh[1];



			/////실제 생성하기


			m_vecBatchedObject.push_back(ObjElement);

			m_iBatchedVecIndex = _uint(m_vecBatchedObject.size() - 1);
			m_SelectedObjectSRT = &(m_vecBatchedObject[m_iBatchedVecIndex].matSRT);


		}


		ImGui::TreePop();
	}


	return S_OK;
}

HRESULT CScene_Edit::Widget_SaveLoadMapData(_double fDeltatime)
{
	if (ImGui::Button("New Map"))
		ImGui::OpenPopup("New Map");
	ImGui::SameLine();
	if (ImGui::Button("Save Map"))
		ImGui::OpenPopup("Save Map");
	ImGui::SameLine();
	if (ImGui::Button("Laod Map"))
		ImGui::OpenPopup("Laod Map");
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();

	// Always center this window when appearing
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("New Map", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("!!!!!!!!!!!!!!!!Waring!!!!!!!!!!!!!!!!\n\n Delete Batched Object Without Save!!!\n\n	Please Check Save One more\n\n\n");
		ImGui::Separator();

		//static int unused_i = 0;
		//ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

		//static bool dont_ask_me_next_time = false;
		//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		//ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
		//ImGui::PopStyleVar();

		if (ImGui::Button("OK", ImVec2(130, 0))) { 

			for (auto& tObjElement : m_vecBatchedObject)
			{
				//오브젝트 제대로 델리트 해주기
				Safe_Release(tObjElement.pObject);
			}
			m_vecBatchedObject.clear();

			OBJELEMENT ObjElement;
			ObjElement.pObject = nullptr;
			ZeroMemory(ObjElement.matSRT.m, sizeof(_float) * 16);
			m_vecBatchedObject.push_back(ObjElement);

			m_SelectedObjectSRT = &(m_vecBatchedObject[0].matSRT);
			m_iBatchedVecIndex = 0;

			ZeroMemory(m_ArrBuffer, sizeof(_float) * 4);
			m_ArrBuffer[3] = 0.1f;
			ZeroMemory(m_iSelectedObjectNMesh, sizeof(_uint) * 2);

			m_bIsModelMove = 0;
			m_iKindsOfMoving = 0;
			m_iSelectedXYZ = 0;




			ImGui::CloseCurrentPopup(); 
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(130, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}


	//////////////////////////////////////////////////////////////////////////


	// Always center this window when appearing
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Save Map", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Save Map!\n\n");




		if (m_FilePathList.size() == 0)
		{
			m_FilePathList.clear();
			_tfinddata64_t fd;
			__int64 handle = _tfindfirst64(TEXT("../bin/Resources/Data/Map/*.*"), &fd);
			if (handle == -1 || handle == 0)
				return E_FAIL;

			_int iResult = 0;

			//char szCurPath[128] = "../bin/Resources/Data/Map/";
			//char szFullPath[128] = "";
			char szFilename[MAX_PATH];

			while (iResult != -1)
			{
				if (!lstrcmp(fd.name, L".") || !lstrcmp(fd.name, L".."))
				{
					iResult = _tfindnext64(handle, &fd);
					continue;
				}


				WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
				//strcpy_s(szFullPath, szCurPath);
				//strcat_s(szFullPath, szFilename);
				m_FilePathList.push_back({szFilename});

				
				iResult = _tfindnext64(handle, &fd);
			}


			_findclose(handle);

		}




		ImGui::Separator();
		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			m_FilePathList.clear();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			m_FilePathList.clear();
		}
		ImGui::EndPopup();
	}


	//////////////////////////////////////////////////////////////////////////

	// Always center this window when appearing
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Laod Map", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Laod Map!\n\n");
		ImGui::Separator();

		//static int unused_i = 0;
		//ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

		static bool dont_ask_me_next_time = false;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
		ImGui::PopStyleVar();

		if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
	return S_OK;
}

HRESULT CScene_Edit::Update_Second_Frame(_double fDeltatime, const char * szFrameBarName)
{
	GETIMGUI->Begin_Update_Frame(fDeltatime, szFrameBarName);



	GETIMGUI->End_Update_Frame();
	return S_OK;
}

void CScene_Edit::Make_HelpWidget(const char * szString)
{

	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(szString);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

}

void CScene_Edit::Make_VerticalSpacing(_uint count)
{
	for (_uint i = 0; i < count; i++)
	{
		ImGui::Spacing();
	}

}

HRESULT CScene_Edit::Input_KeyBoard(_double fDeltaTime)
{

	CGameInstance* pInstance = GetSingle(CGameInstance);

	if (pInstance->Get_DIKeyState(DIK_SPACE) & DIS_Down)
	{
		if (m_bIsModelMove) m_bIsModelMove = 0;
		else
		{
			if (m_iBatchedVecIndex != 0)//설정한 오브젝트가 있다면 있다면
			{
				// 해당 오브젝트의 메트릭스 받아오기
				m_SelectedObjectSRT = &(m_vecBatchedObject[m_iBatchedVecIndex].matSRT);
				ZeroMemory(m_ArrBuffer, sizeof(_float) * 4);
				m_ArrBuffer[3] = 0.1f;

				m_bIsModelMove = 1;
			}
		}

	}


	if (m_bIsModelMove)
	{
		if (pInstance->Get_DIKeyState(DIK_1) & DIS_Down)		m_iKindsOfMoving = 0;
		else if (pInstance->Get_DIKeyState(DIK_2) & DIS_Down)	m_iKindsOfMoving = 1;
		else if (pInstance->Get_DIKeyState(DIK_3) & DIS_Down)	m_iKindsOfMoving = 2;



		if (pInstance->Get_DIMouseButtonState(CInput_Device::MBS_RBUTTON) & DIS_Down)
		{
			m_iSelectedXYZ++;
			if (m_iSelectedXYZ > 2)
				m_iSelectedXYZ = 0;
		}

		_long fWheelMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_WHEEL);
		if (fWheelMove)
		{
			_float tempValue;
			memcpy(&tempValue, ((_float*)(&(m_SelectedObjectSRT->m[2 - m_iKindsOfMoving])) + m_iSelectedXYZ), sizeof(_float));

			tempValue += _float(fWheelMove * m_ArrBuffer[3] * fDeltaTime);


			memcpy(((_float*)(&(m_SelectedObjectSRT->m[2 - m_iKindsOfMoving])) + m_iSelectedXYZ), &tempValue, sizeof(_float));

			int t = 0;
		}

	}


	return S_OK;
}


#endif // USE_IMGUI
HRESULT CScene_Edit::Ready_Layer_MainCamera(const _tchar * pLayerTag)
{




	return S_OK;
}


CScene_Edit * CScene_Edit::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CScene_Edit* pTempGame = new CScene_Edit(pDevice, pDeviceContext);

	if (FAILED(pTempGame->Initialize()))
	{
		Safe_Release(pTempGame);
		MSGBOX("Failed to Creating Scene_Stage1");
		return nullptr;
	}

	return pTempGame;
}

void CScene_Edit::Free()
{
	__super::Free();

}
