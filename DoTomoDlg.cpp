/////////////////////////////////////////////////
// DoTomoDlg.cpp : file di implementazione
/////////////////////////////////////////////////

#include "stdafx.h"
#include "shlwapi.h"// For Directory Browser
#include "resource.h"
#include "NeuTomoUtil.h"
#include "DoTomoDlg.h"
#include "SelectShowDirDlg.h"


// finestra di dialogo CDoTomoDlg

IMPLEMENT_DYNAMIC(CDoTomoDlg, CDialog)

CDoTomoDlg::CDoTomoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDoTomoDlg::IDD, pParent)
  , bMedianSinograms(false)
  , FilterType(SHEPP_LOGAN)
{
	sNormDir         = AfxGetApp()->GetProfileString(__KEY__, _T("Normalized Directory"),  _T("."));
	sSliceDir        = AfxGetApp()->GetProfileString(__KEY__, _T("Slices Directory"),      _T("."));
  bMedianSinograms = AfxGetApp()->GetProfileInt(__KEY__, _T("Filter Sinograms"), 0) != 0;
  FilterType       = (FILTER_TYPES)AfxGetApp()->GetProfileInt(__KEY__, _T("Filter Type"), SHEPP_LOGAN);
}


CDoTomoDlg::~CDoTomoDlg() {
}


void
CDoTomoDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_NORMDIR, editNormDir);
  DDX_Control(pDX, IDC_SLICEDIR, editSliceDir);
  DDX_Control(pDX, IDC_FILTER_SINOGRAMS, buttonMedianSinograms);
  DDX_Control(pDX, IDC_SHEEP_LOGAN, buttonSheepLogan);
  DDX_Control(pDX, IDC_RAM_LAK, buttonRamLak);
  DDX_Control(pDX, IDC_SET_GEOMETRY, buttonSetGeomtery);
}


BEGIN_MESSAGE_MAP(CDoTomoDlg, CDialog)
  ON_BN_CLICKED(IDC_CHANGENORMDIR,    &CDoTomoDlg::OnBnClickedChangeNormDir)
  ON_BN_CLICKED(IDC_CHANGESLICEDIR,   &CDoTomoDlg::OnBnClickedChangeSliceDir)
  ON_BN_CLICKED(IDOK,                 &CDoTomoDlg::OnBnClickedOk)
  ON_BN_CLICKED(IDC_FILTER_SINOGRAMS, &CDoTomoDlg::OnBnClickedFilterSinograms)
  ON_BN_CLICKED(IDC_SHEPP_LOGAN,      &CDoTomoDlg::OnBnClickedSheppLogan)
  ON_BN_CLICKED(IDC_RAM_LAK,          &CDoTomoDlg::OnBnClickedRamLak)
  ON_BN_CLICKED(IDCANCEL,             &CDoTomoDlg::OnBnClickedCancel)
  ON_BN_CLICKED(IDC_SET_GEOMETRY,     &CDoTomoDlg::OnBnClickedSetGeometry)
END_MESSAGE_MAP()


bool
CDoTomoDlg::ChooseDir(bool bEnableCreate, CString& sDir, CString sName, CString sTitle, CString sHint) {
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


// gestori di messaggi CDoTomoDlg

int
CDoTomoDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CDialog::OnCreate(lpCreateStruct) == -1)
    return -1;

  return 0;
}


BOOL
CDoTomoDlg::OnInitDialog(){
  CDialog::OnInitDialog();

  sNormDir         = AfxGetApp()->GetProfileString(__KEY__, _T("Normalized Directory"),  sNormDir);
	sSliceDir        = AfxGetApp()->GetProfileString(__KEY__, _T("Slices Directory"),      sSliceDir);
  bMedianSinograms = AfxGetApp()->GetProfileInt(__KEY__, _T("Filter Sinograms"), 0) != 0;
  FilterType       = (FILTER_TYPES)AfxGetApp()->GetProfileInt(__KEY__, _T("Filter Type"), SHEPP_LOGAN);
  
  editNormDir.SetWindowText(sNormDir);
  editSliceDir.SetWindowText(sSliceDir);

  if(bMedianSinograms)
    buttonMedianSinograms.SetCheck(BST_CHECKED);
  else
    buttonMedianSinograms.SetCheck(BST_UNCHECKED);

  if(FilterType == SHEPP_LOGAN) 
    buttonSheepLogan.SetCheck(BST_CHECKED);
  else
    buttonRamLak.SetCheck(BST_CHECKED);
  return TRUE;  // return TRUE unless you set the focus to a control
}


