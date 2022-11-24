// CS580HWView.cpp : implementation of the CCS580HWView class
//

#include "stdafx.h"
#include "CS580HW.h"

#include "CS580HWDoc.h"
#include "CS580HWView.h"
#include "RotateDlg.h"
#include "TranslateDlg.h"
#include "ScaleDlg.h"

#include "rend.h"
#include "Application5.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCS580HWView

IMPLEMENT_DYNCREATE(CCS580HWView, CView)

BEGIN_MESSAGE_MAP(CCS580HWView, CView)
	//{{AFX_MSG_MAP(CCS580HWView)
	ON_COMMAND(IDM_RENDER, OnRender)
	ON_COMMAND(IDM_ROTATE, OnRotate)
	ON_COMMAND(IDM_TRANSLATE, OnTranslate)
	ON_COMMAND(IDM_SCALE, OnScale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCS580HWView construction/destruction

CCS580HWView::CCS580HWView()
{
	// TODO: add construction code here
	m_pApplication = NULL;
}

CCS580HWView::~CCS580HWView()
{
	if(m_pApplication != NULL)
	{
		delete m_pApplication;
	}
}

BOOL CCS580HWView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CCS580HWView drawing

void CCS580HWView::OnDraw(CDC* pDC)
{
	CCS580HWDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	if(m_pApplication != NULL)
		DrawFrameBuffer(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CCS580HWView diagnostics

#ifdef _DEBUG
void CCS580HWView::AssertValid() const
{
	CView::AssertValid();
}

void CCS580HWView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCS580HWDoc* CCS580HWView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCS580HWDoc)));
	return (CCS580HWDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCS580HWView message handlers

void CCS580HWView::OnRender() 
{
	// TODO: Add your command handler code here

	// Call renderer 

	// Application 5
	if(m_pApplication != NULL)
		((Application5 *)m_pApplication)->Render();
	else 
		AfxMessageBox("Application was not allocated\n");

	// Set window size
	CRect clientRect, windowRect;
	int x_offset, y_offset;

	GetClientRect(&clientRect);
	AfxGetMainWnd()->GetWindowRect(&windowRect);
	
	x_offset = windowRect.Width() - clientRect.Width();
	y_offset = windowRect.Height() - clientRect.Height();

	AfxGetMainWnd()->SetWindowPos(NULL, 0, 0, x_offset+m_pApplication->m_nWidth, y_offset+m_pApplication->m_nHeight, NULL/*,SWP_SHOWWINDOW*/);

	Invalidate(true);	
}

void CCS580HWView::DrawFrameBuffer(CDC *pDC)
{
	if(m_pApplication->m_pFrameBuffer == NULL)
    {
        return;
    }

    HDC hdc;
    hdc = ::CreateCompatibleDC(pDC->m_hDC);
	HBITMAP m_bitmap;

    // Display the current image
    char buffer[sizeof(BITMAPINFO)];
    BITMAPINFO* binfo = (BITMAPINFO*)buffer;
    memset(binfo, 0, sizeof(BITMAPINFOHEADER));
    binfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    
	// Create the bitmap
    BITMAPINFOHEADER* bih = &binfo->bmiHeader;
	bih->biBitCount = 3*8;
	bih->biWidth = m_pApplication->m_nWidth;
	bih->biHeight = m_pApplication->m_nHeight;
    bih->biPlanes = 1;
    bih->biCompression = BI_RGB;
    bih->biSizeImage = 0;
    
    m_bitmap = CreateDIBSection(hdc, binfo, 0, 0, 0, DIB_RGB_COLORS);

    int colors = DIB_RGB_COLORS;
    
    ::SelectObject(hdc, m_bitmap);
	binfo->bmiHeader.biBitCount = 0;
	GetDIBits(hdc, m_bitmap, 0, 0, 0, binfo, colors);
    binfo->bmiHeader.biBitCount = 24;
    binfo->bmiHeader.biHeight = -abs(binfo->bmiHeader.biHeight);
    SetDIBits(hdc, m_bitmap, 0, m_pApplication->m_nHeight, m_pApplication->m_pFrameBuffer, binfo, colors);

    ::SetStretchBltMode(pDC->m_hDC, COLORONCOLOR);
    CRect client;
    GetClientRect(&client);
    ::BitBlt(pDC->m_hDC, 0, 0, m_pApplication->m_nWidth, m_pApplication->m_nHeight, 
        hdc, 0, 0, SRCCOPY);
    ::DeleteDC(hdc);
    DeleteObject(m_bitmap); 
}

void CCS580HWView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class

	// Assign Application 5
	if(m_pApplication == NULL)
	{
		m_pApplication = new Application5;
	}
	
	// Initialize and begin renderer
	((Application5 *)m_pApplication)->Initialize();
}

