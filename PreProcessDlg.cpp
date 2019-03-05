/////////////////////////////////////////////////
// PreProcessDlg.cpp : file di implementazione //
/////////////////////////////////////////////////

#include "stdafx.h"
#include "shlwapi.h"// For Directory Browser
#include "resource.h"
#include "NeuTomoUtil.h"
#include "PreProcessDlg.h"


// finestra di dialogo CPreProcessDlg

IMPLEMENT_DYNAMIC(CPreProcessDlg, CDialog)

CPreProcessDlg::CPreProcessDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CPreProcessDlg::IDD, pParent) 
  , bFilterDark(false)
  , bUseDark(false)
  , bFilterBeam(false)
  , bUseBeam(false)
  , bCorrectAndorBug(false)
{

	sProjectionDir = AfxGetApp()->GetProfileString(__KEY__, _T("Projections Directory"), sProjectionDir);
	sNormDir       = AfxGetApp()->GetProfileString(__KEY__, _T("Normalized Directory"),  sNormDir);
  sBeamDir       = AfxGetApp()->GetProfileString(__KEY__, _T("Beam Directory"),        sBeamDir);
  sDarkDir       = AfxGetApp()->GetProfileString(__KEY__, _T("Dark Directory"),        sDarkDir);

  bUseDark         = AfxGetApp()->GetProfileInt(__KEY__, _T("Use Dark"), 0)          != 0;
  bFilterDark      = AfxGetApp()->GetProfileInt(__KEY__, _T("Filter Dark"), 0)       != 0;
  bUseBeam         = AfxGetApp()->GetProfileInt(__KEY__, _T("Use Beam"), 0)          != 0;
  bFilterBeam      = AfxGetApp()->GetProfileInt(__KEY__, _T("Filter Beam"), 0)       != 0;
  bCorrectAndorBug = AfxGetApp()->GetProfileInt(__KEY__, _T("Correct Andor Bug"), 0) != 0;

  ProjectionFormat = AfxGetApp()->GetProfileInt(__KEY__, _T("Projection Format"), PROJECTION_FORMAT_FITS);

  fAngularStep     = float(_tstof(AfxGetApp()->GetProfileString(__KEY__, _T("Angular Step"), _T("0.9"))));
  fPostThreshold   = float(_tstof(AfxGetApp()->GetProfileString(__KEY__, _T("Post Threshold"), _T("3.0"))));

  bPreMedian       = AfxGetApp()->GetProfileInt(__KEY__, _T("Pre Median Filter"), 0)    != 0;
  bPostMedian      = AfxGetApp()->GetProfileInt(__KEY__, _T("Post Median Filter"), 0)   != 0;
  bPostGaussian    = AfxGetApp()->GetProfileInt(__KEY__, _T("Post Gaussian Filter"), 0) != 0;

}


CPreProcessDlg::~CPreProcessDlg() {
}


void
CPreProcessDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_DARKDIR, editDarkDir);
  DDX_Control(pDX, IDC_BEAMDIR, editBeamDir);
  DDX_Control(pDX, IDC_PROJDIR, editProjDir);
  DDX_Control(pDX, IDC_NORMDIR, editNormDir);
  DDX_Control(pDX, IDC_USEDARK, buttonUseDark);
  DDX_Control(pDX, IDC_CHANGEDARKDIR, buttonChangeDarkDir);
  DDX_Control(pDX, IDC_POSTMEDAIN, buttonPostMedianFilter);
  DDX_Control(pDX, IDC_PREMEDIAN, buttonPreMedianFilter);
  DDX_Control(pDX, IDC_POSTGAUSSIAN, buttonPostGaussianFilter);
  DDX_Control(pDX, IDC_ANGULARSTEP, editAngularStep);
  DDX_Control(pDX, IDC_POST_THRESHOLD, editPostThreshold);
  DDX_Control(pDX, IDC_FILTER_BEAM, buttonFilterBeam);
  DDX_Control(pDX, IDC_USEBEAM, buttonUseBeam);
  DDX_Control(pDX, IDC_FILTER_DARK, buttonFilterDark);
  DDX_Control(pDX, IDC_CHANGEBEAMDIR, buttonChangeBeamDir);
  DDX_Control(pDX, IDC_ANDOR_BUG, buttonAndorBug);
}


