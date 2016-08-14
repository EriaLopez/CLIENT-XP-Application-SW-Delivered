#include "stdafx.h"
#include "OpenGLControl.h"
#include <math.h>
#include <stdio.h>

#include "ScannerClass.h"
#include "GlobalsClass.h"
extern GlobalsClass g_theGlobals;
extern GlobalsClass g_myGlobals;

ScannerClass *SptrOpenGL = NULL; 
void ConvertXYZ(SRIPacket *SRIdataPacket);
int buffToUseLocal_old = -1;

COpenGLControl::COpenGLControl(void)
{
	m_fPosX = 0.0f;		// X position of model in camera view
	m_fPosY = 0.0f;		// Y position of model in camera view
	m_fZoom = 15.0f;		// Zoom on model in camera view // edit this for initial Zoom
	m_fRotX = 0.0f;		// Rotation on model in camera view
	m_fRotY	= 0.0f;		// Rotation on model in camera view
	m_bIsMaximized = false;

	// ADDED 07222010
	SptrOpenGL = (ScannerClass *)g_theGlobals.ScannerPtr[0]; // HACK 07222010 - Using only scanner0 here
}

COpenGLControl::~COpenGLControl(void)
{
}

BEGIN_MESSAGE_MAP(COpenGLControl, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void COpenGLControl::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	ValidateRect(NULL);
}

void COpenGLControl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (0 >= cx || 0 >= cy || nType == SIZE_MINIMIZED) return;

	// Map the OpenGL coordinates.
	glViewport(0, 0, cx, cy);

	// Projection view
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	// Set our current view perspective
	gluPerspective(35.0f, (float)cx / (float)cy, 0.01f, 2000.0f);

	// Model view
	glMatrixMode(GL_MODELVIEW);

	switch (nType)
	{
		// If window resize token is "maximize"
		case SIZE_MAXIMIZED:
		{
			// Get the current window rect
			GetWindowRect(m_rect);

			// Move the window accordingly
			MoveWindow(6, 6, cx - 14, cy - 14);

			// Get the new window rect
			GetWindowRect(m_rect);

			// Store our old window as the new rect
			m_oldWindow = m_rect;

			break;
		}

		// If window resize token is "restore"
		case SIZE_RESTORED:
		{
			// If the window is currently maximized
			if (m_bIsMaximized)
			{
				// Get the current window rect
				GetWindowRect(m_rect);

				// Move the window accordingly (to our stored old window)
				MoveWindow(m_oldWindow.left, m_oldWindow.top - 18, m_originalRect.Width() - 4, m_originalRect.Height() - 4);

				// Get the new window rect
				GetWindowRect(m_rect);

				// Store our old window as the new rect
				m_oldWindow = m_rect;
			}
		
			break;
		}
	}
}

int COpenGLControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1) return -1;

	oglInitialize();

	return 0;
}

void COpenGLControl::OnDraw(CDC *pDC)
{
	// If the current view is perspective...
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -m_fZoom);
	glTranslatef(m_fPosX, m_fPosY, 0.0f);
	glRotatef(m_fRotX, 1.0f, 0.0f, 0.0f);
	glRotatef(m_fRotY, 0.0f, 1.0f, 0.0f);
}


//for real time scan updating

void COpenGLControl::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
		case 1:
		{
			// Clear color and depth buffer bits
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Draw OpenGL scene
			//oglDrawScene();
            myOglDrawScene();
			
			SwapBuffers(hdc);

			break;
		}

		default:
			break;
	}

	CWnd::OnTimer(nIDEvent);
}

void COpenGLControl::OnMouseMove(UINT nFlags, CPoint point)
{
	int diffX = (int)(point.x - m_fLastX);
	int diffY = (int)(point.y - m_fLastY);
	m_fLastX  = (float)point.x;
	m_fLastY  = (float)point.y;

	// Left mouse button
	if (nFlags & MK_LBUTTON)
	{
		m_fRotX += (float)0.5f * diffY;

		if ((m_fRotX > 360.0f) || (m_fRotX < -360.0f))
		{
			m_fRotX = 0.0f;
		}

		m_fRotY += (float)0.5f * diffX;

		if ((m_fRotY > 360.0f) || (m_fRotY < -360.0f))
		{
			m_fRotY = 0.0f;
		}
	}

	// Right mouse button
	else if (nFlags & MK_RBUTTON)
	{
		m_fZoom -= (float)0.1f * diffY;
	}

	// Middle mouse button
	else if (nFlags & MK_MBUTTON)
	{
		m_fPosX += (float)0.05f * diffX;
		m_fPosY -= (float)0.05f * diffY;
	}

	OnDraw(NULL);

	CWnd::OnMouseMove(nFlags, point);
}

void COpenGLControl::oglCreate(CRect rect, CWnd *parent)
{
	CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_OWNDC, NULL, (HBRUSH)GetStockObject(BLACK_BRUSH), NULL);

	CreateEx(0, className, "OpenGL", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, rect, parent, 0);

	// Set initial variables' values
	m_oldWindow	   = rect;
	m_originalRect = rect;

	hWnd = parent;
}

