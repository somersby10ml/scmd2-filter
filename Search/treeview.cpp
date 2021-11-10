#include "pch.h"
#include "treeview.h"


CTreeView::CTreeView() {

}

CTreeView::CTreeView(HWND hTreeview) {
	hTreeView_ = hTreeview;
}

void CTreeView::SetTreeView(HWND hTreeview) {
	hTreeView_ = hTreeview;
}

void CTreeView::ReadAllItem() {
	data_.clear();
	HTREEITEM root = TreeView_GetNextItem(hTreeView_, NULL, TVGN_ROOT);
	data_ = ReadItemRecursive(root);
}

std::vector<cUnit> CTreeView::ReadItemRecursive(HTREEITEM node) {
	std::vector<cUnit> vUnit;
	do {
		cUnit unit;
		unit.text = ReadString(node);
		unit.lParam = ReadLParam(node);

		HTREEITEM child = TreeView_GetNextItem(hTreeView_, node, TVGN_CHILD);
		if (child)
			unit.child = ReadItemRecursive(child);

		vUnit.push_back(unit);

		node = TreeView_GetNextItem(hTreeView_, node, TVGN_NEXT);
	} while (node);

	return vUnit;
}

void CTreeView::SetTreeViewItemRecursive(HWND tree, std::vector<cUnit> node, HTREEITEM parent) {
	for (std::vector<cUnit>::iterator b = node.begin(); b != node.end(); b++) {
		TVINSERTSTRUCT tvInsertStruct;
		tvInsertStruct.hParent = parent;
		tvInsertStruct.hInsertAfter = TVI_LAST;
		tvInsertStruct.item.pszText = (LPWSTR)b->text.c_str();
		tvInsertStruct.item.cchTextMax = b->text.size();
		tvInsertStruct.item.mask = TVIF_TEXT | TVIF_PARAM;
		tvInsertStruct.item.lParam = b->lParam;
		HTREEITEM hRootItem = TreeView_InsertItem(tree, &tvInsertStruct);
		if (b->child.size()) {
			SetTreeViewItemRecursive(tree, b->child, hRootItem);
		}

	}
}

void CTreeView::FilterDataRecursive(std::vector<cUnit>& node, std::wstring text) {
	for (std::vector<cUnit>::iterator b = node.begin(); b != node.end(); ) {

		if (b->child.size()) {
			FilterDataRecursive(b->child, text);
		}

		bool bDelete = false;
		if (b->child.size() == 0) {
			std::wstring::iterator it = std::search(
				b->text.begin(), b->text.end(),
				text.begin(), text.end(),
				[](TCHAR ch1, TCHAR ch2) { return std::toupper(ch1) == std::toupper(ch2); }
			);
			if (it == b->text.end()) {
				b = node.erase(b);
				bDelete = true;
			}
		}

		if (!bDelete) ++b;
		
	}
}


std::vector<cUnit> CTreeView::AllExpandRecursive(HTREEITEM node) {
	std::vector<cUnit> vUnit;
	do {
		HTREEITEM child = TreeView_GetNextItem(hTreeView_, node, TVGN_CHILD);
		if (child)
			AllExpandRecursive(child);
		TreeView_Expand(hTreeView_, node, TVE_EXPAND);
		node = TreeView_GetNextItem(hTreeView_, node, TVGN_NEXT);
	} while (node);
	return vUnit;
}

tstring CTreeView::ReadString(HTREEITEM node) {
	TCHAR text[50] = { 0, };
	TVITEM item = { 0 };
	item.hItem = node;
	item.cchTextMax = _countof(text);
	item.pszText = text;
	item.mask = TVIF_HANDLE | TVIF_TEXT;
	TreeView_GetItem(hTreeView_, &item);
	return text;
}

DWORD CTreeView::ReadLParam(HTREEITEM node) {
	TVITEM item = { 0 };
	item.hItem = node;
	item.mask = TVIF_HANDLE | TVIF_PARAM;
	TreeView_GetItem(hTreeView_, &item);
	return item.lParam;
}

void CTreeView::AllExpand(HTREEITEM node) {
	HTREEITEM root = TreeView_GetNextItem(hTreeView_, NULL, TVGN_ROOT);
	AllExpandRecursive(root);
}

void CTreeView::FilterData(std::wstring text) {
	FilterDataRecursive(data_, text);
}

void CTreeView::SetTreeViewItem(CTreeView& TreeView) {
	SetTreeViewItemRecursive(hTreeView_, TreeView.data_);
}

void CTreeView::Show() {
	if (!IsWindowVisible(hTreeView_))
		ShowWindow(hTreeView_, SW_SHOW);
}

void CTreeView::Hide() {
	if (IsWindowVisible(hTreeView_))
		ShowWindow(hTreeView_, SW_HIDE);
}

void CTreeView::DeleteAll() {
	TreeView_DeleteAllItems(hTreeView_);
}
