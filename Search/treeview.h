#pragma once

class cUnit {
public:
	std::wstring text;
	DWORD lParam;
	std::vector<cUnit> child;
};


class CTreeView {
public:
	CTreeView();
	CTreeView(HWND hTreeview);

	void SetTreeView(HWND hTreeview);

	
	/// <summary>
	/// Treeview read all item to object
	/// </summary>
	void ReadAllItem();

	/// <summary>
	/// Treeview item all expand
	/// </summary>
	/// <param name="node"></param>
	void AllExpand(HTREEITEM node = NULL);

	/// <summary>
	/// filter data from read item
	/// </summary>
	/// <param name="text"></param>
	void FilterData(std::wstring text);


	/// <summary>
	/// Treeview set item
	/// </summary>
	/// <param name="TreeView"></param>
	void SetTreeViewItem(CTreeView& TreeView);

	/// <summary>
	/// Treeview show display
	/// </summary>
	void Show();

	/// <summary>
	/// Treeview hide display
	/// </summary>
	void Hide();

	/// <summary>
	/// Treeview delete all item
	/// </summary>
	void DeleteAll();

	/*tstring text;
	DWORD lParam;
	std::vector<cUnit> child;*/
private:

	/// <summary>
	/// Treeview read string from handle
	/// </summary>
	/// <param name="node"></param>
	/// <returns></returns>
	tstring ReadString(HTREEITEM node);

	/// <summary>
	/// Treeview read param from handle
	/// </summary>
	/// <param name="node"></param>
	/// <returns></returns>
	DWORD ReadLParam(HTREEITEM node);


	void SetTreeViewItemRecursive(HWND tree, std::vector<cUnit> node, HTREEITEM parent = nullptr);
	void FilterDataRecursive(std::vector<cUnit>& node, std::wstring text);
	std::vector<cUnit> AllExpandRecursive(HTREEITEM node = NULL);
	std::vector<cUnit> ReadItemRecursive(HTREEITEM node);
	HWND hTreeView_ = nullptr;
	std::vector<cUnit> data_;
};



