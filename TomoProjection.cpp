#include "StdAfx.h"
#include "TomoProjection.h"


CTomoProjection::CTomoProjection(void)
{
}


CTomoProjection::~CTomoProjection(void) {
}


bool
CTomoProjection::ReadKeys(fitsfile* fitsFilePtr) {
  //int iStatus=0;
  //char comment[81];
  //int iRes;

  //iRes = fits_read_key(fitsFilePtr, TFLOAT, "GATEDELY", &f_TimeOfFlight, comment, &iStatus);
  //if((iRes != VALUE_UNDEFINED) && (iRes != KEY_NO_EXIST)) {
  //  f_TimeOfFlight *= 1.0E-15f;
  //}
  //iStatus = 0;
  
  //iRes=fits_read_key(fitsFilePtr, TFLOAT, "TOF", &f_TimeOfFlight, comment, &iStatus);
  //if((iRes != VALUE_UNDEFINED) && (iRes != KEY_NO_EXIST)) {
  //  f_TimeOfFlight *= 1.0e6f;
  //}
  //iStatus = 0;

  //iRes = fits_read_key(fitsFilePtr, TFLOAT, "ANGPOS", &f_angle, comment, &iStatus);
  //iStatus = 0;

  //iRes = fits_read_key(fitsFilePtr, TFLOAT, "CURRENT", &f_BeamIntensity, comment, &iStatus);
  //iStatus = 0;

  return true;
}


bool
CTomoProjection::WriteKeys(fitsfile* fitsFilePtr) {
  int iStatus=0;
  //char comment[81];
  // write an optional keyword to the header
  // Note that the ADDRESS of the value is passed in the routine
  //int exposure = 1500.0;
  //strcpy_s(comment, sizeof(comment), "Total Exposure Time");
  //if(fits_update_key(fitsFilePtr, TLONG, "EXPOSURE", &exposure, comment, &iStatus)) {
  //  SetErrorString(iStatus);           
  //  return false;
  //}
  return true;
}

