#pragma once
#include "afxwin.h"
#include "shlwapi.h"// For Directory Browser
#include <GL/glew.h>
#include <GL/glut.h>
#include "Cg/cgGL.h"

class CProjection;

struct tBrowseData {
  CString sWindowTitle;
  CString sStartingDir;
};

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData);

int isExtensionSupported(const char *extension);
const CString GetGluErrorString(GLenum error);
bool CheckGLErrors(CString sModule);
bool CheckFramebufferStatus();
void _glutInit();
void invertMat(float mat[16]);
void CrossCorr(float *y1, float *y2, float *r, int nSamples, int nChan);
void fft(float *a, float *b, int m, int iopt);
bool BuildFilter(float* wfilt, int nfft, int rowWidth, FILTER_TYPES filterType, float samplingFrequency);
bool LinRegress(float *x, float *y, int n, float *a, float *b, float *r);
void SetEscape(int iEscape);
int  makepow2(int val);