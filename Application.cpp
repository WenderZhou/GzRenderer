// Application.cpp: implementation of the Application class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CS580HW.h"
#include "Application.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Application::Application()
{
	for (int k = 0; k < AAKERNEL_SIZE + 1; k++)
		m_pRender[k] = NULL;		// the renderer
	
	m_pUserInput = NULL;
	m_pFrameBuffer = NULL;
}

Application::~Application()
{
	//if(m_pFrameBuffer != NULL)
	//	delete m_pFrameBuffer;
	if(m_pUserInput != NULL)
		delete m_pUserInput;
}

