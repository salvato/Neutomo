// GeometryDlg.cpp : file di implementazione
//

#include "stdafx.h"
#include "NeuTomo.h"
#include "GeometryDlg.h"


// finestra di dialogo CGeometryDlg


IMPLEMENT_DYNAMIC(CGeometryDlg, CDialog)

CGeometryDlg::CGeometryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGeometryDlg::IDD, pParent)
  , BeamGeometry(PARALLEL_BEAM)
  , SourceSampleDistance(17.000f)
  , SourceDetectorDistance(17.400f)
  , HorizzontalFOV(40.0f)
  , VerticalFOV(40.0f)
{
	sString                = AfxGetApp()->GetProfileString(__KEY__, _T("Source Sample Distance"),  _T("17.0"));
  SourceSampleDistance   = float(_tstof(sString));
	sString                = AfxGetApp()->GetProfileString(__KEY__, _T("Source Detector Distance"),  _T("17.4"));
  SourceDetectorDistance = float(_tstof(sString));
	sString                = AfxGetApp()->GetProfileString(__KEY__, _T("Horizzontal Field Of View"),  _T("40.0"));
  HorizzontalFOV         = float(_tstof(sString));
	sString                = AfxGetApp()->GetProfileString(__KEY__, _T("Verticaal Field Of View"),  _T("40.0"));
  VerticalFOV            = float(_tstof(sString));
	sString                = AfxGetApp()->GetProfileString(__KEY__, _T("Volume X Size"),  _T("256"));
  VolXSize               = _ttoi(sString);
	sString                = AfxGetApp()->GetProfileString(__KEY__, _T("Volume Y Size"),  _T("256"));
  VolYSize               = _ttoi(sString);
	sString                = AfxGetApp()->GetProfileString(__KEY__, _T("Volume Z Size"),  _T("256"));
  VolZSize               = _ttoi(sString);
  BeamGeometry           = (BEAM_GEOMETRY)AfxGetApp()->GetProfileInt(__KEY__, _T("Beam Geometry"), PARALLEL_BEAM);
}


CGeometryDlg::~CGeometryDlg() {
}


void
CGeometryDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_EDIT_SSD, editSourceSampleDist);
  DDX_Control(pDX, IDC_EDIT_SDD, editSourceDetectorDist);
  DDX_Control(pDX, IDC_EDIT_FOV_WIDTH, editHorizzontalFOV);
  DDX_Control(pDX, IDC_EDIT_FOV_HEIGHT, editVerticalFOV);
  DDX_Control(pDX, IDC_PARALLEL_BEAM, buttonParallelBeam);
  DDX_Control(pDX, IDC_CONE_BEAM,     buttonConeBeam);
  DDX_Control(pDX, IDC_EDIT_VOL_X_SIZE, editVolXSize);
  DDX_Control(pDX, IDC_EDIT_VOL_Y_SIZE, editVolYSize);
  DDX_Control(pDX, IDC_EDIT_VOL_Z_SIZE, editVolZSize);
}


BEGIN_MESSAGE_MAP(CGeometryDlg, CDialog)
  ON_BN_CLICKED(IDOK,                 &CGeometryDlg::OnBnClickedOk)
  ON_BN_CLICKED(IDC_PARALLEL_BEAM,    &CGeometryDlg::OnBnClickedParallelBeam)
  ON_BN_CLICKED(IDC_CONE_BEAM,        &CGeometryDlg::OnBnClickedConeBeam)
END_MESSAGE_MAP()


BEAM_GEOMETRY
CGeometryDlg::GetBeamGeometry() {
  return BeamGeometry;
}


float 
CGeometryDlg::GetSourceSampleDistance() {
  return SourceSampleDistance;
}


float
CGeometryDlg::GetSourceDetectorDistance() {
  return SourceDetectorDistance;
}


float
CGeometryDlg::GetVerticalFOV() {
  return VerticalFOV/1000.0f;// in meters
}


float
CGeometryDlg::GetHorizzontalFOV() {
  return HorizzontalFOV/1000.0f;// in meters
}


BOOL
CGeometryDlg::UpdateData(BOOL bSaveAndValidate) {
  BOOL bResult;
  bResult = CDialog::UpdateData(bSaveAndValidate);
  if(bResult) {
    if(bSaveAndValidate) {
      editSourceSampleDist.GetWindowText(sString);
      SourceSampleDistance   = float(_tstof(sString));
      editSourceDetectorDist.GetWindowText(sString);
      SourceDetectorDistance = float(_tstof(sString));
	    editHorizzontalFOV.GetWindowText(sString);
      HorizzontalFOV         = float(_tstof(sString));
	    editVerticalFOV.GetWindowText(sString);
      VerticalFOV            =float(_tstof(sString));
      if(SourceSampleDistance >= SourceDetectorDistance) {
        editSourceSampleDist.SetSel(0, -1);
        editSourceSampleDist.SetFocus();
        bResult = false; 
      }
	    editVolXSize.GetWindowText(sString);
      VolXSize               = _ttoi(sString);
	    editVolYSize.GetWindowText(sString);
      VolYSize               = _ttoi(sString);
	    editVolZSize.GetWindowText(sString);
      VolZSize               = _ttoi(sString);
    }
  }
  return bResult;
}


