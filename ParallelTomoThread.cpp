#include "stdafx.h"
#include "float.h"
#include <process.h> // _beginthread, _endthread
#include "resource.h"
#include "MsgWindow.h"
#include "NeuTomoUtil.h"
#include "NeuTomoDlg.h"
#include "Projection.h"
#include "ParallelTomoThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//extern CMsgWindow* pMsg;

GLenum GL_TEXTURE_target             = GL_TEXTURE_RECTANGLE_ARB;;
GLenum GL_TEXTURE_internalformat     = GL_LUMINANCE32F_ARB;
GLenum GL_TEXTURE_texFormat		       = GL_LUMINANCE;
GLenum GL_FILTER_mode                = GL_LINEAR;

// Cg vars and functions
CGcontext cgContext;
CGprofile fragmentProfile;
CGprogram fragmentProgram;
CGparameter NewTexture, OldTexture;

void cgErrorCallback();
bool CheckForCgError(CString Situation);


void
DoParallelTomo(void* pParent) {
  CString sTempPath = _T("\\Windows\\Temp\\File_");
  CString sString, sFileOut;
  int newTex, newSumTex, oldSumTex;
  int nProjTex;
  CString sVersion;
  bool bEnoughMemory = true;
  int nProjInMemory, lastAttachedTexture;

  GLuint glutWindowHandle = 0;// handle to offscreen "window"
  GLuint theList;// Display List
  GLfloat eyePlaneS[4], eyePlaneT[4], eyePlaneR[4];
  GLuint outTexID[3], *inTexID=NULL;

  ZeroMemory(eyePlaneS, 4*sizeof(eyePlaneS[0]));
  ZeroMemory(eyePlaneT, 4*sizeof(eyePlaneT[0]));
  ZeroMemory(eyePlaneR, 4*sizeof(eyePlaneR[0]));
  eyePlaneS[0] = eyePlaneT[1] = eyePlaneR[2] = 1.0;

  GLenum attachmentpoints[3];
  attachmentpoints[0] = GL_COLOR_ATTACHMENT0;
  attachmentpoints[1] = GL_COLOR_ATTACHMENT1;
  attachmentpoints[2] = GL_COLOR_ATTACHMENT2;
  
  newSumTex = 0;
  oldSumTex = 1;
  newTex    = 2;

  CProjection Projection;
	CNeuTomoDlg *pDlg = (CNeuTomoDlg *)pParent;

  pDlg->EnableStopButton();

  clock_t start = clock();

  int m    = int(ceil(log(2.0*pDlg->GetImgWidth())/log(2.0)));
  int nfft = 1 << m;
  float *wfilt  = new float[nfft];
  float samplingFrequency = float(pDlg->GetImgWidth())/float(pDlg->GetDetectorWidth());// In m^-1 is this correct?
  if(!BuildFilter(wfilt, nfft, pDlg->GetImgWidth(), pDlg->GetFilterType(), samplingFrequency)) {
    delete[] wfilt;
    return;
  }
  for(int i=0; i<nfft; i++) {
    wfilt[i] *= M_PI/float(pDlg->GetNProjections()-1);//Scale factor
  }

  _glutInit();

  glutWindowHandle = glutCreateWindow("Hidden Window");
  if(glutWindowHandle == 0) {
    AfxMessageBox(_T("Unable to Create OpenGL Window"), MB_ICONSTOP);
    pDlg->bDoTomoEnabled = false;
    if(wfilt   != NULL) delete[] wfilt; wfilt = NULL;
    return;
  }

  int err = glewInit();
  if(err != GLEW_OK) {// sanity check
    AfxMessageBox(CString(glewGetErrorString(err)), MB_ICONSTOP);
    pDlg->bDoTomoEnabled = false;
    glutDestroyWindow(glutWindowHandle);
    if(wfilt) delete[] wfilt; wfilt = NULL;
    return;
  }

  int texSizeX = min(pDlg->GetVolXSize(), pDlg->GetImgWidth());
  int texSizeY = int(float(pDlg->GetImgHeight())*float(texSizeX)/float(pDlg->GetImgWidth())+0.5);
  int texSizeZ = texSizeX;

  CProjection Proj2Save(texSizeX, texSizeY);
  GLuint fbo;

  //Setting up the Cg Runtime
  if(!initCG()) {
    pDlg->bDoTomoEnabled = false;
    if(fragmentProgram) cgDestroyProgram(fragmentProgram);
    cgDestroyContext(cgContext);
    glutDestroyWindow(glutWindowHandle);
    if(wfilt   != NULL) delete[] wfilt; wfilt = NULL;
    return;
  }

  if(!initFBO(&fbo)) {
    AfxMessageBox(_T("Unable to Initialize FBO !"), MB_ICONSTOP);
    pDlg->bDoTomoEnabled = false;
    cgDestroyProgram(fragmentProgram);
    cgDestroyContext(cgContext);
    glutDestroyWindow(glutWindowHandle);
    if(wfilt   != NULL) delete[] wfilt; wfilt = NULL;
    return;
  }
  pDlg->ShowStatusMsg(_T("Creating Textures"));

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  // Create the 3 textures for the FBO
  glGenTextures(3, outTexID);
  if(!initFBOTextures(3, outTexID, texSizeX, texSizeY, attachmentpoints)) {
    pDlg->bDoTomoEnabled = false;
    cgDestroyProgram(fragmentProgram);
    cgDestroyContext(cgContext);
    glutDestroyWindow(glutWindowHandle);
    if(wfilt   != NULL) delete[] wfilt; wfilt = NULL;
    return;
  }

  // Create the projective Textures
  nProjTex = pDlg->GetNProjections()-1;
  inTexID = new GLuint[nProjTex];
  glGenTextures(nProjTex, inTexID);
  if(!initProjTextures(nProjTex, inTexID)) {
    AfxMessageBox(_T("Unable to initialize Projective Textures"), MB_ICONSTOP);
    pDlg->bDoTomoEnabled = false;
    cgDestroyProgram(fragmentProgram);
    cgDestroyContext(cgContext);
    glutDestroyWindow(glutWindowHandle);
    if(wfilt   != NULL) delete[] wfilt; wfilt = NULL;
    if(inTexID != NULL) delete[] inTexID; 
    return;
  }

  glTexGenfv(GL_S, GL_EYE_PLANE, eyePlaneS);
  glTexGenfv(GL_T, GL_EYE_PLANE, eyePlaneT);
  glTexGenfv(GL_R, GL_EYE_PLANE, eyePlaneR);
  if(!CheckGLErrors(_T("glTexGenfv"))){
    pDlg->bDoTomoEnabled = false;
    cgDestroyProgram(fragmentProgram);
    cgDestroyContext(cgContext);
    glutDestroyWindow(glutWindowHandle);
    if(wfilt   != NULL) delete[] wfilt; wfilt = NULL;
    if(inTexID != NULL) delete[] inTexID; 
    return;
  }

  // Projective Texture Matrices
  GLfloat* transformMats = new GLfloat[(pDlg->GetNProjections()-1)*16];
  GLfloat* pMat;
  glMatrixMode(GL_TEXTURE);
  for(int nProj=0; nProj<pDlg->GetNProjections()-1; nProj++) {
    pMat = transformMats+(nProj<<4);
    glLoadIdentity();
    glTranslatef(0.5*pDlg->GetImgWidth()-pDlg->rotationCenter, 0.5*pDlg->GetImgHeight(), -1.0);
    glRotatef(pDlg->tiltAngle, 0.0, 0.0, 1.0);
    glRotatef(pDlg->GetAngles()[nProj], 0.0, 1.0, 0.0);
    glTranslatef(-0.5*pDlg->GetImgWidth(), -0.5*pDlg->GetImgHeight(), 0.0);
    glGetFloatv(GL_TEXTURE_MATRIX, pMat);
  }

  float *pImage, *pRow1, *pRow2;
  float *pr1    = new float[nfft];
  float *pr2    = new float[nfft];

  int zeroPad1 = (nfft-pDlg->GetImgWidth())*sizeof(*pr1);
  int zeroPad2 = nfft*sizeof(*pr2);
  int rowSize  = pDlg->GetImgWidth()*sizeof(*pRow1);
  
  glEnable(GL_TEXTURE_target);

/*
typedef struct _MEMORYSTATUSEX {
  DWORD     dwLength;
  DWORD     dwMemoryLoad;
  DWORDLONG ullTotalPhys;
  DWORDLONG ullAvailPhys;
  DWORDLONG ullTotalPageFile;
  DWORDLONG ullAvailPageFile;
  DWORDLONG ullTotalVirtual;
  DWORDLONG ullAvailVirtual;
  DWORDLONG ullAvailExtendedVirtual;
} MEMORYSTATUSEX, *LPMEMORYSTATUSEX;
*/
  MEMORYSTATUSEX MemStat;
  memset(&MemStat, 0, sizeof(MemStat));// Zero structure
  MemStat.dwLength = sizeof(MemStat);
  ::GlobalMemoryStatusEx(&MemStat);// Get RAM snapshot
  int availableRam_kb = MemStat.ullAvailPhys/1024;
  availableRam_kb /= 3;//To Leave some space for others
  GLint cur_avail_mem_kb = 0;
  glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &cur_avail_mem_kb);
  cur_avail_mem_kb += availableRam_kb;

  int textureSize_kb = pDlg->GetImgWidth()*pDlg->GetImgHeight()*sizeof(*pImage)/1024;
  int MaxNumProjInMemory = int(cur_avail_mem_kb/textureSize_kb);// To allow some space
  MaxNumProjInMemory -= 3;// To allow some other space
  if(MaxNumProjInMemory < 2) {
    AfxMessageBox(_T("Too Big Textures !"));
    return;
  }

  lastAttachedTexture = 0;
  nProjInMemory = 0;
  for(int nProj=0; nProj<pDlg->GetNProjections()-1; nProj++) {// Exclude the 180� one...
    if(pDlg->GetEscape()) {
      cgDestroyProgram(fragmentProgram);
      cgDestroyContext(cgContext);
      glutDestroyWindow(glutWindowHandle);
      if(inTexID != NULL) delete[] inTexID; 
      if(wfilt   != NULL) delete[] wfilt; wfilt = NULL;
      if(pr1     != NULL) delete[] pr1;
      if(pr2     != NULL) delete[] pr2;
      for(int i=0; i<nProj; i++) {
        CString sFileTemp;
        sFileTemp.Format(_T("%s%004d.fft"), sTempPath, i);
        if(!DeleteFile(sFileTemp)) {
          DWORD Error = GetLastError();
          CString Value = _T("Unable to Delete ");
          Value += sFileTemp;
          AfxMessageBox(Value);
        }
      }
      if(transformMats != NULL) delete[] transformMats;
      return;
    }// if(pDlg->GetEscape())

    sString.Format(_T("Filtering Projection # %d at %.3f�"), nProj, pDlg->GetAngles()[nProj]);
    pDlg->ShowStatusMsg(sString);
    Projection.ReadFromFitsFile(pDlg->GetPathNames()[nProj]);
    pImage = Projection.pData;

    for(int nRow=0; nRow<pDlg->GetImgHeight(); nRow++) {
      pRow1 = pImage + nRow*pDlg->GetImgWidth();
      memcpy(pr1, pRow1, rowSize);
      ZeroMemory(pr1+pDlg->GetImgWidth(), zeroPad1);//Zero padding
      ZeroMemory(pr2, zeroPad2);
      if(++nRow < pDlg->GetImgHeight()) {
        pRow2 = pImage + nRow*pDlg->GetImgWidth();
        memcpy(pr2, pRow2, rowSize);
      } // if(++nRow < pDlg->GetImgHeight())
      fft(pr1, pr2, m, 0);// One call two transforms !
		  for(int i=0; i<nfft; i++) {//Filtering...
			  pr1[i] *= wfilt[i];
			  pr2[i] *= wfilt[i];
		  }
		  fft(pr1, pr2, m, 1);//Inverse Transform
      memcpy(pRow1, pr1, rowSize);
      if(nRow < pDlg->GetImgHeight())
        memcpy(pRow2, pr2, rowSize);
    } // for(int nRow=0; nRow<pDlg->GetImgHeight(); nRow++)
    
    if(pDlg->GetFilterSinograms()) {
      Projection.f_MedianThreshold = 0.0;
      Projection.Median(3);
    }

    if(bEnoughMemory) {
      // Try to transfer the Image to Texture Memory
      glBindTexture(GL_TEXTURE_target, inTexID[nProj]);
      glTexImage2D(GL_TEXTURE_target, 0, GL_TEXTURE_internalformat, pDlg->GetImgWidth(), pDlg->GetImgHeight(), 0, GL_TEXTURE_texFormat, GL_FLOAT, pImage);
      nProjInMemory++;
      //if(!CheckGLErrors(_T("Projective Texture Loading"))) {//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      if(nProjInMemory >= MaxNumProjInMemory) {
        bEnoughMemory = false;
        lastAttachedTexture = nProj;
        //sString.Format(_T("Memory for %d Projections\r\n"), nProjInMemory);
        //pMsg->AddText(sString);
      }
    }// if(bEnoughMemory)

    CString sFileTemp;
    sFileTemp.Format(_T("%s%004d.fft"), sTempPath, nProj);
    FILE* outStream = _tfopen(sFileTemp, _T("wb"));
    size_t nToWrite = pDlg->GetImgHeight()*pDlg->GetImgWidth();
    size_t nWritten = fwrite(pImage, sizeof(*pImage), nToWrite, outStream);
    fclose(outStream);
    if(nWritten != nToWrite) {
      AfxMessageBox(_T("Error Writing Temp File: ") + sFileTemp);
      return;//<<<<<<<<<<<<<<<<<<<<<<< Gestire la condizione !!!!!!!!!!!!!!!!!!!!!!
    }
  }// for(int nProj=0; nProj<pDlg->GetNProjections()-1; nProj++)
  if(!CheckGLErrors(_T("Projective Texture Loading"))) {
      return;//<<<<<<<<<<<<<<<<<<<<<<< Gestire la condizione !!!!!!!!!!!!!!!!!!!!!!
  }
  clock_t fft_end = clock();
  sString.Format(_T("Filtered %d Slices With Dimensions:%dx%d in %f[sec]"), 
                 texSizeZ, texSizeX, texSizeY, (fft_end-start)/1000.0);
  pDlg->ShowStatusMsg(sString);

  if(pr1    != NULL) delete[] pr1; pr1 = NULL;
  if(pr2    != NULL) delete[] pr2; pr2 = NULL;
  if(wfilt  != NULL) delete[] wfilt; wfilt = NULL;
  
  float dZ = float(pDlg->GetImgWidth())/float(texSizeZ);
  glClearColor(0.0, 0.0, 0.0, 1.0);

  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glEnable(GL_TEXTURE_target);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, pDlg->GetImgWidth(), 0.0, pDlg->GetImgHeight(), -pDlg->GetImgWidth(), pDlg->GetImgWidth());
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

	glPolygonMode(GL_FRONT, GL_FILL);
  GLfloat zPlane;
    
  float fMin = FLT_MAX, fMax =-FLT_MAX;

  for(int i=0; i<3; i++) {
    glDrawBuffer(attachmentpoints[i]);// Clear all the texture to be prepared
    glClear(GL_COLOR_BUFFER_BIT);     // Is that Needed ?
  }

  theList = glGenLists(1);
  glNewList(theList, GL_COMPILE);
    glLoadIdentity();
    glBegin(GL_QUADS);
      glVertex3f(0.0,      0.0,       0.0);
      glVertex3f(0.0,      texSizeY, 0.0);
      glVertex3f(texSizeX, texSizeY, 0.0);
      glVertex3f(texSizeX, 0.0,       0.0);
    glEnd();
  glEndList();

  // Calculation of the renderable cylindrical region
  // Must be corrected for the aspect ratio <<<<<<<<<<<<<<<<<<<<<<<<<<<
  float fLimit, r, rl, rr;
  r = 0.5*float(pDlg->GetImgWidth()-1.0);
  int iExclude = int(floor(fabs(pDlg->rotationCenter)*dZ))+1;

  Proj2Save.ClearImage();

  cgGLEnableProfile(fragmentProfile); // The profile must be enabled for the binding to work
  cgGLBindProgram(fragmentProgram);   // Bind the program
  cgGLDisableProfile(fragmentProfile);// Disable profile at first

  for(int iPlane=0; iPlane<texSizeZ; iPlane++) {// Costruiamo i vari piani del volume.
    if(pDlg->GetEscape()) {// Diamo la possibilit� di bloccare il lento processo di ricostruzione
      cgDestroyProgram(fragmentProgram);
      cgDestroyContext(cgContext);
      glutDestroyWindow(glutWindowHandle);
      if(inTexID != NULL) delete[] inTexID; 
      if(transformMats != NULL) delete[] transformMats;
      for(int nProj=0; nProj<pDlg->GetNProjections()-1; nProj++) {
        CString sFileTemp;
        sFileTemp.Format(_T("%s%004d.fft"), sTempPath, nProj);
        if(!DeleteFile(sFileTemp)) {
          DWORD Error = GetLastError();
          CString Value = _T("Unable to Delete ");
          Value += sFileTemp;
          AfxMessageBox(Value);
        }
      }
      return;
    }// if(pDlg->GetEscape())
    sString.Format(_T("BackProjecting Plane # %d/%d"), iPlane+1, texSizeX);
    pDlg->ShowStatusMsg(sString);
    zPlane = dZ*float(iPlane) - r;// Volume plane Z coordinate 
    fLimit = ((r-iExclude)*(r-iExclude))-(zPlane*zPlane);// Region inside the cylinder
    if(fLimit > 0.0) {
      fLimit = sqrt(fLimit);// Cord Lenght
      rl = r - fLimit;
      rr = r + fLimit;

      for(int nProj=0; nProj<pDlg->GetNProjections()-1; nProj++) {
        if(nProj > lastAttachedTexture) {
          for(int i=0; i<nProjInMemory; i++) {
            lastAttachedTexture++;
            CString sFileTemp;
            sFileTemp.Format(_T("%s%004d.fft"), sTempPath, lastAttachedTexture);
            FILE* inStream = _tfopen(sFileTemp, _T("rb"));
            size_t nToRead = pDlg->GetImgHeight()*pDlg->GetImgWidth();
            size_t nRead = fread(pImage, sizeof(*pImage), nToRead, inStream);
            fclose(inStream);
            if(nRead != nToRead)
              AfxMessageBox(_T("Error Reading Temp File: ") + sFileTemp);
            glBindTexture(GL_TEXTURE_target, inTexID[i]);
            glTexImage2D(GL_TEXTURE_target, 0, GL_TEXTURE_internalformat, pDlg->GetImgWidth(), pDlg->GetImgHeight(), 0, GL_TEXTURE_texFormat, GL_FLOAT, pImage);
            if(!CheckGLErrors(_T("Projective Texture Loading"))) {
              AfxMessageBox(_T("Gestire_Errore()"));
            }
            if(lastAttachedTexture == pDlg->GetNProjections()-2)
              break;
          }// for(int i=0; i<nProjInMemory; i++)
          //sString.Format(_T("Last Attached Texture= %d/%d\r\n"), lastAttachedTexture, pDlg->GetNProjections()-1); 
          //pMsg->AddText(sString);
        }// if(nProj > lastAttachedTexture)
        glViewport(0, 0, texSizeX, texSizeY);  // Set the final dimension
        glBindTexture(GL_TEXTURE_target, inTexID[nProj%nProjInMemory]);// The texture to be rendered
        glDrawBuffer(attachmentpoints[newTex]);// Set the Render Destination
        glClear(GL_COLOR_BUFFER_BIT);
        pMat = transformMats+(nProj<<4);
        glMatrixMode(GL_TEXTURE);// Just to be sure...
        glLoadMatrixf(pMat);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_QUADS);//image rendering
          glVertex3f(rl, 0.0,       zPlane);
          glVertex3f(rl, pDlg->GetImgHeight(), zPlane);
          glVertex3f(rr, pDlg->GetImgHeight(), zPlane);
          glVertex3f(rr, 0.0,       zPlane);
        glEnd();
        glFinish();
        glDrawBuffer(attachmentpoints[newSumTex]);
        // Parallel projection for 1:1 pixel=texture mapping
        glViewport(0, 0, pDlg->GetImgWidth(), pDlg->GetImgHeight());
        // Enable fragment program
        cgGLEnableProfile(fragmentProfile);
        cgGLSetTextureParameter(NewTexture, outTexID[newTex]);
        cgGLSetTextureParameter(OldTexture, outTexID[oldSumTex]);
        cgGLEnableTextureParameter(NewTexture);
        cgGLEnableTextureParameter(OldTexture);
        // Execute the progam
        glCallList(theList);
        // Exchange the textures
        newSumTex = 1-newSumTex;
        oldSumTex = 1-oldSumTex;
        //Ensure all commands executed
        glFinish();
        cgGLDisableProfile(fragmentProfile);
        #ifdef  _DEBUG
          CheckGLErrors(_T("Step"));
        #endif
      } // for(nProj=0; nProj<nProjections; nProj++)

      // Plane Done ! get GPU result
      glReadBuffer(attachmentpoints[newSumTex]);
      glReadPixels(0, 0, texSizeX, texSizeY, GL_TEXTURE_texFormat, GL_FLOAT, Proj2Save.pData);
      //Clear texture to prepare for the next Plane
      glDrawBuffer(attachmentpoints[oldSumTex]);
      glClear(GL_COLOR_BUFFER_BIT);

      sFileOut.Format(_T("Slice-%004d.fits"), iPlane);
      Proj2Save.i_SliceNum = iPlane;
      if(!Proj2Save.WriteToFitsFile(pDlg->GetSliceDir(), sFileOut)) {
        AfxMessageBox(Proj2Save.sErrorString);
        cgDestroyProgram(fragmentProgram);
        cgDestroyContext(cgContext);
        glutDestroyWindow(glutWindowHandle);
        if(inTexID != NULL) delete[] inTexID; 
        if(transformMats != NULL) delete[] transformMats;
        return;
      }
      glFinish();
    } else {// if(fLimit > 0.0)
      Proj2Save.ClearImage();
      sFileOut.Format(_T("Slice-%004d.fits"), iPlane);
      Proj2Save.i_SliceNum = iPlane;
      if(!Proj2Save.WriteToFitsFile(pDlg->GetSliceDir(), sFileOut)) {
        AfxMessageBox(Proj2Save.sErrorString);
        cgDestroyProgram(fragmentProgram);
        cgDestroyContext(cgContext);
        glutDestroyWindow(glutWindowHandle);
        if(inTexID != NULL) delete[] inTexID; 
        if(transformMats != NULL) delete[] transformMats;
        return;
      }
    }// if(fLimit > 0.0)
    if(!CheckGLErrors(_T("Plane Projection"))) {
      break;
    }
    if((iPlane < texSizeZ-1) &&(MaxNumProjInMemory < pDlg->GetNProjections()-1)) {
      for(int i=0; i<nProjInMemory; i++) {
        CString sFileTemp;
        sFileTemp.Format(_T("%s%004d.fft"), sTempPath, i);
        FILE* inStream = _tfopen(sFileTemp, _T("rb"));
        size_t nToRead = pDlg->GetImgHeight()*pDlg->GetImgWidth();
        size_t nRead = fread(pImage, sizeof(*pImage), nToRead, inStream);
        fclose(inStream);
        if(nRead != nToRead)
          AfxMessageBox(_T("Error Reading Temp File: ") + sFileTemp);
        glBindTexture(GL_TEXTURE_target, inTexID[i]);
        glTexImage2D(GL_TEXTURE_target, 0, GL_TEXTURE_internalformat, pDlg->GetImgWidth(), pDlg->GetImgHeight(), 0, GL_TEXTURE_texFormat, GL_FLOAT, pImage);
        if(!CheckGLErrors(_T("Projective Texture Loading"))) {
          AfxMessageBox(_T("Gestire_Errore()"));
        }
      }// for(int i=0; i<nProjInMemory; i++)
      lastAttachedTexture = nProjInMemory;
      //sString.Format(_T("Last Attached Texture= %d/%d\r\n"), lastAttachedTexture, pDlg->GetNProjections()-1); 
      //pMsg->AddText(sString);
    }
  }// for(int iPlane=0; iPlane<texSizeZ; iPlane++)
  
  for(int nProj=0; nProj<pDlg->GetNProjections()-1; nProj++) {
    CString sFileTemp;
    sFileTemp.Format(_T("%s%004d.fft"), sTempPath, nProj);
    if(!DeleteFile(sFileTemp)) {
      DWORD Error = GetLastError();
      CString Value = _T("Unable to Delete ");
      Value += sFileTemp;
      AfxMessageBox(Value);
    }
  }

  clock_t end = clock();
  sString.Format(_T("Saved %d Slices With Dimensions:%dx%d\nBackprojecting Time [sec]: %f"), 
                 texSizeZ, texSizeX, texSizeY, (end-start)/1000.0);
  AfxMessageBox(sString);

  glDisable(GL_TEXTURE_target);
  if(!CheckGLErrors(_T("glDisable"))) return;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDeleteTextures(3, outTexID);
  glDeleteFramebuffers(1, &fbo);
  glDeleteTextures(nProjTex, inTexID);

  cgDestroyProgram(fragmentProgram);
  cgDestroyContext(cgContext);
  glDeleteLists(theList, 1);

  glutDestroyWindow(glutWindowHandle);
  if(transformMats != NULL) delete[] transformMats;
  if(inTexID != NULL) delete[] inTexID; 
  return;
}


