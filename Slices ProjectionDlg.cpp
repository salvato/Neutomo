// Slices ProjectionDlg.cpp : file di implementazione
//
// Written by Vincenzo Finocchiaro
//
#include "stdafx.h"
#include "float.h"
#include "resource.h"
#include "Projection.h"
#include "Slices ProjectionDlg.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PI 3.14159265358979323846


// finestra di dialogo CSlicesProjectionDlg


CSlicesProjectionDlg::CSlicesProjectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSlicesProjectionDlg::IDD, pParent)
	, iN_img(200)
	, img_width(256)
	, img_height(256)
  , sPhantomDir(_T("."))
{
	m_hIcon     = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	sPhantomDir = AfxGetApp()->GetProfileString(__KEY__, _T("Phantom Directory"),      _T("."));
}


void 
CSlicesProjectionDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_CALCULATE, Progress_Calculate);
	DDX_Text(pDX, IDC_N_IMG, iN_img);
	DDV_MinMaxInt(pDX, iN_img, 1, 999);
	DDX_Text(pDX, IDC_WIDTH_IMG, img_width);
	DDV_MinMaxInt(pDX, img_width, 128, 1024);
	DDX_Text(pDX, IDC_HEIGHT_IMG, img_height);
	DDV_MinMaxInt(pDX, img_height, 128, 1024);
}


BEGIN_MESSAGE_MAP(CSlicesProjectionDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_OPEN_FILE, &CSlicesProjectionDlg::OnBnClickedOpenFile)
END_MESSAGE_MAP()


// gestori di messaggi di CSlicesProjectionDlg

BOOL
CSlicesProjectionDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// Impostare icona grande.
	SetIcon(m_hIcon, FALSE);		// Impostare icona piccola.
	sPhantomDir = AfxGetApp()->GetProfileString(__KEY__, _T("Phantom Directory"),      _T("."));
	return TRUE;  // restituisce TRUE a meno che non venga impostato lo stato attivo su un controllo.
}


void
CSlicesProjectionDlg::OnSysCommand(UINT nID, LPARAM lParam) {
	CDialog::OnSysCommand(nID, lParam);
}


void
CSlicesProjectionDlg::OnPaint() {
	if (IsIconic()) {
		CPaintDC dc(this); // contesto di periferica per il disegno
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialog::OnPaint();
	}
}


HCURSOR
CSlicesProjectionDlg::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}


//typedef struct tag_IMG_HEADER {
//  int		header_lenght; 
//  int		n_rows; 
//  int		n_columns; 
//  float		angle; 
//  BYTE		fill[1008];
//} IMG_HEADER, *PIMG_HEADER; 


bool
AxisCalculate(float center_z, float asse_a, float asse_b, float asse_c, float z, float* new_axis) {
	if(abs(z-center_z) < asse_c) {
		float tmp=sqrt(1-pow((z-center_z)/asse_c,2));
		new_axis[0]=asse_a*tmp;
		new_axis[1]=asse_b*tmp;
		return true;
	} else {
		new_axis[0]=0.0f;
		new_axis[1]=0.0f;
		return false;
	}
}


