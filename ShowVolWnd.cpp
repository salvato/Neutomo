#include "StdAfx.h"
#include "float.h"
#include <math.h>
#include "resource.h"
#include "ximage.h"
#include "NeuTomoUtil.h"
#include "Projection.h"
#include "ShowVolWnd.h"


CShowVolWnd::CShowVolWnd(CString Title, int xSize, int ySize, CWnd* pParent)
  : myTitle(Title)
  , x0(0), y0(0)
  , wdth(xSize), hght(ySize)
  , bVolumeLoaded(false)
  , MAX_3D_TEXTURESIZE(256)
  , slices(127)
  , TextureShown(1)
  , pMyParent(pParent)
  , bShowFrame(true)
{
  UINT cStyle= CS_DBLCLKS | CS_HREDRAW | CS_NOCLOSE | CS_VREDRAW;
  CBrush myBrush;
  myBrush.CreateSolidBrush(RGB(255, 255, 255));
  HBRUSH hbrBackground = HBRUSH(myBrush);
  HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  CString ClassName = AfxRegisterWndClass(cStyle, NULL, hbrBackground, hIcon); 
  DWORD exStyle = WS_EX_DLGMODALFRAME;
  DWORD Style = WS_OVERLAPPEDWINDOW;
  BOOL result = CreateEx(exStyle, ClassName, _T("Volume Window"), Style, 100, 100, wdth, hght, NULL, NULL, 0);
  if(!result) {
    AfxMessageBox(_T("CShowVolWnd CreateEx(...) failed"));
    PostQuitMessage(-1);
  } // if(!result)

  ZeroMemory(xPlane, 4*sizeof(*xPlane));
  ZeroMemory(yPlane, 4*sizeof(*yPlane));
  ZeroMemory(zPlane, 4*sizeof(*zPlane));
  xPlane[0] = yPlane[1] = zPlane[2] = 1.0f;

  ZeroMemory(clip0, sizeof(clip0));
  ZeroMemory(clip1, sizeof(clip1));
  ZeroMemory(clip2, sizeof(clip2));
  ZeroMemory(clip3, sizeof(clip3));
  ZeroMemory(clip4, sizeof(clip4));
  ZeroMemory(clip5, sizeof(clip5));
  clip0[3]=clip1[0]=clip1[3]=clip2[3]=clip3[3]=clip3[1]=clip4[3]=clip5[2]=clip5[3] = 1.0; 
  clip0[0]=clip2[1]=clip4[2] = -1.0;

  m_camera.Set(0, 0, 20, 0, 0, 0, 0, 1, 0);
  m_camera.MouseMode(CGrCamera::PITCHYAW);
  m_camera.Gravity(false);
  density   = 0.2;
  intensity = 0.1;

  Init();
}


CShowVolWnd::~CShowVolWnd(void)
{
}


BEGIN_MESSAGE_MAP(CShowVolWnd, COpenGLCtrl)
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_MESSAGE(MSG_LOAD_VOLUME, OnLoadVolume)
  ON_WM_SIZE()
END_MESSAGE_MAP()


void 
CShowVolWnd::OnPrepareRC() {
  int err = glewInit();
  if(err != GLEW_OK) {// sanity check
    char* cString = (char *)glewGetErrorString(err);
    _TCHAR* tString = new _TCHAR[strlen(cString)+1];
    mbstowcs(tString, cString, strlen(cString));
    tString[strlen(cString)] = 0;
    AfxMessageBox(tString, MB_ICONSTOP);
    delete[] tString;
  } // if(GLEW_OK != err)
}


void 
CShowVolWnd::OnDestroyRC() {
}


void
CShowVolWnd::CustomizePixelFormat(PIXELFORMATDESCRIPTOR* pfd) {
  pfd->cAlphaBits = 8;
  pfd->dwFlags |= PFD_DOUBLEBUFFER;
}


void 
CShowVolWnd::OnViewing() {
}


void 
CShowVolWnd::OnProjection(GLdouble AspectRatio) {
}