//GLenum GL_TEXTURE_target             = GL_TEXTURE_RECTANGLE_ARB;;
//GLenum GL_TEXTURE_internalformat     = GL_LUMINANCE32F_ARB;
//GLenum GL_TEXTURE_texFormat		       = GL_LUMINANCE;
//GLenum GL_FILTER_mode                = GL_LINEAR;

bool
initFBOTextures(int nTextures, GLuint* TexID, int texSizeX, int texSizeY, GLenum* attachmentpoints) {
  for (int i=0; i< nTextures; i++) {
    glBindTexture(GL_TEXTURE_target, TexID[i]);
    glTexParameteri(GL_TEXTURE_target, GL_TEXTURE_WRAP_S,     GL_CLAMP);
    glTexParameteri(GL_TEXTURE_target, GL_TEXTURE_WRAP_T,     GL_CLAMP);
    glTexParameteri(GL_TEXTURE_target, GL_TEXTURE_MIN_FILTER, GL_FILTER_mode);
    glTexParameteri(GL_TEXTURE_target, GL_TEXTURE_MAG_FILTER, GL_FILTER_mode);
    glTexImage2D(GL_TEXTURE_target, 0, GL_TEXTURE_internalformat, texSizeX, texSizeY, 0, GL_TEXTURE_texFormat, GL_FLOAT, 0);
    if(!CheckGLErrors(_T("glGenTextures"))) return false;
    // Attach the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentpoints[i], GL_TEXTURE_target, TexID[i], 0);
    if(!CheckFramebufferStatus()) return false;
  } // for (int i=0; i< nTextures; i++)
  return true;
}


