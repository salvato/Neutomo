#pragma once

#include "fitsio.h"// Fits image formats


class 
CProjection {
public:
  CProjection(void);
  CProjection(int nColumns, int nRows);
  virtual ~CProjection(void);

private:
  float GetPixelColorInterpolated(float x, float y, float rplColor);
  float GetPixelColorWithOverflow(int x, int y, float rplColor);
  bool GaussianBlur(float radius=1.0f, CProjection* iDst=NULL);

protected:
  void bSort(float *a, int n);
  int gen_convolve_matrix (float radius, float **cmatrix_p);
  void blur_line (float *cmatrix, int cmatrix_length, float* src_row, float* dest_row, int nColumns);
  void blur_column(float *cmatrix, int cmatrix_length, float* src_col, float* dest_col, int nRows, int nColumns);
  void SetErrorString(const int status);
  virtual bool ReadKeys(fitsfile* fitsFilePtr);
  virtual bool WriteKeys(fitsfile* fitsFilePtr);

public:
  int		n_rows; 
  int		n_columns; 
  const float FLOAT_UNDEFINED;
  const int   INT_UNDEFINED;
  float* pData;
  float f_angle;
  float f_BeamIntensity;
  float f_MedianThreshold;
  float f_TimeOfFlight;
  float f_PosX, f_PosY, f_PosZ;
  int   i_SliceNum;
  CString sErrorString;

  virtual bool Reset(void);

  bool ReadFromFitsFile(CString sPathName);
  bool WriteToFitsFile(CString sDirectory, CString sFileName);

  bool IsInitialized(void);
  bool InitFromData(float* pDatain, int nRows, int nColumns);
  bool Normalize(float fMin, float fMax);
  bool Median(int Ksize);
  bool Copy(CProjection& P2Copy);
  bool Sum(CProjection& P2Add);
  bool Subtract(CProjection& P2Subtract);
  bool Crop(int xStart, int xEnd, int yStart, int yEnd);
  bool LinTransform(float factor, float offset);
  bool GetMinMax(float* min, float* max);
  bool GetPixelsSum(float* PixelsSum);
  bool ClearImage(void);

  bool FlatFieldCorr(CProjection& Beam, CProjection& Dark);
  bool FlatFieldCorr(CProjection& Beam);
  bool OpenBeamCorr(int x0, int x1, int y0, int y1);
  bool OpenBeamCorr(float fBeam);

  bool VerticalFlip(void);
  bool HorizontalFlip(void);
  bool Rotate(float angle);// In radians
  bool ShiftRows(int nRows);
};