void 
CShowVolWnd::OnViewport(int cx, int cy) {
}


void 
CShowVolWnd::OnRender() {
  // Determine the aspect ratio
  GLdouble aspectratio = GLdouble(wdth) / GLdouble(hght);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // Set the camera parameters
  gluPerspective(m_camera.FieldOfView(), // Vertical field of view in degrees.
                 aspectratio,            // The aspect ratio.
                 1.,                     // Near clipping
                 100.);                 // Far clipping
  // and the camera location
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  m_camera.gluLookAt();

  // Enable depth test
  glEnable(GL_DEPTH_TEST);

  // Cull backfacing polygons
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  if(bVolumeLoaded) {
    ActualDraw();
  }

  if(bShowFrame) {
    //draw the box framing everything
    glColor3f(0.0f,    0.0f,  0.7f);
    glBegin(GL_LINES);
      glVertex3f(1.0f,   1.0f,  1.0f);
      glVertex3f(1.0f,   1.0f, -1.0f);
      glVertex3f(1.0f,  -1.0f,  1.0f);
      glVertex3f(1.0f,  -1.0f, -1.0f);
      glVertex3f(-1.0f,  1.0f,  1.0f);
      glVertex3f(-1.0f,  1.0f, -1.0f);
      glVertex3f(-1.0f, -1.0f,  1.0f);
      glVertex3f(-1.0f, -1.0f, -1.0f);
      glVertex3f(1.0f,   1.0f,  1.0f);
      glVertex3f(1.0f,  -1.0f,  1.0f);
      glVertex3f(1.0f,   1.0f, -1.0f);
      glVertex3f(1.0f,  -1.0f, -1.0f);
      glVertex3f(-1.0f,  1.0f,  1.0f);
      glVertex3f(-1.0f, -1.0f,  1.0f);
      glVertex3f(-1.0f,  1.0f, -1.0f);
      glVertex3f(-1.0f, -1.0f, -1.0f);
      glVertex3f(1.0f,   1.0f,  1.0f);
      glVertex3f(-1.0f,  1.0f,  1.0f);
      glVertex3f(1.0f,   1.0f, -1.0f);
      glVertex3f(-1.0f,  1.0f, -1.0f);
      glVertex3f(1.0f,  -1.0f,  1.0f);
      glVertex3f(-1.0f, -1.0f,  1.0f);
      glVertex3f(1.0f,  -1.0f, -1.0f);
      glVertex3f(-1.0f, -1.0f, -1.0f);
    glEnd();
  }
}