// gestori di messaggi CGeometryDlg


BOOL
CGeometryDlg::OnInitDialog(){
  CDialog::OnInitDialog();

	sString                = AfxGetApp()->GetProfileString(__KEY__, _T("Source Sample Distance"),  _T("17.0"));
  SourceSampleDistance   = float(_tstof(sString));
	sString                = AfxGetApp()->GetProfileString(__KEY__, _T("Source Detector Distance"),  _T("17.4"));
  SourceDetectorDistance = float(_tstof(sString));
	sString                = AfxGetApp()->GetProfileString(__KEY__, _T("Horizzontal Field Of View"),  _T("40.0"));
  HorizzontalFOV         = float(_tstof(sString));
	sString                = AfxGetApp()->GetProfileString(__KEY__, _T("Verticaal Field Of View"),  _T("40.0"));
  VerticalFOV            = float(_tstof(sString));
	sString                = AfxGetApp()->GetProfileString(__KEY__, _T("Volume X Size"),  _T("256"));
  VolXSize               = _ttoi(sString);
	sString                = AfxGetApp()->GetProfileString(__KEY__, _T("Volume Y Size"),  _T("256"));
  VolYSize               = _ttoi(sString);
	sString                = AfxGetApp()->GetProfileString(__KEY__, _T("Volume Z Size"),  _T("256"));
  VolZSize               = _ttoi(sString);

  BeamGeometry     = (BEAM_GEOMETRY)AfxGetApp()->GetProfileInt(__KEY__, _T("Beam Geometry"), PARALLEL_BEAM);
  if(BeamGeometry == PARALLEL_BEAM) { 
    buttonParallelBeam.SetCheck(BST_CHECKED);
  } else {
    buttonConeBeam.SetCheck(BST_CHECKED);
  }

  sString.Format(_T("%.3f"), SourceSampleDistance);
  editSourceSampleDist.SetWindowText(sString);
  sString.Format(_T("%.3f"), SourceDetectorDistance);
  editSourceDetectorDist.SetWindowText(sString);
  sString.Format(_T("%.3f"), HorizzontalFOV);
  editHorizzontalFOV.SetWindowText(sString);
  sString.Format(_T("%.3f"), VerticalFOV);
  editVerticalFOV.SetWindowText(sString);

  sString.Format(_T("%d"), VolXSize);
  editVolXSize.SetWindowText(sString);
  sString.Format(_T("%d"), VolYSize);
  editVolYSize.SetWindowText(sString);
  sString.Format(_T("%d"), VolZSize);
  editVolZSize.SetWindowText(sString);

  UpdateData(false);
  return TRUE;  // return TRUE unless you set the focus to a control
}


void
CGeometryDlg::OnBnClickedOk() {
  if(!UpdateData(true)) {
    MessageBeep(-1);
    return;
  }
  sString.Format(_T("%.3f"), SourceSampleDistance);
  AfxGetApp()->WriteProfileString(__KEY__, _T("Source Sample Distance"),  sString);
  sString.Format(_T("%.3f"), SourceDetectorDistance);
  AfxGetApp()->WriteProfileString(__KEY__, _T("Source Detector Distance"),  sString);
  sString.Format(_T("%.3f"), HorizzontalFOV);
  AfxGetApp()->WriteProfileString(__KEY__, _T("Horizzontal Field Of View"),  sString);
  sString.Format(_T("%.3f"), VerticalFOV);
  AfxGetApp()->WriteProfileString(__KEY__, _T("Vertical Field Of View"),  sString);

  sString.Format(_T("%d"), VolXSize);
  AfxGetApp()->WriteProfileString(__KEY__, _T("Volume X Size"),  sString);
  sString.Format(_T("%d"), VolYSize);
  AfxGetApp()->WriteProfileString(__KEY__, _T("Volume Y Size"),  sString);
  sString.Format(_T("%d"), VolZSize);
  AfxGetApp()->WriteProfileString(__KEY__, _T("Volume Z Size"),  sString);

  AfxGetApp()->WriteProfileInt(__KEY__, _T("Beam Geometry"), (int)BeamGeometry);
  OnOK();
}


void
CGeometryDlg::OnBnClickedParallelBeam() {
  BeamGeometry = PARALLEL_BEAM;
}


void
CGeometryDlg::OnBnClickedConeBeam() {
  BeamGeometry = CONE_BEAM;
}


int
CGeometryDlg::GetVolXSize() {
  return VolXSize;
}


int
CGeometryDlg::GetVolYSize() {
  return VolYSize;
}


int
CGeometryDlg::GetVolZSize() {
  return VolZSize;
}