void
CDoTomoDlg::OnBnClickedOk() {
  if(!UpdateData(true)) {
    MessageBeep(-1);
    return;
  }
  if(sNormDir  != _T("")) AfxGetApp()->WriteProfileString(__KEY__, _T("Normalized Directory"),  sNormDir);
  if(sSliceDir != _T("")) AfxGetApp()->WriteProfileString(__KEY__, _T("Slices Directory"),      sSliceDir);
  AfxGetApp()->WriteProfileInt(__KEY__, _T("Filter Sinograms"), bMedianSinograms);
  AfxGetApp()->WriteProfileInt(__KEY__, _T("Filter Type"), (int)FilterType);
  OnOK();
}


void 
CDoTomoDlg::OnBnClickedChangeNormDir() {
  CString sNewDir = sNormDir;
  if(ChooseDir(true, sNewDir, _T("*.*"), _T("Choose Output Directory"), _T("Select or Create the Directory Where to Write the Normalized Projections")))
    sNormDir = sNewDir;
  editNormDir.SetWindowText(sNormDir);
  UpdateData(false);
}


void
CDoTomoDlg::OnBnClickedChangeSliceDir() {
  CString sNewDir = sSliceDir;
  if(ChooseDir(true, sNewDir, _T("*.*"), _T("Choose Output Directory"), _T("Select or Create the Directory Where to Write the Tomography Slices")))
    sSliceDir = sNewDir;
  editSliceDir.SetWindowText(sSliceDir);
  UpdateData(false);
}


CString
CDoTomoDlg::GetNormDir() {
  return sNormDir;
}


CString
CDoTomoDlg::GetSliceDir() {
  return sSliceDir;
}


void
CDoTomoDlg::OnBnClickedFilterSinograms() {
  bMedianSinograms = buttonMedianSinograms.GetCheck()==BST_CHECKED;
  UpdateData(false);
}


bool
CDoTomoDlg::GetFilterSinograms() {
  return bMedianSinograms;
}


FILTER_TYPES
CDoTomoDlg::GetFilterType() {
  return FilterType;
}


void
CDoTomoDlg::OnBnClickedSheppLogan() {
  FilterType = SHEPP_LOGAN;
}


void
CDoTomoDlg::OnBnClickedRamLak() {
  FilterType = RAM_LAK;
}


BOOL
CDoTomoDlg::UpdateData(BOOL bSaveAndValidate) {
  return CDialog::UpdateData(bSaveAndValidate);
}


void
CDoTomoDlg::OnBnClickedCancel() {
  OnCancel();
}


void
CDoTomoDlg::OnBnClickedSetGeometry() {
  if(GeometryDlg.DoModal() != IDOK) return;
}


BEAM_GEOMETRY
CDoTomoDlg::GetBeamGeometry() {
  return GeometryDlg.GetBeamGeometry();
}


float 
CDoTomoDlg::GetSourceSampleDistance() {
  return GeometryDlg.GetSourceSampleDistance() ;
}


float
CDoTomoDlg::GetSourceDetectorDistance() {
  return GeometryDlg.GetSourceDetectorDistance();
}


float
CDoTomoDlg::GetVerticalFOV() {
  return GeometryDlg.GetVerticalFOV();
}


float
CDoTomoDlg::GetHorizzontalFOV() {
  return GeometryDlg.GetHorizzontalFOV();
}


int
CDoTomoDlg::GetVolXSize() {
  return GeometryDlg.GetVolXSize();
}


int
CDoTomoDlg::GetVolYSize() {
  return GeometryDlg.GetVolYSize();
}


int
CDoTomoDlg::GetVolZSize() {
  return GeometryDlg.GetVolZSize();
}

