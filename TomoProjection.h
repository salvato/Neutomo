#pragma once
#include "projection.h"
class CTomoProjection :
  public CProjection
{
public:
  CTomoProjection(void);
  virtual ~CTomoProjection(void);

protected:
  bool ReadKeys(fitsfile* fitsFilePtr);
  bool WriteKeys(fitsfile* fitsFilePtr);

public:
  float f_angle;
};