// Callback function for rotation  
void CCS580HWView::OnRotate() 
{
	// TODO: Add your command handler code here
	CRotateDlg dlg;
	GzInput* input;
	GzMatrix rotMat;

	if(m_pApplication == NULL) return;

	input = m_pApplication->m_pUserInput;
	if(input == NULL) return;

	// Initialize
	input->rotation = { 0.0f,0.0f,0.0f };
	dlg.Initialize(input->rotation.x, input->rotation.y, input->rotation.z);

	if(dlg.DoModal() == IDOK)
	{
		//  Create Rotation Matrix 
		switch(dlg.m_nAxis)
		{
		case 0 :
			input->rotation.x = dlg.m_fRot;
			rotMat = GzRotXMat(input->rotation.x);
			break;
		case 1:
			input->rotation.y = dlg.m_fRot;
			rotMat = GzRotYMat(input->rotation.y);
			break;
		case 2:
			input->rotation.z = dlg.m_fRot;
			rotMat = GzRotZMat(input->rotation.z);
			break;
		}

		// Accumulate matrix
		for (int k = 0; k < AAKERNEL_SIZE; k++)
			m_pApplication->m_pRender[k]->GzPushMatrix(rotMat); 
	}
}

// Callback function for Translation
void CCS580HWView::OnTranslate() 
{
	// TODO: Add your command handler code here
	CTranslateDlg dlg;
	GzInput* input;
	GzMatrix trxMat;


	if(m_pApplication == NULL) return;

	input = m_pApplication->m_pUserInput;
	if(input == NULL) return;

	// Initialize
	input->translation = { 0.0f,0.0f,0.0f };
	dlg.Initialize(input->translation.x, input->translation.y, input->translation.z);

	if(dlg.DoModal() == IDOK)
	{
		// Update input translation value
		input->translation.x = dlg.m_fTx; input->translation.y = dlg.m_fTy; input->translation.z = dlg.m_fTz;

		//  Create Translation Matrix
		trxMat = GzTrxMat(input->translation);

		// Accumulate matrix
		for (int k = 0; k < AAKERNEL_SIZE; k++)
			m_pApplication->m_pRender[k]->GzPushMatrix(trxMat); 
	}
}

// Callback function for Scaling
void CCS580HWView::OnScale() 
{
	// TODO: Add your command handler code here
	CScaleDlg dlg;
	GzInput* input;
	GzMatrix scaleMat;

	if(m_pApplication == NULL) return;

	input = m_pApplication->m_pUserInput;
	if(input == NULL) return;

	// Initialize
	input->scale = { 1.0f,1.0f,1.0f };
	dlg.Initialize(input->scale.x, input->scale.y, input->scale.z);

	if(dlg.DoModal() == IDOK)
	{
		// Update input scale value
		input->scale.x = dlg.m_fSx; input->scale.y = dlg.m_fSy; input->scale.z = dlg.m_fSz;

		//  Create Scaling Matrix
		scaleMat = GzScaleMat(input->scale);

		// Accumulate matrix
		for (int k = 0; k < AAKERNEL_SIZE; k++)
			m_pApplication->m_pRender[k]->GzPushMatrix(scaleMat); 
	}
}
