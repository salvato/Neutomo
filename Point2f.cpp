#include "StdAfx.h"
#include "Point2f.h"
#include "math.h"

CPoint2f::CPoint2f() {
  x=y=0.0f;
}


CPoint2f::CPoint2f(float const x_, float const y_) {
  x=x_;
  y=y_;
}


CPoint2f::CPoint2f(CPoint2f const &p) {
  x=p.x;
  y=p.y;
}


float 
CPoint2f::Distance(CPoint2f const p2) {
  return (float)sqrt((x-p2.x)*(x-p2.x)+(y-p2.y)*(y-p2.y));
}


float 
CPoint2f::Distance(float const x_, float const y_) {
  return (float)sqrt((x-x_)*(x-x_)+(y-y_)*(y-y_));
}

