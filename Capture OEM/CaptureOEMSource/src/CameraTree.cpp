/**********************************************************************************************************
 * COPYRIGHT © 2003 PixeLINK.  ALL RIGHTS RESERVED.                                                       *
 *                                                                                                        *
 * Copyright Notice and Disclaimer of Liability:                                                          *
 *                                                                                                        *
 * Liability, distribution and use of this software is governed by the PixeLINK Software Agreement        *
 * distributed with this product.                                                                         *
 *                                                                                                        *
 * In short:                                                                                              *
 *                                                                                                        *
 * PixeLINK hereby explicitly prohibits any form of reproduction (with the express strict exception       *
 * for backup and archival purposes, which are allowed as stipulated within the License Agreement         *
 * for PixeLINK Software), modification, or distribution of this software or its associated               *
 * documentation unless explicitly specified in a written agreement signed by both parties.               *
 *                                                                                                        *
 * To the extent permitted by law, PixeLINK disclaims all other warranties or conditions of any kind,     *
 * either express or implied, except as stated in the PixeLINK Software Agreement or by written           *
 * consent of all parties.  Other written or oral statements by PixeLINK, its representatives, or         *
 * others do not constitute warranties or conditions of PixeLINK.  The Purchaser hereby agree to rely     *
 * on the software, associated hardware and documentation and results stemming from the use thereof       *
 * solely at their own risk.                                                                              *
 *                                                                                                        *
 * By using the products(s) associated with the software (or the software itself) the Purchaser           *
 * and/or user(s) agree(s) to abide by the terms and conditions set forth within this document, as        *
 * well as, respectively, any and all other documents issued by PixeLINK in relation to the               *
 * product(s).                                                                                            *
 *                                                                                                        *
 * For more information, review the PixeLINK Software Agreement distributed with this product or          *
 * contact sales@pixelink.com.                                                                            *
 *                                                                                                        *
 **********************************************************************************************************/

// CameraTree.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDemo.h"
#include ".\CameraTree.h"


class PxLTreeNode
{
public:
	PxLTreeNode() {}
	virtual ~PxLTreeNode() {}
	char const* ToString() { return m_str.c_str(); }
	void AddToTree(CTreeCtrl *pTree);
protected:
	virtual void PreInsert(TVINSERTSTRUCT &tvis) = 0;
	virtual void PostInsert(HTREEITEM hItem, CTreeCtrl *pTree) = 0;
	std::string m_str;
};

class PxLCameraTreeNode : public PxLTreeNode
{
public:
	PxLCameraTreeNode(U32 serialNumber)
	{
		m_serialNumber = serialNumber;
		char temp[10];
		sprintf(temp, "%u", serialNumber);
		m_str = temp;
	}
protected:
	void PreInsert(TVINSERTSTRUCT &tvis);
	void PostInsert(HTREEITEM hItem, CTreeCtrl *pTree);
	U32 m_serialNumber;
};

class PxLInfoTreeNode : public PxLTreeNode
{
};

class PxLDescriptorTreeNode : public PxLTreeNode
{
};


void PxLCameraTreeNode::PreInsert(TVINSERTSTRUCT &tvis)
{
	tvis.item.lParam = m_serialNumber;
}

void PxLCameraTreeNode::PostInsert(HTREEITEM hItem, CTreeCtrl *pTree)
{
	TVINSERTSTRUCT tvis;
	tvis.hParent = hItem;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_CHILDREN | TVIF_STATE | TVIF_TEXT;
	tvis.item.iImage = 0;
	tvis.item.iSelectedImage = 0;
	tvis.item.state = 0;
	tvis.item.stateMask = 0;
	tvis.item.lParam = 0;
	tvis.item.cChildren = 1;

	tvis.item.pszText = "Camera Info";
	HTREEITEM info = pTree->InsertItem(&tvis);

	tvis.item.pszText = "Descriptors";
	HTREEITEM desc = pTree->InsertItem(&tvis);
}

void PxLTreeNode::AddToTree(CTreeCtrl *pTree)
{
	TVINSERTSTRUCT tvis;
	tvis.hParent = TVI_ROOT;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_CHILDREN | TVIF_STATE | TVIF_TEXT;
	tvis.item.pszText = const_cast<LPSTR>(m_str.c_str());
	tvis.item.iImage = 0;
	tvis.item.iSelectedImage = 0;
	tvis.item.state = 0;
	tvis.item.stateMask = 0;
	tvis.item.lParam = 0;
	tvis.item.cChildren = 1;

	this->PreInsert(tvis); // virtual

	HTREEITEM cam = pTree->InsertItem(&tvis);

	this->PostInsert(cam, pTree);
}



// CCameraTree

IMPLEMENT_DYNAMIC(CCameraTree, CTreeCtrl)

CCameraTree::CCameraTree()
{
}

CCameraTree::~CCameraTree()
{
}


BEGIN_MESSAGE_MAP(CCameraTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemExpanded)
END_MESSAGE_MAP()



// CCameraTree message handlers


void CCameraTree::UpdateTopLevel(void)
{
	// Get List of cameras.
}


void CCameraTree::OnItemExpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	pNMTreeView->itemNew.lParam; // == Serial Number if node is a camera (top-level)

	*pResult = 0;
}


void CCameraTree::OnItemExpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);


	*pResult = 0;
}




























