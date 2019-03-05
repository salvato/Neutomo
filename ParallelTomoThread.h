#pragma once

bool initCG();
bool initFBO(GLuint* pFbo);
bool initFBOTextures(int nTextures, GLuint* TexId, int texSizeX, int texSizeY, GLenum* attachmentpoints);
bool initProjTextures(int nProjections, GLuint* TexID);

void DoParallelTomo(void* pParent);
