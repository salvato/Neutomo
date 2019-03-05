/*******************************************************************************
File        :   WindowTip.cpp
Description :   This Contains the module defination of properties of CTipInfo
                and CWindowTip class that used to display the tips

Created     :   Oct 16, 2001
Author      :   Rakesh Kumar Agrawal
e-mail      :   onlyrakesh@hotmail.com

Compiler    :   Visual C++ 6.0
********************************************************************************/

#include "stdafx.h"
#include "WindowTip.h"
#include ".\windowtip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/******************************************************************************
                        Description of Class CTipInfo
******************************************************************************/

/*---------------------------------------------------------
Method      :   CTipInfo::CTipInfo
Description :   Default Constructor for CTipInfo class.

Return Type :   None    
Parameter   :   None
-----------------------------------------------------------*/
CTipInfo::CTipInfo() {
  m_rectRegion = CRect(0, 0, 1, 1) ;  
  m_strTipText = _T("") ;
}



/*---------------------------------------------------------
Method      :   CTipInfo::CTipInfo
Description :   Parametized Constructor for CTipInfo class
                To initialize its variables

Return Type :   None    
Parameter   :   1.  CString         // To Set Tip Text
                2.  CRect           // To Set Rectanguler Region
-----------------------------------------------------------*/
CTipInfo::CTipInfo(CRect rectRegion, CString strText) {
  m_rectRegion = rectRegion ;
  m_strTipText = strText ;
}


CTipInfo::~CTipInfo() {
  m_strTipText = _T("");
}


/*---------------------------------------------------------
Method      :   CTipInfo::SetText
Description :   Used to set the TipText for a Tip

Return Type :   void
Parameter   :   1.  CString         // Text of Tip
-----------------------------------------------------------*/
void
CTipInfo::SetText(CString strText) {
  m_strTipText = strText ;
}


/*---------------------------------------------------------
Method      :   CTipInfo::SetRegion
Description :   Used to set the Rectanguler area for Tip

Return Type :   void   
Parameter   :   1.  CRect           // Secifies the Rectangle
-----------------------------------------------------------*/
void
CTipInfo::SetRegion(CRect rectRegion) {
  m_rectRegion = rectRegion;
}


/*---------------------------------------------------------
Method      :   CTipInfo::GetText
Description :   Gives the Tip Text

Return Type :   CString
Parameter   :   None
-----------------------------------------------------------*/
CString
CTipInfo::GetText() {
  return m_strTipText;
}


/*---------------------------------------------------------
Method      :   CTipInfo::GetRegion
Description :   Gives the Tips Rectanguler Region

Return Type :   CRect
Parameter   :   None 
-----------------------------------------------------------*/
CRect
CTipInfo::GetRegion() {
  return m_rectRegion;
}



/******************************************************************************
                        Description of Class CTipInfo
******************************************************************************/
/*---------------------------------------------------------
                        Message Map    
-----------------------------------------------------------*/
BEGIN_MESSAGE_MAP(CWindowTip, CWnd)
	//{{AFX_MSG_MAP(CWindowTip)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/*---------------------------------------------------------
Method      :   CWindowTip::CWindowTip
Description :   Default Constructor for CWindowTip class.

Return Type :   None    
Parameter   :   None 
-----------------------------------------------------------*/
CWindowTip::CWindowTip() {
  m_nCurrentTip = -1;
  m_bDisplayed  = FALSE;
}


/*---------------------------------------------------------
Method      :   CWindowTip::CWindowTip
Description :   Destructor for CWindowTip class.

Return Type :   None    
Parameter   :   None 
-----------------------------------------------------------*/
CWindowTip::~CWindowTip() {
  m_arrToolTips.RemoveAll();
}


/*---------------------------------------------------------
Method      :   CWindowTip::CreateTipWnd
Description :   Creates Tool Tip Window

Return Type :   void 
Parameter   :   1.  CWnd*       // Parent Window
-----------------------------------------------------------*/
void
CWindowTip::CreateTipWnd(CWnd* pParentWnd) {
  // First Create a child window of given window
  CWnd::CreateEx(NULL, NULL, _T("ITS ME"), WS_CHILD ,
                 0, 0, 10, 10, pParentWnd->GetSafeHwnd(), NULL);
  // then load this in memory without showing it to user
  ShowWindow(SW_HIDE);
}



