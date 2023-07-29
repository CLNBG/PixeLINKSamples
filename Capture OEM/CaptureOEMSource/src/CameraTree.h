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

#pragma once


#include <map>


class PxLTreeNode; // forward decl.

// CCameraTree

class CCameraTree : public CTreeCtrl
{
	DECLARE_DYNAMIC(CCameraTree)

public:
	CCameraTree();
	virtual ~CCameraTree();
	void UpdateTopLevel(void);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnItemExpanding(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnItemExpanded(NMHDR *pNMHDR, LRESULT *pResult);

private:
	std::map<U32, PxLTreeNode*> itemMap;
};


