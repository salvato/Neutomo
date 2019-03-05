// Utilities for Tomography
#include "stdafx.h"
#include "math.h"
#include "float.h"
#include "Point2f.h"
#include "AlignWnd.h"
#include "Projection.h"
#include "Plot3.h"
#include "NeuTomoUtil.h"


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


int
makepow2(int val) {
  if(!val) return 0;
  int power=0;
  while(val>>= 1) power++;
  return (1 << power);
}


// Callback for Direcory Browser
int CALLBACK 
BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData) {
  TCHAR szDir[MAX_PATH];
  tBrowseData* pBrowseData = (tBrowseData*)pData;

  switch(uMsg) {
    case BFFM_INITIALIZED:
      SetWindowText(hwnd, pBrowseData->sWindowTitle);
      _tcscpy(szDir, pBrowseData->sStartingDir);
      // WParam is TRUE since you are passing a path.
      // It would be FALSE if you were passing a pidl.
      SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)szDir);
      break;
    case BFFM_SELCHANGED: 
      // Set the status window to the currently selected path.
      if(SHGetPathFromIDList((LPITEMIDLIST) lp, szDir)) {
        SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szDir);
      }
      break;
  }
  return 0;
}


void
_glutInit() {
  int argc = 1;
  char* argv = GetCommandLineA();
  glutInit(&argc, &argv);
}


const CString
GetGluErrorString(GLenum error) {
  CString sString;
  sString.Format(_T("%s"), gluErrorStringWIN(error));
  return sString;
}


int
isExtensionSupported(const char *extension) {
  const GLubyte *extensions = NULL;
  const GLubyte *start;
  GLubyte *where, *terminator;
  // Extension names should not have spaces.
  where = (GLubyte *)strchr(extension, ' ');
  if(where || *extension == '\0')
    return 0;
  extensions = glGetString(GL_EXTENSIONS);
  // It takes a bit of care to be fool-proof about parsing the
  // OpenGL extensions string. Don't be fooled by sub-strings, etc.
  start = extensions;
  for(;;) {
    where = (GLubyte *)strstr((const char *) start, extension);
    if(!where)
      break;
    terminator = where + strlen(extension);
    if(where == start || *(where - 1) == ' ')
      if(*terminator == ' ' || *terminator == '\0')
        return 1;
    start = terminator;
  }
  return 0;
}


bool
CheckGLErrors(CString sModule) {
  CString sError;
  GLenum error;
  bool retval = true;
  while((error = glGetError()) != GL_NO_ERROR) {
    sError.Format(_T("GL Error in:%s\n%s"), sModule, GetGluErrorString(error));
    AfxMessageBox(sError);
    retval = false;
  } // while((error = glGetError()) != GL_NO_ERROR)
  return retval;
}


// Checks framebuffer status.
// Copied directly out of the spec, modified to deliver a return value.
bool
CheckFramebufferStatus() {
  GLenum status;
  status = (GLenum) glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  switch(status) {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
      return true;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
	    AfxMessageBox(_T("Framebuffer incomplete, incomplete attachment\n"));
      break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
	    AfxMessageBox(_T("Unsupported framebuffer format\n"));
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
	    AfxMessageBox(_T("Framebuffer incomplete, missing attachment\n"));
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
	    AfxMessageBox(_T("Framebuffer incomplete, attached images must have same dimensions\n"));
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
	    AfxMessageBox(_T("Framebuffer incomplete, attached images must have same format\n"));
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
	    AfxMessageBox(_T("Framebuffer incomplete, missing draw buffer\n"));
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
	    AfxMessageBox(_T("Framebuffer incomplete, missing read buffer\n"));
      break;
  }
  return false;
}


////////////////////////////////////////////////////////////
//
// inversion for 4x4 matrix only containing rotations
//  and translations
///////////////////////////////////////////////////////
void
invertMat(float mat[16]){
  float temp;

  temp=mat[1];
  mat[1]=mat[4];
  mat[4]=temp;

  temp=mat[2];
  mat[2]=mat[8];
  mat[8]=temp;

  temp=mat[6];
  mat[6]=mat[9];
  mat[9]=temp;
  
  mat[12]=-mat[12];
  mat[13]=-mat[13];
  mat[14]=-mat[14];
}


