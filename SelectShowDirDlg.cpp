// SelectShowDirDlg.cpp : file di implementazione
//

#include "stdafx.h"
#include "shlwapi.h"// For Directory Browser
#include "resource.h"
#include "NeuTomoUtil.h"
#include "SelectShowDirDlg.h"


// finestra di dialogo CSelectShowDirDlg

IMPLEMENT_DYNAMIC(CSelectShowDirDlg, CDialog)

CSelectShowDirDlg::CSelectShowDirDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectShowDirDlg::IDD, pParent)
  , sShowDir(_T("."))
{
	sShowDir = AfxGetApp()->GetProfileString(__KEY__, _T("Show Directory"),  sShowDir);
}


CSelectShowDirDlg::~CSelectShowDirDlg() {
}


void
CSelectShowDirDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_NORMDIR, editShowDir);
}


BEGIN_MESSAGE_MAP(CSelectShowDirDlg, CDialog)
  ON_BN_CLICKED(IDC_CHANGESHOWDIR, &CSelectShowDirDlg::OnBnClickedChangeShowDir)
  ON_BN_CLICKED(IDOK, &CSelectShowDirDlg::OnBnClickedOk)
END_MESSAGE_MAP()


bool
CSelectShowDirDlg::ChooseDir(bool bEnableCreate, CString& sDir, CString sName, CString sTitle, CString sHint) {
  BROWSEINFO bi = {0};
  tBrowseData BrowseData;
  BrowseData.sStartingDir = sDir;
  BrowseData.sWindowTitle = sTitle;

  bi.hwndOwner      = this->GetSafeHwnd();
  bi.pidlRoot       = NULL;
  bi.pszDisplayName = NULL;
  bi.lpszTitle      = sHint;
  if(bEnableCreate)
    bi.ulFlags      = BIF_RETURNFSANCESTORS|BIF_NEWDIALOGSTYLE|BIF_RETURNONLYFSDIRS|BIF_VALIDATE;
  else
    bi.ulFlags      = BIF_RETURNFSANCESTORS|BIF_NEWDIALOGSTYLE|BIF_NONEWFOLDERBUTTON|BIF_RETURNONLYFSDIRS|BIF_VALIDATE;
  bi.lpfn           = BrowseCallbackProc;
  bi.lParam         = (LPARAM)&BrowseData;    // param for the callback
  // do show the dialog
  LPITEMIDLIST pidl = ::SHBrowseForFolder(&bi);
  if(pidl) {
    TCHAR szPath[MAX_PATH+1];
    SHGetPathFromIDList(pidl, szPath);
    sDir = szPath;
    sName = sDir.Mid(sDir.ReverseFind('\\')+1);
    IMalloc* pMalloc = NULL; 
    if(SUCCEEDED(SHGetMalloc(&pMalloc))) { 
      pMalloc->Free((PVOID)pidl); 
      pMalloc->Release(); 
    } else {
      AfxMessageBox(_T("Error: SHGetMalloc() Failed !"));
    }
    return true;
  }
  return false;
}


CString
CSelectShowDirDlg::GetShowDir() {
  return sShowDir;
}


// gestori di messaggi CSelectShowDirDlg

void
CSelectShowDirDlg::OnBnClickedChangeShowDir() {
  CString sNewDir = sShowDir;
  if(ChooseDir(false, sNewDir, _T("*.*"), _T("Choose Input Directory"), _T("Select the Directory Containing the Slices")))
    sShowDir = sNewDir;
  editShowDir.SetWindowText(sShowDir);
  UpdateData(false);
}


BOOL
CSelectShowDirDlg::OnInitDialog() {
  CDialog::OnInitDialog();

	sShowDir = AfxGetApp()->GetProfileString(__KEY__, _T("Show Directory"), sShowDir);
  editShowDir.SetWindowText(sShowDir);

  return TRUE;  // return TRUE unless you set the focus to a control
  // ECCEZIONE: le pagine delle proprietà OCX devono restituire FALSE
}


void
CSelectShowDirDlg::OnBnClickedOk() {
  if(!UpdateData(true)) {
    MessageBeep(-1);
    return;
  }
  if(sShowDir  != _T("")) AfxGetApp()->WriteProfileString(__KEY__, _T("Show Directory"),  sShowDir);

  OnOK();
}
