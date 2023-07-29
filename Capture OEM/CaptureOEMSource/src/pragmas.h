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



// Names truncated in debug info - happens a lot when using STL - I don't care.
#pragma warning(disable:4786)

// I like to be warned about using = instead of == in a conditional (I screw up sometimes).
// Set warning level to 3 instead of 4.
#pragma warning(3:4706)

// Warning that you need to export standard library classes from your DLL. It's a vicious lie! Don't be fooled!
#pragma warning(disable:4251)

// Warning about comparing a signed to an unsigned value (with <, >, ==, !=, etc).
#pragma warning(disable:4018)