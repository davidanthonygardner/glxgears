/*
 * Mesa 3-D graphics library
 * Version: 6.5.2
 *
 * Copyright (C) 1999-2006 Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


/*
 * This program opens two windows, renders into one and uses
 * glCopyPixels to copy the image from the first window into the
 * second by means of the extension function wglMakeContextCurrentARB().
 */
/*
 * Author: kenc
 *
 * Created: March 1, 2013
 * Copyright	(c) 2013, Attachmate Corporation All Rights Reserved
 */
#include <windows.h>
#include <GL/gl.h>
#include <assert.h>
#include "wglutil.h"

static HGLRC Context = NULL;
static HWND Win[2] = {NULL, NULL};
static int Width[2] = {0, 0}, Height[2] = {0, 0};
static float Angle = 0.0f;


static BOOL
Redraw(BOOL DrawFront)
{
	assert(Context != NULL);
	if (!wglExtMakeContextCurrent(Win[0], Win[0], Context))
		return FALSE;

	Angle += 1.0f;
	if (DrawFront) {
		glDrawBuffer(GL_FRONT);
		glReadBuffer(GL_FRONT);
	}
	else {
		glDrawBuffer(GL_BACK);
		glReadBuffer(GL_BACK);
	}

	glViewport(0, 0, Width[0], Height[0]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glShadeModel(GL_FLAT);
	glClearColor(0.5, 0.5, 0.5, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	/* draw blue quad */
	glColor3f(0.3f, 0.3f, 1.0f);
	glPushMatrix();
	glRotatef(Angle, 0, 0, 1);
	glBegin(GL_POLYGON);
	glVertex2f(-0.5, -0.25);
	glVertex2f(0.5, -0.25);
	glVertex2f(0.5, 0.25);
	glVertex2f(-0.5, 0.25);
	glEnd();
	glPopMatrix();

	if (DrawFront)
		glFinish();
	else
		wglExtSwapBuffers(Win[0]);

	/* copy image from window 0 to window 1 */
	if (!wglExtMakeContextCurrent(Win[1], Win[0], Context))
		return FALSE;

	/* copy the image between windows */
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glCopyPixels(0, 0, Width[0], Height[0], GL_COLOR);
	if (DrawFront)
		glFinish();
	else
		wglExtSwapBuffers(Win[1]);
	return TRUE;
}


static void
Resize(HWND win, int width, int height)
{
	int i;
	HDC hDC;

	if (win == Win[0])
		i = 0;
	else
		i = 1;

	if ((hDC = (HDC)(INT_PTR)GetWindowLongPtr(win, GWLP_USERDATA)) != NULL) {
		BOOL ret;

		if ((ret = wglMakeCurrent(hDC, Context)) != FALSE) {
			Width[i] = width;
			Height[i] = height;
		}
		assert(ret != FALSE);
	}
}

static LRESULT CALLBACK
EventLoop(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	RECT rect;
	LRESULT ret;
	LPWINDOWPOS pWinPos;
	static BOOL drawFront = FALSE;

	switch (msg) {
	case WM_CREATE:
		ret = (LRESULT)storeDC(hWnd);
		assert(ret == 0);
		break;

	case WM_PAINT:
		GetUpdateRect(hWnd, &rect, FALSE);
		Redraw(drawFront);
		ret = 0;
		break;

	case WM_WINDOWPOSCHANGED:
		pWinPos = (LPWINDOWPOS)lParam;
		//if (!(pWinPos->flags&SWP_NOSIZE))
			Resize(hWnd, pWinPos->cx, pWinPos->cy);
		ret = 0;
		break;

	case WM_CHAR:
		if (wParam == 'f') {	// 'f' key
			drawFront = !drawFront;
			Redraw(drawFront);
		}
		else if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		ret = 0;
		break;

	case WM_DESTROY:
		if ((hDC = fetchDC(hWnd)) != NULL) {
			ret = ReleaseDC(hWnd, hDC);
			assert(ret != FALSE);
		}
		ret = 1;
		break;

	default:
		ret = DefWindowProc(hWnd, msg, wParam, lParam);
		break;
	}
	return ret;
}


static BOOL
Init(HINSTANCE hInst, int show)
{
	BOOL rval = FALSE;

	if (wglExtInit(hInst, EventLoop) != FALSE) {
		const int attribList[] = {
				WGL_DRAW_TO_WINDOW, GL_TRUE,
				WGL_SUPPORT_OPENGL, GL_TRUE,
				WGL_DOUBLE_BUFFER, GL_TRUE,
				WGL_PIXEL_TYPE, WGL_TYPE_RGBA,
				WGL_COLOR_BITS, 32,
				WGL_DEPTH_BITS, 24,
				0};
		int format;

		if ((format = wglExtChoosePixelFormat(attribList)) != 0) {
			if ((Win[0] = getWindow(hInst, 0, 0, 300, 300, format)) != NULL) {
				if ((Win[1] = getWindow(hInst, 350, 0, 300, 300, format)) != NULL) {
					ShowWindow(Win[0], show);
					ShowWindow(Win[1], show);
					rval = TRUE;
				}
			}
		}
	}
	return rval;
}


int CALLBACK
WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	int ret = -1;

	if (Init(hInst, nCmdShow) != FALSE) {
		MSG msg;
		HDC hDC;

		if ((hDC = fetchDC(Win[0])) != NULL) {
			if ((Context = wglCreateContext(hDC)) != NULL) {
				BOOL rval;

				while ((ret = GetMessage(&msg, NULL, 0, 0)) != 0) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				if (Win[0] != NULL) {
					rval = DestroyWindow(Win[0]);
					assert(rval != FALSE);
				}
				if (Win[1] != NULL) {
					rval = DestroyWindow(Win[1]);
					assert(rval != FALSE);
				}
				rval = wglDeleteContext(Context);
				assert(rval != FALSE);
			}
		}
		wglExtDispose(hInst);
		ret = (int)msg.wParam;
    }
	if (Context != NULL)
		wglDeleteContext(Context);
	return ret;
}