void COpenGLControl::oglInitialize(void)
{
	// Initial Setup:
	//
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32, // bit depth
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		16, // z-buffer depth
		0, 0, 0, 0, 0, 0, 0,
	};

	// Get device context only once.
	hdc = GetDC()->m_hDC;
	
	// Pixel format.
	m_nPixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, m_nPixelFormat, &pfd);

	// Create the OpenGL Rendering Context.
	hrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hrc);

	// Basic Setup:
	//
	// Set color to use when clearing the background.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	// Turn on backface culling
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	
	// Turn on depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Send draw request
	OnDraw(NULL);
}

typedef struct {
    double r,g,b;
} COLOUR;

COLOUR GetColour(double v,double vmin,double vmax)
{
   COLOUR c = {1.0,1.0,1.0}; // white
   double dv;

   if (v < vmin)
      v = vmin;
   if (v > vmax)
      v = vmax;
   dv = vmax - vmin;

   if (v < (vmin + 0.25 * dv)) {
      c.r = 0;
      c.g = 4 * (v - vmin) / dv;
   } else if (v < (vmin + 0.5 * dv)) {
      c.r = 0;
      c.b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
   } else if (v < (vmin + 0.75 * dv)) {
      c.r = 4 * (v - vmin - 0.5 * dv) / dv;
      c.b = 0;
   } else {
      c.g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
      c.b = 0;
   }

   return(c);
}

void COpenGLControl::oglDrawScene(void)
{
	//return;
	// Wireframe Mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glEnable(GL_CULL_FACE);  // hide faces that are behind

	glPointSize(1.0f); // set point size to _x5 pixels

#if 1 // 1 = plot actual points. 0 = plot a cube for testing OpenGL
	//printf("Npoints %d\n", g_theGlobals.Npoints);

	double minmax, fac, dist;
	glBegin(GL_POINTS);
	if(g_theGlobals.Npoints > 1)
	{
		minmax = 1.0/(g_theGlobals.maxDist - g_theGlobals.minDist);
		fac = 5.0/g_theGlobals.maxDist;
		for (unsigned i = 0; i < g_theGlobals.Npoints; i++)
		{
			dist = g_theGlobals.R[i]; //3D dist from scanner
			COLOUR c = GetColour(dist, g_theGlobals.minDist, g_theGlobals.maxDist);

			glColor3f(c.r,c.g,c.b); // set color //glColor3b(r,g,b);
//			glVertex3f(1.0, 1.0, 1.0);
			
			glVertex3f(fac*g_theGlobals.X[i], fac*g_theGlobals.Y[i], fac*g_theGlobals.Z[i]); // set vertex
		}
	}
	glEnd();

	return;
#else
   // for testing

	//glBegin(GL_QUADS);
	//glBegin(GL_POLYGON);
	glBegin(GL_POINTS);
		// Top Side
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f( 1.0f, 1.0f,  1.0f);
		glVertex3f( 1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f,  1.0f);

		// Bottom Side
		//glColor3f(1.0f,0.0f,0.0f);
				glColor3f(1.0f,1.0f,1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f( 1.0f, -1.0f, -1.0f);
		glVertex3f( 1.0f, -1.0f,  1.0f);
		glVertex3f(-1.0f, -1.0f,  1.0f);

		glEnd();
#endif
}

void COpenGLControl::myOglDrawScene(void)
{

    printf("eclopezv myOglDrawScene() \n");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_TRIANGLES);

    glVertex3f(-1.0f, -0.5f, -4.0f);    // lower left vertex
    glVertex3f(1.0f, -0.5f, -4.0f);    // lower right vertex
    glVertex3f(0.0f, 0.5f, -4.0f);    // upper vertex

    glEnd();

    //return;
    // Wireframe Mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glEnable(GL_CULL_FACE);  // hide faces that are behind

    glPointSize(1.0f); // set point size to _x5 pixels

#if 1 // 1 = plot actual points. 0 = plot a cube for testing OpenGL
                       //printf("Npoints %d\n", g_theGlobals.Npoints);

    double minmax, fac, dist;
    glBegin(GL_POINTS);
    if (g_myGlobals.Npoints > 1)
    {
        minmax = 1.0 / (g_myGlobals.maxDist - g_myGlobals.minDist);
        fac = 5.0 / g_myGlobals.maxDist;
        for (unsigned i = 0; i < g_myGlobals.Npoints; i++)
        {
            dist = g_myGlobals.R[i]; //3D dist from scanner
            COLOUR c = GetColour(dist, g_myGlobals.minDist, g_myGlobals.maxDist);

            glColor3f(c.r, c.g, c.b); // set color //glColor3b(r,g,b);
                                      //			glVertex3f(1.0, 1.0, 1.0);

            glVertex3f(fac*g_myGlobals.X[i], fac*g_myGlobals.Y[i], fac*g_myGlobals.Z[i]); // set vertex
        }
    }
    glEnd();

    return;
#else
                       // for testing

                       //glBegin(GL_QUADS);
                       //glBegin(GL_POLYGON);
    glBegin(GL_POINTS);
    // Top Side
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);

    // Bottom Side
    //glColor3f(1.0f,0.0f,0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);

    glEnd();
#endif
}