BEGIN_MESSAGE_MAP(CPreProcessDlg, CDialog)
  ON_WM_CREATE()
  ON_BN_CLICKED(IDC_CHANGEDARKDIR,     &CPreProcessDlg::OnBnClickedChangeDarkDir)
  ON_BN_CLICKED(IDC_CHANGEBEAMDIR,     &CPreProcessDlg::OnBnClickedChangeBeamDir)
  ON_BN_CLICKED(IDC_CHANGEPROJDIR,     &CPreProcessDlg::OnBnClickedChangeProjDir)
  ON_BN_CLICKED(IDC_CHANGENORMDIR,     &CPreProcessDlg::OnBnClickedChangeNormDir)
  ON_BN_CLICKED(IDC_USEDARK,           &CPreProcessDlg::OnBnClickedUseDark)
  ON_BN_CLICKED(IDC_PREMEDIAN,         &CPreProcessDlg::OnBnClickedPreMedian)
  ON_BN_CLICKED(IDC_POSTMEDAIN,        &CPreProcessDlg::OnBnClickedPostMedain)
  ON_BN_CLICKED(IDOK,                  &CPreProcessDlg::OnBnClickedOk)
  ON_BN_CLICKED(IDC_FILTER_BEAM,       &CPreProcessDlg::OnBnClickedFilterBeam)
  ON_BN_CLICKED(IDC_USEBEAM,           &CPreProcessDlg::OnBnClickedUseBeam)
  ON_BN_CLICKED(IDC_FILTER_DARK,       &CPreProcessDlg::OnBnClickedFilterDark)
  ON_BN_CLICKED(IDC_ANDOR_BUG,         &CPreProcessDlg::OnBnClickedAndorBug)
END_MESSAGE_MAP()


BOOL 
CPreProcessDlg::UpdateData(BOOL bSaveAndValidate) {
  if(!bSaveAndValidate) return CDialog::UpdateData(bSaveAndValidate);
  return CDialog::UpdateData(bSaveAndValidate);
}


bool
CPreProcessDlg::ChooseDir(bool bEnableCreate, CString& sDir, CString sName, CString sTitle, CString sHint) {
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


////////////////////////////////////////
// gestori di messaggi CPreProcessDlg //
////////////////////////////////////////


int
CPreProcessDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CDialog::OnCreate(lpCreateStruct) == -1)
    return -1;

  return 0;
}


BOOL
CPreProcessDlg::OnInitDialog(){
  CDialog::OnInitDialog();

  CString sString;

	sProjectionDir = AfxGetApp()->GetProfileString(__KEY__, _T("Projections Directory"), sProjectionDir);
	sNormDir       = AfxGetApp()->GetProfileString(__KEY__, _T("Normalized Directory"),  sNormDir);
  sBeamDir       = AfxGetApp()->GetProfileString(__KEY__, _T("Beam Directory"),        sBeamDir);
  sDarkDir       = AfxGetApp()->GetProfileString(__KEY__, _T("Dark Directory"),        sDarkDir);

  bUseDark         = AfxGetApp()->GetProfileInt(__KEY__, _T("Use Dark"), 0)          != 0;
  bFilterDark      = AfxGetApp()->GetProfileInt(__KEY__, _T("Filter Dark"), 0)       != 0;
  bUseBeam         = AfxGetApp()->GetProfileInt(__KEY__, _T("Use Beam"), 0)          != 0;
  bFilterBeam      = AfxGetApp()->GetProfileInt(__KEY__, _T("Filter Beam"), 0)       != 0;
  bCorrectAndorBug = AfxGetApp()->GetProfileInt(__KEY__, _T("Correct Andor Bug"), 0) != 0;

  ProjectionFormat = AfxGetApp()->GetProfileInt(__KEY__, _T("Projection Format"), PROJECTION_FORMAT_BINARY);

  fAngularStep     = float(_tstof(AfxGetApp()->GetProfileString(__KEY__, _T("Angular Step"), _T("0.9"))));
  sString.Format(_T("%.4f"), fAngularStep);
  editAngularStep.SetWindowText(sString);

  fPostThreshold   = float(_tstof(AfxGetApp()->GetProfileString(__KEY__, _T("Post Threshold"), _T("3.0"))));
  sString.Format(_T("%.2f"), fPostThreshold);
  editPostThreshold.SetWindowText(sString);

  bPreMedian       = AfxGetApp()->GetProfileInt(__KEY__, _T("Pre Median Filter"), 0)    != 0;
  bPostMedian      = AfxGetApp()->GetProfileInt(__KEY__, _T("Post Median Filter"), 0)   != 0;
  bPostGaussian    = AfxGetApp()->GetProfileInt(__KEY__, _T("Post Gaussian Filter"), 0) != 0;

  if(bPreMedian)    buttonPreMedianFilter.SetCheck(BST_CHECKED);
  if(bPostGaussian) buttonPostGaussianFilter.SetCheck(BST_CHECKED);
  if(bPostMedian) {
    buttonPostMedianFilter.SetCheck(BST_CHECKED);
    editPostThreshold.EnableWindow(true);
  } else {
    editPostThreshold.EnableWindow(false);
  }

  editDarkDir.SetWindowText(sDarkDir);
  editBeamDir.SetWindowText(sBeamDir);
  editProjDir.SetWindowText(sProjectionDir);
  editNormDir.SetWindowText(sNormDir);

  if(bUseDark)
    buttonUseDark.SetCheck(BST_CHECKED);
  else
    buttonUseDark.SetCheck(BST_UNCHECKED);

  if(bFilterDark)
    buttonFilterDark.SetCheck(BST_CHECKED);
  else
    buttonFilterDark.SetCheck(BST_UNCHECKED);

  if(bUseBeam)
    buttonUseBeam.SetCheck(BST_CHECKED);
  else
    buttonUseBeam.SetCheck(BST_UNCHECKED);

  if(bFilterBeam)
    buttonFilterBeam.SetCheck(BST_CHECKED);
  else
    buttonFilterBeam.SetCheck(BST_UNCHECKED);

  if(bCorrectAndorBug)
    buttonAndorBug.SetCheck(BST_CHECKED);
  else
    buttonAndorBug.SetCheck(BST_UNCHECKED);

  editDarkDir.EnableWindow(bUseDark);
  buttonChangeDarkDir.EnableWindow(bUseDark);
  buttonFilterDark.EnableWindow(bUseDark);

  editBeamDir.EnableWindow(bUseBeam);
  buttonChangeBeamDir.EnableWindow(bUseBeam);
  buttonFilterBeam.EnableWindow(bUseBeam);

  return TRUE;  // return TRUE unless you set the focus to a control
}


