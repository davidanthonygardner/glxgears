/*
 * Author: kenc
 *
 * Created: March 1, 2013
 * Copyright	(c) 2013, Attachmate Corporation All Rights Reserved
 */
#include <windows.h>
#include <assert.h>
#include <GL/gl.h>
#include "wglutil.h"

#define STYLE			WS_OVERLAPPEDWINDOW
#define EXSTYLE			WS_EX_OVERLAPPEDWINDOW
#define GL_CLASS		"GL"

static PFNWGLMAKECONTEXTCURRENTARBPROC WGLMakeContextCurrent = NULL;
static PFNWGLCHOOSEPIXELFORMATARBPROC WGLChoosePixelFormat = NULL;

LRESULT
storeDC(HWND hWnd)
{
	HDC hDC;

	if ((hDC = GetDC(hWnd)) != NULL) {
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)hDC);

		assert(GetWindowLongPtr(hWnd, GWLP_USERDATA) == (LONG_PTR)hDC);
		return 0;
	}
	return -1;
}

HDC
fetchDC(HWND hWnd)
{
	assert(hWnd != NULL);
	if (hWnd != NULL) {
		HDC hDC = (HDC)(INT_PTR)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		assert(hDC != NULL);
		return hDC;
	}
	return NULL;
}

HWND
getWindow(HINSTANCE hInst, int x, int y, int w, int h, int format)
{
	HWND hParent;

	if ((hParent = GetDesktopWindow()) != NULL) {
		HWND hWnd = CreateWindowEx(EXSTYLE, GL_CLASS, "wincopy", STYLE, x, y, w, h, hParent, NULL, hInst, NULL);
		HDC hDC;
		BOOL ret;

		assert(hWnd != NULL);
		if ((hDC = (HDC)(INT_PTR)GetWindowLongPtr(hWnd, GWLP_USERDATA)) != NULL) {
			PIXELFORMATDESCRIPTOR pfd;

			if ((ret = SetPixelFormat(hDC, format, &pfd)) != FALSE)
				return hWnd;
			ret = DestroyWindow(hWnd);
			assert(ret != FALSE);
		}
	}
	assert(hParent != NULL);
	return NULL;
}

static int
getFormat(HDC hDC, PIXELFORMATDESCRIPTOR *pfd)
{
	int format;

	int size = sizeof(PIXELFORMATDESCRIPTOR);
	memset(pfd, 0, size);
	pfd->nSize = size;
	pfd->nVersion = 1;
	pfd->dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL;
	pfd->iLayerType = PFD_MAIN_PLANE;
	pfd->iPixelType = PFD_TYPE_RGBA;
	format = ChoosePixelFormat(hDC, pfd);
	assert(format > 0);
	return format;
}

static HGLRC
createCurrentContext(HDC hDC)
{
	HGLRC hGLRC = NULL;

	if ((hGLRC = wglCreateContext(hDC)) != NULL) {
		BOOL current;

		if ((current = wglMakeCurrent(hDC, hGLRC)) == FALSE) {
			BOOL ret = wglDeleteContext(hGLRC);

			assert(ret != FALSE);
			hGLRC = NULL;
		}
	}
	else
		assert(hGLRC != NULL);
	return hGLRC;
}

