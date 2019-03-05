#pragma once

//***bd*** simple floating point point
class CPoint2f
{
public:
  CPoint2f();
  CPoint2f(float const x_, float const y_);
  CPoint2f(CPoint2f const &p);

  float Distance(CPoint2f const p2);
  float Distance(float const x_, float const y_);

  float x,y;
};
