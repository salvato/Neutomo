#include "stdafx.h"
#include <process.h> // _beginthread, _endthread
#include "resource.h"
#include "NeuTomoDlg.h"
#include "Projection.h"
#include "GrabberWnd.h"
#include "SliceThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void __cdecl
RunShowSlicesThread(void *lpParam) {
	CNeuTomoDlg *pDlg = (CNeuTomoDlg *)lpParam;
  if(pDlg == NULL) {
	  pDlg->hThread = 0;  // We have Done !
    _endthread();
  }
	pDlg->SetEscape(0); // Prepare for Elaboration
	ShowSlices(lpParam);// Do the show and return only when user stops
	pDlg->hThread=0;    // We have Done !
}


void
ShowSlices(void *lpParam) {
	CNeuTomoDlg *pDlg = (CNeuTomoDlg *)lpParam;
  CProjection Projection;
  CString sString;
  float* pData;
  RGBTRIPLE *pRow;
  int yOff, iVal;
  float GrayMin = pDlg->GetGrayMin();
  float GrayMax = pDlg->GetGrayMax();
  float DeltaGray = GrayMax-GrayMin;
  while(!pDlg->GetEscape()) {
    sString.Format(_T("Slice# %d"), pDlg->GetCurrentSlice());
    pDlg->pGrabberWnd->SetWindowText(sString);
    Projection.ReadFromFitsFile(pDlg->GetPathNames()[pDlg->GetCurrentSlice()]);
    pData = Projection.pData;
    for(int y=0; y<pDlg->GetImgHeight(); y++) {
      pRow = (RGBTRIPLE *)pDlg->pGrabberWnd->Image.GetBits(y);
      yOff = pDlg->GetImgWidth() * y;
      for(int x=0; x<pDlg->GetImgWidth(); x++) {
        iVal = int(255.0*(*(pData+yOff+x)-GrayMin)/DeltaGray);
        (*(pRow+x)).rgbtRed = (*(pRow+x)).rgbtGreen = (*(pRow+x)).rgbtBlue = iVal;
      }
    }
    pDlg->pGrabberWnd->UpdateNow();
    //CString sString = pDlg->GetPathNames()[pDlg->GetCurrentSlice()];
    //sString += ".bmp";
    //pDlg->pGrabberWnd->Image.Save(sString, CXIMAGE_FORMAT_BMP);
    pDlg->SetCurrentSlice((pDlg->GetCurrentSlice()+1) % pDlg->GetNProjections()); 
    Sleep(30);
  }// while(true)
  pDlg->SetCurrentSlice(pDlg->GetCurrentSlice() - 1);
  if(pDlg->GetCurrentSlice() < 0 )pDlg->SetCurrentSlice(pDlg->GetNProjections()-1);
}


