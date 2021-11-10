/*
	name: SCMD2 Search Plugin (Scm Draft)
	description: Filter unit items
	date: 2021-11-11
*/

#include "pch.h"
#include "dllmain.h"

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_DETACH:
		break;
	}
	return true;
}

DWORD WINAPI GetPluginVersion(void)
{
	return PLUGINVERSION;
}

// This code is run when the menu is pressed.
BOOL WINAPI RunPlugin(TEngineData* EngineData,		//	Struct containing engine data
	DWORD CurSection,				//	Section plugin is being run for (Currently either triggers or mission briefing)
	CChunkData* Triggers,			//	Pointer to trigger datachunk
	CChunkData* MissionBriefing,	//	Pointer to mission briefing datachunk
	CChunkData* SwitchRenaming,		//	Pointer to switch renaming datachunk
	CChunkData* UnitProperties,		//	Pointer to unit properties datachunk
	CChunkData* UnitPropUsage)	//	Poin
{

	return true;
}

// Menu name specifier.
BOOL WINAPI PluginGetMenuString(DWORD Section, CHAR* MenuString, WORD StringLength)
{
	return false;
}

BOOL WINAPI InitPlugin(HWND MainWindow,
	HINSTANCE MainInstance,
	AllocRam AllocMem,
	DeAllocRam DeleteMem,
	ReAllocRam ResizeMem,
	DWORD* RequestedSections)	//	DWORD[8]
{
#if _DEBUG
	DebugConsole::Show();
#endif

	scmd2_malloc = AllocMem;
	scmd2_free = DeleteMem;
	scmd2_realloc = ResizeMem;

	hMainWindow = MainWindow;
	hMainInstance = MainInstance;

	hTreeView = FindWindowEx(MainWindow, NULL, _T("SysTreeView32"), NULL);
	if (!hTreeView)
		return false;

	// Set treeview height
	RECT mainRect;
	GetWindowRect(hMainWindow, &mainRect);
	DWORD mainWidth = mainRect.right - mainRect.left;
	DWORD mainHeight = mainRect.bottom - mainRect.top;

	RECT treeViewRect;
	GetWindowRect(hTreeView, &treeViewRect);
	DWORD treeViewWidth = treeViewRect.right - treeViewRect.left;
	DWORD treeViewHeight = treeViewRect.bottom - treeViewRect.top;
	ScreenToClient(hMainWindow, (LPPOINT)&treeViewRect);

	treeViewHeight = mainHeight - treeViewRect.top - 120;
	MoveWindow(hTreeView, treeViewRect.left, treeViewRect.top, treeViewWidth, treeViewHeight, TRUE);


	// Create [left bottom] control

	// TextBox
	hEdit = CreateWindowEx(0, _T("Edit"), nullptr, 
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT, 
		0, 0, 0, 0, 
		hMainWindow, (HMENU)IDC_EDIT, hMainInstance, NULL);

	HFONT hFont = CreateFont(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Malgun Gothic"));

	SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);


	// CheckBox
	hCheckBox = CreateWindowEx(0, _T("button"), nullptr,
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
		0, 0, 0, 0,
		hMainWindow, (HMENU)IDC_CHECKBOX, hMainInstance, NULL);

	// My TreeView
	DWORD dwStyle = GetWindowLong(hTreeView, GWL_STYLE);
	//  WS_VISIBLE | WS_CHILDWINDOW | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS
	if (dwStyle & WS_VISIBLE)
		dwStyle -= WS_VISIBLE; // hide

	DWORD dwExStyle = GetWindowLong(hTreeView, GWL_EXSTYLE);
	// WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CLIENTEDGE

	hMyTreeView = CreateWindowEx(dwExStyle, _T("SysTreeView32"), NULL, dwStyle, 
		0, 0, 0, 0,
		hMainWindow, (HMENU)IDC_MYTREEVIEW, hMainInstance, NULL);

	SetWindowPos(hMyTreeView, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	
	TreeView.SetTreeView(hTreeView);
	MyTreeView.SetTreeView(hMyTreeView);

	oldWndProc = GetWindowLong(MainWindow, GWLP_WNDPROC);
	SetWindowLong(MainWindow, GWLP_WNDPROC, (LONG)MainWndProc);
	ReSizeControl();
	return true;
}



void ReSizeControl() {
	// get main window rect
	RECT mainRect;
	GetWindowRect(hMainWindow, &mainRect);
	DWORD mainWidth = mainRect.right - mainRect.left;
	DWORD mainHeight = mainRect.bottom - mainRect.top;

	// Get original treeView rect
	RECT treeViewRect;
	GetWindowRect(hTreeView, &treeViewRect);
	DWORD treeViewWidth = treeViewRect.right - treeViewRect.left;
	DWORD treeViewHeight = treeViewRect.bottom - treeViewRect.top;
	ScreenToClient(hMainWindow, (LPPOINT)&treeViewRect);

	treeViewHeight = mainHeight - treeViewRect.top - 120;

	MoveWindow(hTreeView, treeViewRect.left, treeViewRect.top, treeViewWidth, treeViewHeight, TRUE);
	MoveWindow(hMyTreeView, treeViewRect.left, treeViewRect.top, treeViewWidth, treeViewHeight, TRUE);

	MoveWindow(hEdit, treeViewRect.left, mainHeight - 110, treeViewWidth - 20, 20, true);
	MoveWindow(hCheckBox, treeViewRect.left + treeViewWidth - 15, mainHeight - 110, 20, 20, true);
}

void OnChange() {
	TCHAR szFilter[200] = { 0, };
	DWORD len = GetWindowText(hEdit, szFilter, _countof(szFilter));
	if (!len) {
		TreeView.Show();
		MyTreeView.Hide();
		return;
	}

	// toggle control
	TreeView.Hide();
	MyTreeView.Show();

	MyTreeView.DeleteAll();
	
	TreeView.ReadAllItem();
	TreeView.FilterData(szFilter);

	MyTreeView.SetTreeViewItem(TreeView);

	DWORD bCheck = SendMessage(hCheckBox, BM_GETCHECK, NULL, NULL);
	if (bCheck == BST_CHECKED) {
		MyTreeView.AllExpand();
	}

}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
		case WM_PAINT:
		case WM_NCPAINT:
		case WM_PRINTCLIENT:
		case WM_ERASEBKGND:
			ReSizeControl();
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
			//return CallWindowProc((WNDPROC)oldWndProc, hWnd, uMsg, wParam, lParam); <-- not working

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
			case IDC_EDIT:
				switch (HIWORD(wParam)) {
				case EN_CHANGE: 
					OnChange();
					break;
				}
			}

		//case WM_PAINT:
		case WM_ACTIVATE:
		case WM_DRAWITEM:
		case WM_SIZE:
		{

			DWORD ret = CallWindowProc((WNDPROC)oldWndProc, hWnd, uMsg, wParam, lParam);
			ReSizeControl();
			return ret;
		}

		case WM_NOTIFY:
		{
			// send event my treeview -> Original treeview
			LPNM_TREEVIEW pntv = (LPNM_TREEVIEW)lParam;
			if (pntv->hdr.code == TVN_SELCHANGED) {
				if (pntv->hdr.hwndFrom == hMyTreeView) {
					wParam = (WPARAM)hTreeView;
					pntv->hdr.hwndFrom = hTreeView;
					pntv->hdr.idFrom = IDC_TREEVIEW;
					pntv->itemNew.lParam = pntv->itemNew.lParam;
				}

				// Modify the position of drawing items
				HWND hMID = FindWindowEx(hMainWindow, NULL, _T("MDIClient"), nullptr);
				HWND hMapWindow = FindWindowEx(hMID, NULL, _T("ScmDraft Map Window"), nullptr);
				if (hMapWindow) {
					RECT mapWindowRect;
					GetWindowRect(hMapWindow, &mapWindowRect);

					POINT mapWindowPosition;
					mapWindowPosition.x = (mapWindowRect.right - mapWindowRect.left) / 2;
					mapWindowPosition.y = (mapWindowRect.bottom - mapWindowRect.top) / 2;
					SendMessage(hMapWindow, WM_MOUSEMOVE, (WPARAM)0, (LPARAM)MAKELPARAM(mapWindowPosition.x, mapWindowPosition.y));
					SendMessage(hMapWindow, WM_MOUSEMOVE, (WPARAM)0, (LPARAM)MAKELPARAM(mapWindowPosition.x, mapWindowPosition.y));
				}
			}
		}

	}

	return CallWindowProc((WNDPROC)oldWndProc, hWnd, uMsg, wParam, lParam);
}