// This function does all of the actual OpenGL drawing.  
void
CShowVolWnd::ActualDraw() {

  //enable the alpha blending/testing
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, density*intensity);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  
  //set up the texture matrix
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glTranslatef(0.5f, 0.5f, 0.5f);
  glScalef(0.5f, 0.5f, 0.5f);
  glGetFloatv(GL_MODELVIEW_MATRIX, mat);
  invertMat(mat);
  mat[12]=0.0f;// Want rotation only, 
  mat[13]=0.0f;// zero the translation
  mat[14]=0.0f;// ...
  glMultMatrixf(mat);
  glTranslatef(0.0f, 0.0f, m_camera.CameraDistance());
  
  glEnable(GL_TEXTURE_3D_EXT);

  // Configure the clip planes
  // All points with eye coordinates (xe, ye, ze, we) that satisfy 
  // (A B C D)M^-1 (xe ye ze we)T >= 0 lie in the half-space defined by the plane
  // M is the modelview matrix at the time glClipPlane() is called
  glClipPlane(GL_CLIP_PLANE0, clip0);
  glClipPlane(GL_CLIP_PLANE1, clip1);
  glClipPlane(GL_CLIP_PLANE2, clip2);
  glClipPlane(GL_CLIP_PLANE3, clip3);
  glClipPlane(GL_CLIP_PLANE4, clip4);
  glClipPlane(GL_CLIP_PLANE5, clip5);

  glEnable(GL_CLIP_PLANE0);
  glEnable(GL_CLIP_PLANE1);
  glEnable(GL_CLIP_PLANE2);
  glEnable(GL_CLIP_PLANE3);
  glEnable(GL_CLIP_PLANE4);
  glEnable(GL_CLIP_PLANE5);

  //set modelview to identity
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();    

  //setup the texture coord generation
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

  //setup the tex gen
  glTexGenfv(GL_S, GL_EYE_PLANE, xPlane);
  glTexGenfv(GL_T, GL_EYE_PLANE, yPlane);
  glTexGenfv(GL_R, GL_EYE_PLANE, zPlane);
  
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);

  glBindTexture(GL_TEXTURE_3D_EXT, volTextures[TextureShown]);

  glTranslatef(0.0f, 0.0f, -m_camera.CameraDistance());

  //draw the slices
  glColor4f(1.0f, 1.0f, 1.0f, intensity);

  ASSERT(slices > 1);
  for(int i=0; i<slices; i++){
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -1.0f + (float)i*2.0f/(float)(slices-1));
    glBegin(GL_QUADS);
    for(int j=0; j<7; j++){
      for(int k=0; k<7; k++){
        glVertex3fv(points[j*8+k]);
        glVertex3fv(points[j*8+k+1]);
        glVertex3fv(points[(j+1)*8+k+1]);
        glVertex3fv(points[(j+1)*8+k]);
      }
    }
    glEnd();
    glPopMatrix();
  
  }
  glPopMatrix();

  glDisable(GL_CLIP_PLANE0);
  glDisable(GL_CLIP_PLANE1);
  glDisable(GL_CLIP_PLANE2);
  glDisable(GL_CLIP_PLANE3);
  glDisable(GL_CLIP_PLANE4);
  glDisable(GL_CLIP_PLANE5);

  glDisable(GL_ALPHA_TEST);

  glDisable(GL_TEXTURE_3D_EXT);
  
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_GEN_R);

}


void
CShowVolWnd::SetShownTexture(int Texture) {
  TextureShown = Texture;
  if(bVolumeLoaded) {
    RedrawOpenGLWindow();
  }
}


void
CShowVolWnd::SetShowFrame(int ShowFrame) {
  bShowFrame = (ShowFrame != 0);
    RedrawOpenGLWindow();
}


int
CShowVolWnd::GetShownTexture() {
  return TextureShown;
}


bool
CShowVolWnd::CopyPix(CProjection &Projection, CxImage* pImage, float fMin, float fMax) {
  float* pData;
  RGBTRIPLE *pRow; 
  int yOff;
  pData = Projection.pData;
  int xSize = Projection.n_columns;
  int ySize = Projection.n_rows;
  if(xSize != pImage->GetWidth()) return false;
  if(ySize != pImage->GetHeight()) return false;
  for(int y=0; y<ySize; y++) {
    pRow = (RGBTRIPLE *)pImage->GetBits(y);
    yOff = xSize * y;
    for(int x=0; x<xSize; x++) {
      (*(pRow+x)).rgbtRed = (*(pRow+x)).rgbtGreen = (*(pRow+x)).rgbtBlue = int(255.5*(*(pData+yOff+x)-fMin)/(fMax-fMin));
    }
  }
  return true;
}