bool
initProjTextures(int nProjections, GLuint* TexID) {
  for(int i=0; i<nProjections; i++) {
    glBindTexture(GL_TEXTURE_target, TexID[i]);
    glTexParameteri(GL_TEXTURE_target, GL_TEXTURE_WRAP_S,     GL_CLAMP);
    glTexParameteri(GL_TEXTURE_target, GL_TEXTURE_WRAP_T,     GL_CLAMP);
    glTexParameteri(GL_TEXTURE_target, GL_TEXTURE_MIN_FILTER, GL_FILTER_mode);
    glTexParameteri(GL_TEXTURE_target, GL_TEXTURE_MAG_FILTER, GL_FILTER_mode);
    if(!CheckGLErrors(_T("glTexParameteri"))) return false;
  } // for(int i=0; i<nProjections; i++)

  // Texture coordinates in Eye Space
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  if(!CheckGLErrors(_T("glTexGeni"))) return false;

  return true;
}


// Sets up the Cg runtime and creates shader.
bool
initCG() {
  cgSetErrorCallback(cgErrorCallback);
  cgContext = cgCreateContext();
  #ifdef _DEBUG
    cgGLSetDebugMode(CG_TRUE);
  #else
    cgGLSetDebugMode(CG_FALSE);
  #endif

  _TCHAR  szAppPath[MAX_PATH] = _T("");
  CString strAppDirectory;

  ::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

  // Extract directory
  strAppDirectory = szAppPath;
  strAppDirectory = strAppDirectory.Left(strAppDirectory.ReverseFind('\\'));
  strAppDirectory += _T("\\projTex.cg");
  char fragmentProgramFileName[MAX_PATH];
  size_t pConvertedChars;
  wcstombs_s(&pConvertedChars, fragmentProgramFileName, MAX_PATH-1, strAppDirectory, strAppDirectory.GetLength());

  char* fragmentProgramName = {"projTex"};
  fragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
  cgGLSetOptimalOptions(fragmentProfile);

  // create fragment programs
  fragmentProgram = 
    cgCreateProgramFromFile(cgContext,                // Cg runtime context
                            CG_SOURCE,                // Program in human-readable form
                            fragmentProgramFileName,  // Program Filename (in the same path is better...)
                            fragmentProfile,          // Profile: latest fragment profile
                            fragmentProgramName,      // Entry function name
                            NULL                      // No extra compiler options
                           );
  if(!fragmentProgram) {
    AfxMessageBox(CString(fragmentProgramName) + _T(".cg\nFragment Program not found in Directory\n")+strAppDirectory, MB_ICONSTOP);
    return false;
  }
  cgGLLoadProgram(fragmentProgram);// load program
  // and get parameter handles by name
  NewTexture = cgGetNamedParameter(fragmentProgram, "newTexture");
  OldTexture = cgGetNamedParameter(fragmentProgram, "oldTexture");
  return true;
}


