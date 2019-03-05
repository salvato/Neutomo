// Slices ProjectionDlg.h : file di intestazione
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// finestra di dialogo CSlicesProjectionDlg
class CSlicesProjectionDlg : public CDialog
{
// Costruzione
public:
	CSlicesProjectionDlg(CWnd* pParent = NULL);	// costruttore standard

// Dati della finestra di dialogo
	enum { IDD = IDD_SLICESPROJECTION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// supporto DDX/DDV


// Implementazione
protected:
	HICON m_hIcon;

	// Funzioni generate per la mappa dei messaggi
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnBnClickedOpenFile();
	// Percentuale di calcolo delle immagini effettuata.
	CProgressCtrl Progress_Calculate;
	// Numero di immagini da calcolare.
	int iN_img;
	// Larghezza delle immagini da calcolare.
	int img_width;
	// Altezza delle immagini da calcolare.
	int img_height;
  CString sPhantomDir;
};