int
CShowVolWnd::Init(void) {
  //texgen planes
  ZeroMemory(xPlane, 4*sizeof(*xPlane));
  ZeroMemory(yPlane, 4*sizeof(*yPlane));
  ZeroMemory(zPlane, 4*sizeof(*zPlane));
  xPlane[0] = yPlane[1] = zPlane[2] = 1.0f;
  
  //xform matrices
  ZeroMemory(objectXform, sizeof(objectXform));
  ZeroMemory(planeXform, sizeof(planeXform));
  objectXform[0] = objectXform[5] = objectXform[10] = objectXform[15] = 1.0f;
  planeXform[0]  = planeXform[5]  = planeXform[10]  = planeXform[15]  = 1.0f;

  //create the points used to draw the slices
  float temp;
  for(int i=0; i<8; i++) {
    for(int j=0; j<8; j++) {
      points[i*8+j][0]=(float)j/7.0f-0.5f;
      points[i*8+j][1]=(float)i/7.0f-0.5f;
      points[i*8+j][2]=-0.5f;
      temp = points[i*8+j][0]*points[i*8+j][0]+
             points[i*8+j][1]*points[i*8+j][1]+
             points[i*8+j][2]*points[i*8+j][2];
      temp = 1.0f / (float)sqrt(temp);
      points[i*8+j][0] *= temp;
      points[i*8+j][1] *= temp;
      points[i*8+j][2] *= temp;
      points[i*8+j][0] *= 2.0f;
      points[i*8+j][1] *= 2.0f;
      points[i*8+j][2] += 1.0f;
    }
  }

  return 0;
}// CVolumeWnd::Init(void)


void
CShowVolWnd::MapColor(GLubyte *color, float t) {
  t *= 2.0f; //scale to improve the distribution of the data
  color[0] = 255 * sin(t*3.1415927f*0.5f);
  color[1] = 255 * sin(t*3.1415927f);
  color[2] = 255 * fabs(sin(t*3.1415927f*2.0f));
  color[3] = 255 * sin(t*3.1415927f*0.5f);
}