void 
cgErrorCallback() {
  CGerror lastError = cgGetError();
  if(lastError) {
    CString sError1, sError2, sError;
    char* cString = (char *)cgGetErrorString(lastError);
    size_t requiredSize = mbstowcs(NULL, cString, 0) + 1; 
    _TCHAR* tString = new _TCHAR[requiredSize];
    size_t count = mbstowcs(tString, cString, strlen(cString));
    if(count<requiredSize) tString[count] = 0;
    sError1.Format(_T("%s"), tString);
    delete[] tString;
    cString = (char *)cgGetLastListing(cgContext);
    if(cString != NULL) {
      requiredSize = mbstowcs(NULL, cString, 0) + 1;
      tString = new _TCHAR[requiredSize];
      count = mbstowcs(tString, cString, strlen(cString));
      if(count<requiredSize) tString[count] = 0;
      sError.Format(_T("%s%s%s"),sError1, _T("\n"), tString);
      delete[] tString;
    } else {
      sError.Format(_T("%s\nNULL"),sError1);
    }
  	AfxMessageBox(sError);
  	return;
  } // if(lastError)
} 


bool
CheckForCgError(CString Situation) {
  CGerror error;
  CString sError;
  const char* cString;
  _TCHAR* tString;
  size_t requiredSize, count;

  cString = cgGetLastErrorString(&error);
  requiredSize = mbstowcs(NULL, cString, 0) + 1; 
  tString = new _TCHAR[requiredSize];
  count = mbstowcs(tString, cString, strlen(cString));
  if(count<requiredSize) tString[count] = 0;
  sError.Format(_T("%s"), tString);
  delete[] tString;

  if(error != CG_NO_ERROR) {
    CString sMsg;
    if (error == CG_COMPILER_ERROR) {
      cString = cgGetLastListing(cgContext);
      requiredSize = mbstowcs(NULL, cString, 0) + 1; 
      tString = new _TCHAR[requiredSize];
      count = mbstowcs(tString, cString, strlen(cString));
      if(count<requiredSize) tString[count] = 0;
      sMsg.Format(_T("%s: %s\n%s"), Situation, sError, tString);
      delete[] tString;
    } else {
      sMsg.Format(_T("%s: %s"), Situation, sError);
    }
    AfxMessageBox(sMsg, MB_ICONSTOP);
    return false;
  } // if(error != CG_NO_ERROR)
  return true;
}


// Creates framebuffer object, binds it to reroute rendering operations 
// from the traditional framebuffer to the offscreen buffer
bool
initFBO(GLuint* pFbo) {
  glGenFramebuffersEXT(1, pFbo);// Create a FBO
  glBindFramebufferEXT(GL_FRAMEBUFFER, *pFbo);// bind offscreen framebuffer
  if(!CheckGLErrors(_T("initFBO"))) {
    return false;
  }
  return true;
}