void
CPreProcessDlg::OnBnClickedOk() {
  if(!UpdateData(true)) {
    MessageBeep(-1);
    return;
  }
  if(sProjectionDir != _T("")) AfxGetApp()->WriteProfileString(__KEY__, _T("Projections Directory"), sProjectionDir);
  if(sNormDir       != _T("")) AfxGetApp()->WriteProfileString(__KEY__, _T("Normalized Directory"),  sNormDir);
  if(sBeamDir       != _T("")) AfxGetApp()->WriteProfileString(__KEY__, _T("Beam Directory"),        sBeamDir);
  if(sDarkDir       != _T("")) AfxGetApp()->WriteProfileString(__KEY__, _T("Dark Directory"),        sDarkDir);
  
  AfxGetApp()->WriteProfileInt(__KEY__, _T("Use Dark"), bUseDark);
  AfxGetApp()->WriteProfileInt(__KEY__, _T("Filter Dark"), bFilterDark);
  AfxGetApp()->WriteProfileInt(__KEY__, _T("Use Beam"), bUseBeam);
  AfxGetApp()->WriteProfileInt(__KEY__, _T("Filter Beam"), bFilterBeam);
  AfxGetApp()->WriteProfileInt(__KEY__, _T("Correct Andor Bug"), bCorrectAndorBug);

  AfxGetApp()->WriteProfileInt(__KEY__, _T("Projection Format"), ProjectionFormat);
  
  AfxGetApp()->WriteProfileInt(__KEY__, _T("Pre Median Filter"), bPreMedian);
  AfxGetApp()->WriteProfileInt(__KEY__, _T("Post Median Filter"), bPostMedian);
  AfxGetApp()->WriteProfileInt(__KEY__, _T("Post Gaussian Filter"), bPostGaussian);

  CString sString;
  editAngularStep.GetWindowText(sString);
  AfxGetApp()->WriteProfileString(__KEY__, _T("Angular Step"), sString);
  fAngularStep = _tstof(sString);


  editPostThreshold.GetWindowText(sString);
  AfxGetApp()->WriteProfileString(__KEY__, _T("Post Threshold"), sString);

  OnOK();
}


void
CPreProcessDlg::OnBnClickedChangeDarkDir() {
  CString sNewDir    = sDarkDir;
  if(ChooseDir(false, sNewDir, _T("*.*"), _T("Choose Dark Directory"), _T("Select the Directory Containing the Dark Images"))) 
    sDarkDir = sNewDir;
  editDarkDir.SetWindowText(sDarkDir);
  UpdateData(false);
}