/////////////////////////////////////////////////////////////////////////////
// Linear Regression                                                       //
//                                                                         //
// y(x) = a + b x, for n samples                                           //
// The following assumes the standard deviations are unknown for x and y   //
// Return a, b and r the regression coefficient                            //
/////////////////////////////////////////////////////////////////////////////
bool
LinRegress(float *x, float *y, int n, float *a, float *b, float *r) {
  int i;
  float sumx=0,sumy=0,sumx2=0,sumy2=0,sumxy=0;
  float sxx,syy,sxy;

  *a = *b = *r = 0;
  if(n < 2) return false;
  /* Compute some things we need */
  for(i=0;i<n;i++) {
    sumx += x[i];
    sumy += y[i];
    sumx2 += (x[i] * x[i]);
    sumy2 += (y[i] * y[i]);
    sumxy += (x[i] * y[i]);
  }
  sxx = sumx2 - sumx * sumx / n;
  syy = sumy2 - sumy * sumy / n;
  sxy = sumxy - sumx * sumy / n;
  /* Infinite slope (b), non existant intercept (a) */
  if(fabs(sxx) == 0.0) return false;
  /* Calculate the slope (b) and intercept (a) */
  *b = sxy / sxx;
  *a = sumy / n - (*b) * sumx / n;
  /* Compute the regression coefficient */
  if(fabs(syy) == 0)
    *r = 1;
  else
    *r = sxy / sqrt(sxx * syy);
  return true;
}


/*//////////////////////////////////////////////////////////
	fft - calculate FFT

	Carl Crawford
	Purdue University
	W. Lafayette, IN. 47907

	Calling Sequence....fft(real, im, m, iopt)

	Where real and im are the real and imaginary
	parts of the input data.  The result is
	returned in place.  M is the log base 2
	of the number of elements in the array.

	iopt is equal to 0 for the forward
	transform and 1 for the inverse transform.
*///////////////////////////////////////////////////////////
void
fft(float *a, float *b, int m, int iopt) {
	// a[];  real part of data
	// b[];  imaginary part of data
	// m;	   size of data = 2**m
	// iopt: 0=dft, 1=idft
	int	nv2, nm1, n, le, le1, ip;
	float pi, pile1, tmp;
	float ua, ub, wa, wb, ta, tb, *ap, *bp;
	int i, j, l;
	n = 1<<m;
	// For the inverse transform
	// scale the result by N
	if(iopt) {
		for(i=0, ap=a, bp=b; i<n; i++) {
			*ap++ /= n;
			*bp++ /= -n;
		}
	}
	nv2 = n/2;
	nm1 = n - 1;
	j = 0;
	for(i=0;i<nm1;i++) {
		if(i<j) {
			ta = a[j];	tb = b[j];
			a[j] = a[i];	b[j] = b[i];
			a[i] = ta;	b[i] = tb;
		}
		l = nv2;
		while(l < (j+1) ) {
			j = j - l;
			l = l / 2;
		}
		j = j + l;
	}
	pi = acos(-1.0);
	for(l=1; l<=m; l++) {
		le = 1<<l;
		le1 = le>>1;
		ua = 1.0;	ub = 0.0;
		pile1 = pi / le1;
		wa = cos(pile1);	wb = -sin(pile1);
		for(j=0; j<le1; j++) {
			for(i=j; i<n; i+=le) {
				ip = i + le1;
				ta = a[ip] * ua - b[ip] * ub;
				tb = a[ip] * ub + b[ip] * ua;
				a[ip] = a[i] - ta;
				b[ip] = b[i] - tb;
				a[i] += ta;
				b[i] += tb;
			}
			ua = (tmp = ua) * wa - ub * wb;
			ub = tmp * wb + ub * wa;
		}
	}
	if(iopt != 0) {
		for(i=0; i<n; i++)
			b[i] = -b[i];
	}
}


void 
CrossCorr(float *y1, float *y2, float *r, int nSamples, int nChan) {
  int i, j, k, n;
  float r1, r2, mean1=0.0, mean2=0.0;
  ZeroMemory(r, sizeof(*r)*nChan);
  for(j=0; j<nSamples; j++) {// t
    mean1 += *(y1+j);
    mean2 += *(y2+j);
  } // for(j=0; j<nSamples; j++)
  mean1 /= double(nSamples);
  mean2 /= double(nSamples);

  for(i=-nChan/2; i<nChan/2; i++) {// tau
    n = 0;
    for(j=0; j<nSamples; j++) {// t
      k = j + i;// t + tau
      if(k<0 || k>=nSamples) continue;
      r1 = *(y1+j);
      r2 = *(y2+k);
      *(r+i+nChan/2) += (r1-mean1)*(r2-mean2);
      n++;
    }// for(i=-nChan/2; i<nChan/2; i++) {for(j=0; j<nSamples; j++)
    *(r+i+nChan/2) /= double(n);// Normalize to N_Points
  } // for(i=-nChan/2; i<nChan/2; i++)
}

//#define __FILTERCHECK 1

