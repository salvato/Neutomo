/*******************************************************************************
File        :   WindowTip.h
Description :   This Contains the module by which one can Specify many ToolTips
                for a single window according to specified rectanguler regions
                of window.

Created     :   Oct 16, 2001
Author      :   Rakesh Kumar Agrawal
e-mail      :   onlyrakesh@hotmail.com

Compiler    :   Visual C++ 6.0
********************************************************************************/
#ifndef _WINDOWTIP_H_
#define _WINDOWTIP_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "afxtempl.h"
const short TIP_TIMER_ID = 10001;


/*---------------------------------------------------------------------------
 Class      :   CTipInfo
 Description:   An Object of this class stores a Tip info and Used in
                Displying the tip

 Created    :   Oct 16, 2001
 Author     :   Rakesh Kumar Agrawal
 e-mail     :   onlyrakesh@hotmail.com

 Revisions  :   In Future It will contain a link also to show some 
                descriptive help
----------------------------------------------------------------------------*/
class CTipInfo {
// --------------------------------
//      Properties 
// --------------------------------

  // <--- Constructors & Distructors --->
public:
  CTipInfo();                       //  Default Constructor.
  CTipInfo(CRect rectRegion,        //  Parameterized Constructor that directly sepcifies
           CString strText);        //  Region and TipText.
  ~CTipInfo();                      //  Destructor
    
  // <--- Mutetors & Inspectors --->
public:
  void SetText(CString strText);    //  Sets the Text for Tip
  void SetRegion(CRect rectRegion); //  Specifies the Region for Tip

  CString GetText();                //  Gets the Text of Tip
  CRect GetRegion();                //  Gets the Ractanguler Region of Tip


// --------------------------------
//      Attributes
// --------------------------------
private:
  CRect m_rectRegion;               //  Stores the Recanguler Region of Tip
  CString m_strTipText;             //  Stores the Tip text
};  // End of CTipInfo Class defination

typedef CArray<CTipInfo, CTipInfo> arrTipInfo;  // A Collection of Tip Infos



// Get NONCLIENTMETRICS info: ctor calls SystemParametersInfo.
//
class CNonClientMetrics : public NONCLIENTMETRICS {
public:
  CNonClientMetrics() {
    cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS,0,this,0);
  }
};



/*---------------------------------------------------------------------------
 Class      :   CWindowTip
 Description:   Encapsulates all complexities of Hiding and displaying 
                of Tip, Resizing of Tip Window and Used To Display the 
                Tip.

 Created    :   Oct 16, 2001
 Author     :   Rakesh Kumar Agrawal
 e-mail     :   onlyrakesh@hotmail.com

 Revisions  :   In Future It will show Tips on Regions instead of 
                a Recatnguler area.
                Options to Change the Font, Color and all
----------------------------------------------------------------------------*/
class CWindowTip : public CWnd
{
// --------------------------------
//      Properties 
// --------------------------------

  // <--- Constructors & Distructors --->
public:
	CWindowTip();                       //  Default Constructor.
	virtual ~CWindowTip();

  // <--- Facilitators --->
public:
  void    CreateTipWnd(CWnd * pParentWnd);    //  To create Tip Window
  void    ShowTip(CPoint point);              //  To show Tip
  INT_PTR AddTip(CTipInfo newTip);            //  To Add a New Tip to window Tips
  CTipInfo* GetTipInfo(INT_PTR pointer);
  void SetTipInfo(INT_PTR pointer, CTipInfo TipInfo);
  void HideTip();                     //  To hide the Tip Window

  // <---- Generated message map functions --->
protected:
	//{{AFX_MSG(CWindowTip)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


// --------------------------------
//      Attributes
// --------------------------------

private:
  arrTipInfo  m_arrToolTips;          //  To store the collection of Tips
  short       m_nCurrentTip;          //  To keep treak of Current Tip
  CPoint      m_pntPrevPos;           //  To manage Mouse Moves
  BOOL        m_bDisplayed;           //  Show that Tips is displayed or not

}; // End of CWindowTip Class defination

#endif // _WINDOWTIP_H_