void
CPreProcessDlg::OnBnClickedChangeBeamDir() {
  CString sNewDir    = sBeamDir;
  if(ChooseDir(false, sNewDir, _T("*.*"), _T("Choose Beam Directory"), _T("Select the Directory Containing the Unobstructed Beam")))
    sBeamDir = sNewDir;
  editBeamDir.SetWindowText(sBeamDir);
  UpdateData(false);
}


void
CPreProcessDlg::OnBnClickedChangeProjDir() {
  CString sNewDir = sProjectionDir;
  if(ChooseDir(false, sNewDir, _T("*.*"), _T("Choose Projections Directory"), _T("Select the Directory Containing the Projection Images")))
    sProjectionDir = sNewDir;
  editProjDir.SetWindowText(sProjectionDir);
  UpdateData(false);
}


void
CPreProcessDlg::OnBnClickedChangeNormDir() {
  CString sNewDir = sNormDir;
  if(ChooseDir(true, sNewDir, _T("*.*"), _T("Choose Output Directory"), _T("Select or Create the Directory Where to Write the Normalized Projections")))
    sNormDir = sNewDir;
  editNormDir.SetWindowText(sNormDir);
  UpdateData(false);
}


void
CPreProcessDlg::OnBnClickedUseDark() {
  bUseDark = buttonUseDark.GetCheck()==BST_CHECKED;
  editDarkDir.EnableWindow(bUseDark);
  buttonChangeDarkDir.EnableWindow(bUseDark);
  buttonFilterDark.EnableWindow(bUseDark);
  UpdateData(false);
}


void
CPreProcessDlg::OnBnClickedAndorBug() {
  bCorrectAndorBug = buttonAndorBug.GetCheck()==BST_CHECKED;
  UpdateData(false);
}


void
CPreProcessDlg::SetDarkDir(CString sNewDirectory) {
  sDarkDir = sNewDirectory;
}


void
CPreProcessDlg::SetBeamDir(CString sNewDirectory) {
  sBeamDir = sNewDirectory;
}

void
CPreProcessDlg::SetProjectionDir(CString sNewDirectory) {
  sProjectionDir = sNewDirectory;
}

void
CPreProcessDlg::SetNormDir(CString sNewDirectory) {
  sNormDir = sNewDirectory;
}


CString
CPreProcessDlg::GetDarkDir() {
  return sDarkDir;
}


CString
CPreProcessDlg::GetBeamDir() {
  return sBeamDir;
}


CString
CPreProcessDlg::GetProjectionDir() {
  return sProjectionDir;
}


CString
CPreProcessDlg::GetNormDir() {
  return sNormDir;
}


void
CPreProcessDlg::OnBnClickedPreMedian() {
  bPreMedian = (buttonPreMedianFilter.GetCheck()== BST_CHECKED);
}


void
CPreProcessDlg::OnBnClickedPostMedain() {
  bPostMedian = buttonPostMedianFilter.GetCheck()== BST_CHECKED;
  editPostThreshold.EnableWindow(bPostMedian);
}


bool
CPreProcessDlg::GetCorrectionAndorBug() {
  return bCorrectAndorBug;
}


int
CPreProcessDlg::GetProjectionFormat() {
  return ProjectionFormat;
}


bool
CPreProcessDlg::GetUseDark() {
  return bUseDark;
}


bool
CPreProcessDlg::GetUseBeam() {
  return bUseBeam;
}


bool
CPreProcessDlg::GetPreMedianFilter() {
  return bPreMedian;
}


bool
CPreProcessDlg::GetPostMedianFilter() {
  return bPostMedian;
}


void
CPreProcessDlg::OnBnClickedFilterBeam() {
  bFilterBeam = buttonFilterBeam.GetCheck()==BST_CHECKED;
  UpdateData(false);
}


bool
CPreProcessDlg::GetFilterBeam() {
  return bFilterBeam;
}


void
CPreProcessDlg::OnBnClickedUseBeam() {
  bUseBeam = buttonUseBeam.GetCheck()==BST_CHECKED;
  editBeamDir.EnableWindow(bUseBeam);
  buttonChangeBeamDir.EnableWindow(bUseBeam);
  buttonFilterBeam.EnableWindow(bUseBeam);
  UpdateData(false);
}


void
CPreProcessDlg::OnBnClickedFilterDark() {
  bFilterDark = buttonFilterDark.GetCheck()==BST_CHECKED;
  UpdateData(false);
}


bool
CPreProcessDlg::GetFilterDark() {
  return bFilterDark;
}
