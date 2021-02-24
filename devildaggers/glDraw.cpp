#include "stdafx.h"
#include "glDraw.h"
//https://guidedhacking.com/

#   define  GLUT_STROKE_ROMAN               ((void *)0x0000)
#   define  GLUT_STROKE_MONO_ROMAN          ((void *)0x0001)
#   define  GLUT_BITMAP_9_BY_15             ((void *)0x0002)
#   define  GLUT_BITMAP_8_BY_13             ((void *)0x0003)
#   define  GLUT_BITMAP_TIMES_ROMAN_10      ((void *)0x0004)
#   define  GLUT_BITMAP_TIMES_ROMAN_24      ((void *)0x0005)
#   define  GLUT_BITMAP_HELVETICA_10        ((void *)0x0006)
#   define  GLUT_BITMAP_HELVETICA_12        ((void *)0x0007)
#   define  GLUT_BITMAP_HELVETICA_18        ((void *)0x0008)

void GL::Hook(char* function, uintptr_t &oFunction, void * hFunction)
{
	HMODULE hMod = GetModuleHandle(L"opengl32.dll");

	if (hMod)
	{
		oFunction = (uintptr_t)mem::TrampolineHook((void*)GetProcAddress(hMod, function), hFunction, 5);
	}
}

void GL::SetupOrtho()
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, viewport[2], viewport[3]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
}

void GL::RestoreGL()
{
	glEnable(GL_DEPTH_TEST);
}
/*
void GL::Text(HDC hdc, GLfloat x, GLfloat y, GLfloat r, GLfloat g, GLfloat b, void * font, char const*const pFmtText, ...)
{
	HGLRC  hglrc;

	// create a rendering context  
	hglrc = wglCreateContext(hdc);

	// make it the calling thread's current rendering context  
	wglMakeCurrent(hdc, hglrc);

	// now we can call OpenGL API  

	// make the system font the device context's selected font  
	SelectObject(hdc, GetStockObject(SYSTEM_FONT));

	// create the bitmap display lists  
	// we're making images of glyphs 0 thru 254  
	// the display list numbering starts at 1000, an arbitrary choice  
	wglUseFontBitmaps(hdc, 0, 255, 1000);

	// display a string:  
	// indicate start of glyph display lists  
	glListBase(1000);
	// now draw the characters in a string  
	glCallLists(24, GL_UNSIGNED_BYTE, pFmtText);
}*/

void GL::DrawFilledRect(float x, float y, float width, float height, const GLubyte color[3])
{
	glColor3ub(color[0], color[1], color[2]);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glEnd();
}

void GL::DrawOutline(float x, float y, float width, float height, float lineWidth, const GLubyte color[3])
{
	glLineWidth(lineWidth);
	glBegin(GL_LINE_STRIP);
	glColor3ub(color[0], color[1], color[2]);
	glVertex2f(x - 0.5f, y - 0.5f);
	glVertex2f(x + width + 0.5f, y - 0.5f);
	glVertex2f(x + width + 0.5f, y + height + 0.5f);
	glVertex2f(x - 0.5f, y + height + 0.5f);
	glVertex2f(x - 0.5f, y - 0.5f);
	glEnd();
}