afx_msg LRESULT
CShowVolWnd::OnLoadVolume(WPARAM wParam, LPARAM lParam) {

  CString sString;
  CClientDC dc(this);

  int xSize, ySize, zSize;// The slice's sizes
  GLubyte* pVolumeC  = NULL;// The Color and
  GLubyte* pVolumeBW = NULL;// B&W 3d Textures

  CProjection** pProjections = NULL;// The slice's array

  CString sDirectory = *((CString *)(lParam));// The slices directory 
  
  WIN32_FIND_DATA FileData;
  HANDLE hSearch;
  CString sPathName, sFileDati;

  CWaitCursor WaitCursor;

  sPathName = sDirectory + _T("\\*.fits");
  nProjections = 0;
  float fMin = FLT_MAX, fMax =-FLT_MAX;
  float rMin = FLT_MAX, rMax =-FLT_MAX;

  // How many Projections do we have ?
  hSearch = FindFirstFile(sPathName, &FileData);
  if(hSearch == INVALID_HANDLE_VALUE) {
    AfxMessageBox(_T("Invalid Path\n")+sPathName, MB_ICONSTOP);
    return false;
  }
  sFileDati = FileData.cFileName;
  CProjection ProjTemp;
  if(!ProjTemp.ReadFromFitsFile(sDirectory+_T("\\")+sFileDati)) {
    AfxMessageBox(ProjTemp.sErrorString, MB_ICONSTOP);
    return false;
  }
  xSize = ProjTemp.n_columns;
  ySize = ProjTemp.n_rows;
  ProjTemp.GetMinMax(&fMin, &fMax);
  nProjections++;
  while(FindNextFile(hSearch, &FileData)) {
    sFileDati = FileData.cFileName;
    if(!ProjTemp.ReadFromFitsFile(sDirectory+_T("\\")+sFileDati)) {
      AfxMessageBox(ProjTemp.sErrorString, MB_ICONSTOP);
      return false;
    }
    ProjTemp.GetMinMax(&rMin, &rMax);
    nProjections++;
    if(rMin < fMin) fMin = rMin;
    if(rMax > fMax) fMax = rMax;
  }
  FindClose(hSearch);

  if(nProjections == 0) {
    AfxMessageBox(_T("No Slice Files Found"), MB_ICONSTOP);
    return false;
  }
  zSize = nProjections;

  // Now we know how many projections we have and their size
  // Calculate the texture sizes as the NEAREST power of 2
  int xTex, yTex, zTex;
  xTex = 1 << int(ceil(log((double)xSize)/log(2.0)));
  yTex = 1 << int(ceil(log((double)ySize)/log(2.0)));
  //zTex = 1 << int(ceil(log((double)zSize)/log(2.0)));
  zTex = zSize;

  // But limited to MAX_3D_TEXTURESIZE for memory reasons
  xTex = min(xTex, MAX_3D_TEXTURESIZE);
  yTex = min(yTex, MAX_3D_TEXTURESIZE);
  //zTex = min(zTex, MAX_3D_TEXTURESIZE);

  CxImage** pImages = new CxImage*[nProjections];
  for(int i=0; i<nProjections; i++) {
    pImages[i] = new CxImage(xSize, ySize, 24);
  }

  // Let's read the projections in Memory
  hSearch = FindFirstFile(sPathName, &FileData);
  int nFiles = 0;
  try {
    do{
      sFileDati = FileData.cFileName;
      sString.Format(_T("Reading File: %s"), sFileDati);
      if(pMyParent != NULL)
        pMyParent->SendMessage(MSG_STATUS_MESSAGE, 0, (LPARAM)&sString);
      if(!ProjTemp.ReadFromFitsFile(sDirectory+_T("\\")+sFileDati)) {
        AfxMessageBox(ProjTemp.sErrorString, MB_ICONSTOP);
        FindClose(hSearch);
        goto ErrorExit;
      }
      if(!CopyPix(ProjTemp, pImages[nFiles], fMin, fMax)) {
        AfxMessageBox(_T("Error Copying Pixel Data"), MB_ICONSTOP);
        FindClose(hSearch);
        goto ErrorExit;
      }
      pImages[nFiles]->Resample2(xTex, yTex);
      nFiles++;// One more file
    } while(FindNextFile(hSearch, &FileData));
  } catch(...) {
    AfxMessageBox(_T("Error Reading Slice Files"), MB_ICONSTOP);
    FindClose(hSearch);
    goto ErrorExit;
  }
  FindClose(hSearch);

  try {
    pVolumeC  = new BYTE[xTex*yTex*zTex*4];
    pVolumeBW = new BYTE[xTex*yTex*zTex];
  } catch(...) {
    AfxMessageBox(_T("No RAM for Volume"), MB_ICONSTOP);
    goto ErrorExit;
  }

  if(pVolumeC == NULL || pVolumeBW == NULL) {
    AfxMessageBox(_T("No RAM for Volume"), MB_ICONSTOP);
    goto ErrorExit;
  }

  ZeroMemory(pVolumeC,  xTex*yTex*zTex*4);
  ZeroMemory(pVolumeBW, xTex*yTex*zTex);

  //int xM = min(xTex, xSize);
  //int yM = min(yTex, ySize);
  //int zM = min(zTex, zSize);

  int xM = xTex;
  int yM = yTex;
  int zM = min(zTex, zSize);

  int iOffC, iOffBW;
  BYTE *pPlaneC, *pPlaneBW;
  float pVal;
  RGBTRIPLE* pRow;
  for(int z=0; z<zM; z++) {
    sString.Format(_T("Building 3D-Texture Slice %d/%d"), z, zM);
    if(pMyParent != NULL) 
      pMyParent->SendMessage(MSG_STATUS_MESSAGE, 0, (LPARAM)&sString);
    pPlaneC  = pVolumeC  + z*xTex*yTex*4;
    pPlaneBW = pVolumeBW + z*xTex*yTex;
    for(int y=0; y<yM; y++) {
      pRow = (RGBTRIPLE*)pImages[z]->GetBits(y);
      iOffC   = y*xTex*4;
      iOffBW  = y*xTex;
      for(int x=0; x<xM; x++) {
        pVal = float((*(pRow+x)).rgbtBlue);
        pVal = (pVal-0.0)/(255.0-0.0);
        MapColor((pPlaneC+iOffC+(4*x)), pVal);
        *(pPlaneBW+iOffBW+x) = 255.0f*sin(pVal*3.1415927f/2.0f);
      }
    }
  }

  // setup the 3D textures
	// request 2 texture names from OpenGL
  glGenTextures(2, volTextures);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// tell OpenGL we're going to be setting up the texture name it gave us	
  glBindTexture(GL_TEXTURE_3D, volTextures[0]);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // when this texture needs to be magnified to fit on a big polygon,
  // use linear interpolation of the texels to determine the color
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// when this texture needs to be shrunk to fit on small polygons,
  // use linear interpolation of the texels to determine the color
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexImage3D(GL_TEXTURE_3D, 0, GL_ALPHA8, xTex, yTex, zTex, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pVolumeBW);

	// tell OpenGL we're going to be setting up the texture name it gave us	
  glBindTexture(GL_TEXTURE_3D, volTextures[1]);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // when this texture needs to be magnified to fit on a big polygon,
  // use linear interpolation of the texels to determine the color
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// when this texture needs to be shrunk to fit on small polygons,
  // use linear interpolation of the texels to determine the color
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, xTex, yTex, zTex, 0, GL_RGBA, GL_UNSIGNED_BYTE, pVolumeC);
  
  CheckGLErrors(_T("glTexImage3D()"));

  if(pVolumeC != NULL) delete[] pVolumeC; pVolumeC = NULL;
  if(pVolumeBW != NULL) delete[] pVolumeBW; pVolumeBW = NULL;

  if(pImages != NULL) {
    for(int i=0; i<nProjections; i++) {
      if(pImages[i] != NULL)
        delete pImages[i];
    }
    delete[] pImages;
    pImages = NULL;
  }
  bVolumeLoaded = true;
  RedrawOpenGLWindow();
  return true;

