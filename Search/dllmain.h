#pragma once
#include "SCMDPlugin.h"
#include "treeview.h"
#include "debug.h"

const char* PluginName = "Search Plugin";
#define PLUGINVERSION		0x00000002

AllocRam scmd2_malloc;
DeAllocRam scmd2_free;
ReAllocRam scmd2_realloc;


HWND hMainWindow;
HINSTANCE hMainInstance;
LONG oldWndProc;

// My controls
HWND hTreeView, hMyTreeView, hEdit, hCheckBox;
CTreeView TreeView, MyTreeView;

// Control constant
const DWORD PLUGIN_START = 1500;
const DWORD IDC_EDIT = PLUGIN_START + 1;
const DWORD IDC_CHECKBOX = PLUGIN_START + 2;
const DWORD IDC_MYTREEVIEW = PLUGIN_START + 3;
const DWORD IDC_TREEVIEW = 0x479; // original control

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ReSizeControl();