bool
BuildFilter(float* wfilt, int nfft, int rowWidth, FILTER_TYPES filterType, float samplingFrequency) {
  int m = int(log(double(nfft))/log(2.0)+0.5);
  float *pr1    = NULL;
  float *pr2    = NULL;
  if(filterType == RAM_LAK) {
    pr1    = new float[nfft];
    pr2    = new float[nfft];
    ZeroMemory(pr1, nfft*sizeof(*pr1));
    ZeroMemory(pr2, nfft*sizeof(*pr2));

    // Ram-Lak Filter
    // See:
    // J.Banhart ed. "Advanced Tomographyc Methods in Materials
    // Research and Engineering - Oxford University (2008)
    float tau = 2.0f/float(rowWidth-1);
 	  *pr1 = 0.25/tau;
	  for(int i=1; i<rowWidth; i+=2) {
		  *(pr1+nfft-i) = *(pr1+i) = -1.0/(i*i*M_PI*M_PI*tau);
	  }

    #ifdef __FILTERCHECK
      CPlotWindow Plot(_T("Ram-Lak Filter in Time domain"));
      if(pr1 != NULL) {
        Plot.ClearPlot();
        Plot.NewDataSet(1, 1, RGB(255,255,0), CPlotWindow::iline, _T(""));
        for(int i=0; i<nfft; i++) {
          Plot.NewPoint(1, i, double(*(pr1+i)));
        }
        Plot.SetShowDataSet(1,  true);
        Plot.UpdatePlot();
        if(AfxMessageBox(_T("Press OK to Continue"), MB_OKCANCEL) == IDCANCEL) {
          if(pr1    != NULL) delete[] pr1; pr1 = NULL;
          if(pr2    != NULL) delete[] pr2; pr2 = NULL;
          return false;
        }
      }
    #endif
    fft(pr1, pr2, m, 0);
    memcpy(wfilt, pr1, nfft*sizeof(*pr1));
  } else {//Shepp-Logan
    pr1    = new float[nfft];
    pr2    = new float[nfft];
    ZeroMemory(pr1, nfft*sizeof(*pr1));
    ZeroMemory(pr2, nfft*sizeof(*pr2));
    // Shepp-Logan Filter
    // See:
    // Manuel Dierick
    // Tomographic Imaging Techniques using Cold and Thermal Neutron Beams
    // Thesis (2005)
    // Department of Subatomic and Radiation Physics - Gent University
    float tau = 2.0f/float(rowWidth-1);
    *pr1 = 2.0/(M_PI*M_PI*tau);
	  for(int i=1; i<rowWidth; i++) {
		  *(pr1+nfft-i) = *(pr1+i) = 2.0/(M_PI*M_PI*tau*(1.0-4.0*i*i));
	  }

    #ifdef __FILTERCHECK
      CPlotWindow Plot(_T("Shepp-Logan Filter in Time domain"));
      if(pr1 != NULL) {
        Plot.ClearPlot();
        Plot.NewDataSet(1, 1, RGB(255,255,0), CPlotWindow::iline, _T(""));
        for(int i=0; i<nfft; i++) {
          Plot.NewPoint(1, i, double(*(pr1+i)));
        }
        Plot.SetShowDataSet(1,  true);
        Plot.UpdatePlot();
        if(AfxMessageBox(_T("Press OK to Continue"), MB_OKCANCEL) == IDCANCEL) {
          if(pr1    != NULL) delete[] pr1; pr1 = NULL;
          if(pr2    != NULL) delete[] pr2; pr2 = NULL;
          return false;
        }
      }
    #endif
    fft(pr1, pr2, m, 0);
    memcpy(wfilt, pr1, nfft*sizeof(*pr1));
  }
  #ifdef __FILTERCHECK
    CPlotWindow Plot(_T("Frequency Domain Filter Response"));
    Plot.ClearPlot();
    Plot.NewDataSet(1, 1, RGB(255,255,0), CPlotWindow::iline, _T(""));
    for(int i=0; i<nfft; i++) {
      Plot.NewPoint(1, i, double(*(wfilt+i)));
    }
    Plot.SetShowDataSet(1,  true);
    Plot.UpdatePlot();
    if(AfxMessageBox(_T("Press OK to Continue"), MB_OKCANCEL) == IDCANCEL) {
      if(pr1    != NULL) delete[] pr1; pr1 = NULL;
      if(pr2    != NULL) delete[] pr2; pr2 = NULL;
      return false;
    }
  #endif// __FILTERCHECK
  if(pr1    != NULL) delete[] pr1; pr1 = NULL;
  if(pr2    != NULL) delete[] pr2; pr2 = NULL;
  return true;
}