/*---------------------------------------------------------
Method      :   CWindowTip::ShowTip
Description :   Used by application user to show the tips

Return Type :   void 
Parameter   :   1.  CPoint      // Specifies current Mouse Position
-----------------------------------------------------------*/
void
CWindowTip::ShowTip(CPoint point) {
  if(!GetSafeHwnd())
    return ;
  // If Mouse is moved or no tip is current then
  // Have to search for other tips which can be displayed
  if(m_pntPrevPos != point || m_nCurrentTip != -1) {
    if(m_bDisplayed)
      HideTip(); // Hide Tip if it is displyed
    // Traverse the Tip list and select one
    // If mouse is in Tips Region
    CRect t_tmpRect ;
    for(int i=0; i < m_arrToolTips.GetSize() ; i++) {
      t_tmpRect = m_arrToolTips.GetAt(i).GetRegion();
      if((point.x < t_tmpRect.right && point.x > t_tmpRect.left) &&
         (point.y < t_tmpRect.bottom && point.y > t_tmpRect.top)) {
        m_nCurrentTip = i;
        m_pntPrevPos = point;
        m_bDisplayed = TRUE;
        ShowWindow(SW_SHOWNOACTIVATE);
        UpdateWindow();
        break;
      } // if((point.x < t_tmpRect.right && point.x > t_tmpRect.left) &&(point.y < t_tmpRect.bottom && point.y > t_tmpRect.top))
    } // for(int i=0; i < m_arrToolTips.GetSize() ; i++)   
  } // if(m_pntPrevPos != point || m_nCurrentTip != -1)
}



/*---------------------------------------------------------
Method      :   CWindowTip::AddTip
Description :   Used by application user to add the tips

Return Type :   void 
Parameter   :   1.  CTipInfo    // TipInfo object
-----------------------------------------------------------*/
INT_PTR
CWindowTip::AddTip(CTipInfo newTip) {
  return m_arrToolTips.Add(newTip);
}


/*---------------------------------------------------------
Method      :   CWindowTip::GetTipInfo
Description :   Used by application user to retrieve a tip

Return Type :   CTipInfo* 
Parameter   :   1.  pointer    // TipInfo pointer object
-----------------------------------------------------------*/
CTipInfo*
CWindowTip::GetTipInfo(INT_PTR pointer) {
  return &m_arrToolTips.ElementAt(pointer);
}


void
SetTipInfo(INT_PTR pointer, CTipInfo TipInfo) {

}



/*---------------------------------------------------------
Method      :   CWindowTip::HideTip
Description :   To hide the displayed tip

Return Type :   void 
Parameter   :   None 
-----------------------------------------------------------*/
void
CWindowTip::HideTip() {
  m_bDisplayed = FALSE;
  ShowWindow(SW_HIDE);
}


/*---------------------------------------------------------
Method      :   CWindowTip::OnPaint
Description :   To Paint the Tip window

Return Type :   void    
Parameter   :   None
-----------------------------------------------------------*/
void
CWindowTip::OnPaint() {
  CPaintDC dc(this); // device context for painting
  // First create the Pen, Brush and Font for tip
  CPen t_tipPen;
  CFont t_tipFont;
  CBrush t_tipBrush;

  t_tipPen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
  t_tipFont.CreateFont(15, 0, 0, 0, FW_REGULAR, 0, 0, 0, 0, 0, 0, 0, 0, _T("MS Sans Serif"));
  t_tipBrush.CreateSolidBrush(RGB(255, 255, 192));

  // Select them
  CPen *oldPen = dc.SelectObject(&t_tipPen);
  CFont *oldFont = dc.SelectObject(&t_tipFont);
  CBrush *oldBsh = dc.SelectObject(&t_tipBrush);

  // Draw a rectangle to give background color
  CRect rect;
  GetClientRect(&rect);
  dc.Rectangle(&rect);

  // Set Transparent mode so that it will not show fonts back color
  dc.SetBkMode(TRANSPARENT);

  // Now display the tip
  CString str = _T(" ");
 	if(m_bDisplayed) {
    str = m_arrToolTips[m_nCurrentTip].GetText() ;
    dc.TextOut(2, 0, str);   
  }

  // ReSize the tip window according to tip text
  CSize stringSize = dc.GetTextExtent(str);
  CRect RectParent;
  GetParent()->GetClientRect(&RectParent);
  rect.top = m_pntPrevPos.y - stringSize.cy;
  rect.left = m_pntPrevPos.x; 
  rect.bottom = rect.top + stringSize.cy;
  rect.right = rect.left + stringSize.cx;
  // Give some extra widht and height to the window
  rect.InflateRect(2,1);
  rect.OffsetRect(12, -12);

  if(rect.right > RectParent.right) {
    rect.OffsetRect(-rect.Width()-12, 0);
  } else {

  }

  if(rect.top < RectParent.top) {
    rect.OffsetRect(0, rect.Height()+22);
  } else {

  }

  MoveWindow(&rect);

  // Select Old Pen, Font and Brush
  dc.SelectObject(oldBsh);
  dc.SelectObject(oldPen);
  dc.SelectObject(oldFont);
}