ErrorExit:
  if(pVolumeC != NULL) delete[] pVolumeC; pVolumeC = NULL;
  if(pVolumeBW != NULL) delete[] pVolumeBW; pVolumeBW = NULL;
  if(pImages != NULL) {
    for(int i=0; i<nProjections; i++) {
      if(pImages[i] != NULL)
        delete pImages[i];
    }
    delete[] pImages;
    pImages = NULL;
  }
  return false;
}


void
CShowVolWnd::OnSize(UINT nType, int width, int height) {
  wdth = width;
  hght = height;
  glViewport(0, 0, (GLsizei)wdth, (GLsizei)hght);
}


void
CShowVolWnd::OnRButtonDown(UINT nFlags, CPoint point) {
  m_camera.MouseDown(point.x, point.y);
  m_camera.MouseMode(CGrCamera::ROLLMOVE);
}


void
CShowVolWnd::OnRButtonUp(UINT nFlags, CPoint point) {
  m_camera.MouseMode(CGrCamera::PITCHYAW);
}


void
CShowVolWnd::OnMouseMove(UINT nFlags, CPoint point) {
  if(nFlags & MK_LBUTTON) {
     m_camera.MouseMove(point.x, point.y);
     RedrawOpenGLWindow();
   } else if(nFlags & MK_RBUTTON) {
     m_camera.MouseMove(point.x, point.y);
     RedrawOpenGLWindow();
  } 
}


void
CShowVolWnd::OnLButtonDown(UINT nFlags, CPoint point) {
  m_camera.MouseDown(point.x, point.y);
}


void 
CShowVolWnd::SetDensity(float newDensity) {
  density = newDensity;
  if(density > 1.0) density = 1.0;
  if(density < 0.0) density = 0.0;
  if(bVolumeLoaded) {
    RedrawOpenGLWindow();
  }
}


float 
CShowVolWnd::GetDensity() {
  return density;
}


void 
CShowVolWnd::SetIntensity(float newIntensity) {
  intensity = newIntensity;
  if(intensity > 1.0) intensity = 1.0;
  if(intensity < 0.0) intensity = 0.0;
  if(bVolumeLoaded) {
    RedrawOpenGLWindow();
  }
}


float 
CShowVolWnd::GetIntensity() {
  return intensity;
}


void 
CShowVolWnd::SetSliceN(float nSlices) {
  slices = nSlices;
  if(slices < 1) slices = 1;
  if(bVolumeLoaded) {
    RedrawOpenGLWindow();
  }
}


int 
CShowVolWnd::GetSliceN() {
  return slices;
}