static BOOL
checkWGLExtensions(HINSTANCE hInst)
{
	HWND hWnd;
	BOOL rval, ret = FALSE;

	if ((hWnd = CreateWindowEx(EXSTYLE, GL_CLASS, "tmp", STYLE, 0, 0, 1, 1,
		GetDesktopWindow(), NULL, hInst, NULL)) != NULL) {
		HDC hDC;

		if ((hDC = fetchDC(hWnd)) != NULL) {
			PIXELFORMATDESCRIPTOR pfd;
			int format;

			if ((format = getFormat(hDC, &pfd)) > 0) {
				BOOL setFormat;

				if ((setFormat = SetPixelFormat(hDC, format, &pfd)) != FALSE) {
					HGLRC hGLRC = createCurrentContext(hDC);

					if (hGLRC != NULL) {
						PFNWGLGETEXTENSIONSSTRINGPROC WGLGetExtensionsStringFunc;

						WGLGetExtensionsStringFunc = (PFNWGLGETEXTENSIONSSTRINGPROC)wglGetProcAddress("wglGetExtensionsStringARB");
						assert(WGLGetExtensionsStringFunc != NULL);
						if (WGLGetExtensionsStringFunc != NULL) {
							const char *wglExtensionString = (*WGLGetExtensionsStringFunc)(hDC);

							if (wglExtensionString != NULL) {
								WGLMakeContextCurrent = (PFNWGLMAKECONTEXTCURRENTARBPROC)wglGetProcAddress("wglMakeContextCurrentARB");
								assert(WGLMakeContextCurrent != NULL);
								WGLChoosePixelFormat = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
								assert(WGLChoosePixelFormat != NULL);
								if (WGLMakeContextCurrent != NULL && WGLChoosePixelFormat != NULL)
									ret = TRUE;
							}
						}
						rval = wglDeleteContext(hGLRC);
						assert(rval != FALSE);
					}
				}
			}
		}
		rval = DestroyWindow(hWnd);
		assert(rval != FALSE);
	}
	return ret;
}


BOOL
wglExtInit(HINSTANCE hInst, WNDPROC proc)
{
	WNDCLASS glClass;
	ATOM atom;

	memset(&glClass, 0, sizeof(WNDCLASS));
	glClass.style = CS_PARENTDC;
	glClass.lpfnWndProc = proc;
	glClass.hInstance = hInst;
	glClass.lpszClassName = GL_CLASS;
	glClass.cbWndExtra = sizeof(HANDLE);
	if ((atom = RegisterClass(&glClass)) == 0) {
		assert(atom != 0);
		return FALSE;
	}
	return checkWGLExtensions(hInst);
}

void
wglExtDispose(HINSTANCE hInst)
{
	BOOL ret;

	ret = UnregisterClass(GL_CLASS, hInst);
	assert(ret != FALSE);
}

BOOL
wglExtMakeContextCurrent(HWND did, HWND rid, HGLRC gid)
{
	HDC hDrawDC;
	BOOL ret = FALSE;

	if ((hDrawDC = (HDC)(INT_PTR)GetWindowLongPtr(did, GWLP_USERDATA)) != NULL) {
		if (did != rid) {
			HDC hReadDC;

			if ((hReadDC = (HDC)(INT_PTR)GetWindowLongPtr(rid, GWLP_USERDATA)) != NULL) {
				ret = (*WGLMakeContextCurrent)(hDrawDC, hReadDC, gid);
				assert(ret != FALSE);
			}
		}
		else
			ret = wglMakeCurrent(hDrawDC, gid);
	}
	assert(ret != FALSE);
	return ret;
}

BOOL
wglExtSwapBuffers(HWND hWnd)
{
	HDC hDC;

	if ((hDC = (HDC)(INT_PTR)GetWindowLongPtr(hWnd, GWLP_USERDATA)) != NULL);
		return wglSwapLayerBuffers(hDC, WGL_SWAP_MAIN_PLANE);
	return FALSE;
}

int
wglExtChoosePixelFormat(const int *attrs)
{
	HDC hDC;
	HWND hWnd = GetDesktopWindow();

	if ((hDC = (HDC)GetDC(hWnd)) != NULL) {
		int format = 0;
		UINT nFormats;
		BOOL ret;

		WGLChoosePixelFormat(hDC, attrs, NULL, 1, &format, &nFormats);
		assert(nFormats > 0 && format != 0);
		ret = ReleaseDC(hWnd, hDC);
		assert(ret != FALSE);
		return format;
	}
	return 0;
}