void
CSlicesProjectionDlg::OnBnClickedOpenFile() {
/*
  if (!UpdateData(true)) {
		return;
	}
	CFileDialog FileOpen(TRUE, 0, 0, OFN_HIDEREADONLY, _T("Ellipsis Data File (*.d)|*.d|All Files (*.*)|*.*||"));
	FileOpen.m_ofn.lpstrTitle = _T("Open Ellipsis File Name");
	FileOpen.m_ofn.lpstrInitialDir = sPhantomDir;
  if(FileOpen.DoModal() != IDOK) {  
	  return;
  }
	CString FileIn = FileOpen.GetPathName();
	FILE *ifd;
  if((ifd = _tfopen(FileIn, _T("r"))) == NULL){
		CString String;
		String.Format(_T("Can't open: %s\n"), FileIn);
		AfxMessageBox(String,MB_ICONWARNING);
		return;
	}
	char buf[1024];
	int n_ellipses=0;
	while (fgets(buf,sizeof(buf), ifd) != NULL) {
		n_ellipses++;
	}
	if(n_ellipses == 0) {
		fclose(ifd);	  
		CString String;
		String.Format(_T("%s is empty\n"), FileIn);
		AfxMessageBox(String, MB_ICONWARNING);
		return;
	}
	fseek(ifd,0,0);
	float *px,*py,*pz,*pa,*pb,*pc,*palfa,*prho;
	px = new float[n_ellipses];
	py = new float[n_ellipses];
	pz = new float[n_ellipses];
	pa = new float[n_ellipses];
	pb = new float[n_ellipses];
	pc = new float[n_ellipses];
	palfa = new float[n_ellipses];
	prho = new float[n_ellipses];
	for(int j=0; j<n_ellipses; j++) {
		// Lettura del file,
		if(fscanf(ifd,"%f %f %f %f %f %f %f %f", px+j, py+j, pz+j,  pa+j, pb+j, pc+j, palfa+j, prho+j) != 8) {
			AfxMessageBox(_T("Read error"),MB_ICONWARNING);
			fclose(ifd);
			delete[] px;
			delete[] py;
			delete[] pz;
			delete[] pa;
			delete[] pb;
			delete[] pc;
			delete[] palfa;
			delete[] prho;
			return;
		}
    *(palfa+j) = float(PI/180.0*(*(palfa+j)));
	}	
	fclose(ifd);
	CString NameFileIn = FileOpen.GetFileTitle();
    SetWindowText(NameFileIn);
	// File letto.
	// Creazione immagini.
	CFileDialog FileSave(FALSE, 0, NameFileIn, OFN_HIDEREADONLY, _T("Binary Image Float (*.bif)|*.bif|All Files (*.*)|*.*||"));
	FileSave.m_ofn.lpstrTitle = _T("Save Binary Images");
	FileSave.m_ofn.lpstrInitialDir = sPhantomDir;
	if(FileSave.DoModal() != IDOK) {  
		delete[] px;
		delete[] py;
		delete[] pz;
		delete[] pa;
		delete[] pb;
		delete[] pc;
		delete[] palfa;
		delete[] prho;
		return;
	}
  CProjection Projection(img_width, img_height, 0.0f);
	float x_pitch     = (2.0f/Projection.n_columns),
		    z_pitch     = (2.0f/Projection.n_rows),
		    x_min       = (1 - Projection.n_columns) * 0.5f,
		    z_min       = (1 - Projection.n_rows) * 0.5f,
		    angle_pitch = float(PI/iN_img),
		    theta       = 0.0f,
		    z_coord     = 0.0f,
		    x_coord     = 0.0f,		  
		    tmp1        = 0.0f,
		    tmp2        = 0.0f,
		    tmp3        = 0.0f,
		    tmpa        = 0.0f,	
		    axis[2];
	CString sFileName;
  sPhantomDir = FileSave.GetPathName();
  sPhantomDir = sPhantomDir.Left(sPhantomDir.ReverseFind(_T('\\')));
  float f;
	for(int i=0; i<(iN_img+1); i++) {
		Progress_Calculate.SetPos(i*100 / iN_img);
		ZeroMemory(Projection.pData, Projection.n_rows*Projection.n_columns*sizeof(*(Projection.pData)));
		theta = angle_pitch*i;
		Projection.f_angle = float(180.0 * theta / PI);
		for(int j=0; j<Projection.n_rows; j++) {
			z_coord = z_pitch * (j+z_min);
			for(int k=0; k<n_ellipses; k++) {
				if(AxisCalculate(pz[k], pa[k], pb[k], pc[k], z_coord, axis)) {
					// tmpa = (A cos(theta - alfa))^2 + (B sin(theta - alfa))^2 = a(theta)^2
					tmpa = pow((axis[0] * cos(theta-palfa[k])),2) + pow((axis[1] * sin(theta-palfa[k])),2);
					if((py[k]==0) && (px[k]==0)) {
						tmp2 = 0;
					} else {
						// tmp2 = s * cos(gamma - (theta - alfa))
						tmp2 = sqrt(pow(px[k], 2) + pow(py[k], 2)) * cos(atan2f(py[k], px[k]) - (theta-palfa[k]));
					}
					for(int m=0; m<Projection.n_columns; m++) {
						x_coord = x_pitch * (m+x_min);
						if(tmpa > pow(x_coord-tmp2, 2)) {
							// tmp1 = 2 rho A B / a(theta)^2
							tmp1 = (2.0f * prho[k] * axis[0] * axis[1]) / tmpa;
              f = tmp1 * sqrt(tmpa-pow(x_coord-tmp2, 2));
							Projection.pData[(Projection.n_columns * j) + m] += f;
						}
					}
				}
			}
		}

		sFileName.Format(_T("%s_%.3d.bif"), FileSave.GetFileTitle(), i);
    Projection.WriteToFile(sPhantomDir, sFileName);
	}
  if(sPhantomDir != _T("")) AfxGetApp()->WriteProfileString(__KEY__, _T("Phantom Directory"), sPhantomDir);
	CString String;
	String.Format(_T("Writing Completed Successfully\n"));
	AfxMessageBox(String,MB_ICONWARNING);
		
	delete[] px;
	delete[] py;
	delete[] pz;
	delete[] pa;
	delete[] pb;
	delete[] pc;
	delete[] palfa;
	delete[] prho;
	return;
*/
}