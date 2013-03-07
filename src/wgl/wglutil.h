/*
 * Author: kenc
 *
 * Created: March 1, 2013
 * Copyright	(c) 2013, Attachmate Corporation All Rights Reserved
 */
#ifndef wglExt_h
#define wglExt_h

/*
 * Derrived from... at http://developer.download.nvidia.com/opengl/includes/wglext.h
 */

#define WGL_WGLEXT_PROTOTYPES	1

/* WGL_extensions_string extension */
typedef const char * (WINAPI *PFNWGLGETEXTENSIONSSTRINGPROC)(HDC);

/* WGL_pixel_format extension */
typedef BOOL (WINAPI *PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC, const int *, const FLOAT *,  UINT, int *, UINT *);

/*
 * Accepted in the <piAttributes> parameter array of
 * wglGetPixelFormatAttribivARB, and wglGetPixelFormatAttribfvARB, and
 * as a type in the <piAttribIList> and <pfAttribFList> parameter
 * arrays of wglChoosePixelFormatARB:
 */

#ifndef WGL_ARB_make_current_read
#define ERROR_INVALID_PIXEL_TYPE_ARB	0x2043
#define ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB 0x2054
#endif

#define WGL_NUMBER_PIXEL_FORMATS	0x2000
#define WGL_DRAW_TO_WINDOW			0x2001
#define WGL_DRAW_TO_BITMAP			0x2002
#define WGL_ACCELERATION			0x2003
#define WGL_NEED_PALETTE			0x2004
#define WGL_NEED_SYSTEM_PALETTE		0x2005
#define WGL_SWAP_LAYER_BUFFERS		0x2006
#define WGL_SWAP_METHOD				0x2007
#define WGL_NUMBER_OVERLAYS			0x2008
#define WGL_NUMBER_UNDERLAYS		0x2009
#define WGL_TRANSPARENT				0x200A
#define WGL_TRANSPARENT_RED_VALUE	0x2037
#define WGL_TRANSPARENT_GREEN_VALUE	0x2038
#define WGL_TRANSPARENT_BLUE_VALUE	0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE	0x203A
#define WGL_TRANSPARENT_INDEX_VALUE	0x203B
#define WGL_SHARE_DEPTH				0x200C
#define WGL_SHARE_STENCIL			0x200D
#define WGL_SHARE_ACCUM				0x200E
#define WGL_SUPPORT_GDI				0x200F
#define WGL_SUPPORT_OPENGL			0x2010
#define WGL_DOUBLE_BUFFER			0x2011
#define WGL_STEREO					0x2012
#define WGL_PIXEL_TYPE				0x2013
#define WGL_COLOR_BITS				0x2014
#define WGL_RED_BITS				0x2015
#define WGL_RED_SHIFT				0x2016
#define WGL_GREEN_BITS				0x2017
#define WGL_GREEN_SHIFT				0x2018
#define WGL_BLUE_BITS				0x2019
#define WGL_BLUE_SHIFT				0x201A
#define WGL_ALPHA_BITS				0x201B
#define WGL_ALPHA_SHIFT				0x201C
#define WGL_ACCUM_BITS				0x201D
#define WGL_ACCUM_RED_BITS			0x201E
#define WGL_ACCUM_GREEN_BITS		0x201F
#define WGL_ACCUM_BLUE_BITS			0x2020
#define WGL_ACCUM_ALPHA_BITS		0x2021
#define WGL_DEPTH_BITS				0x2022
#define WGL_STENCIL_BITS			0x2023
#define WGL_AUX_BUFFERS				0x2024
#define WGL_SAMPLE_BUFFERS			0x2041
#define WGL_SAMPLES					0x2042
#define PFD_DRAW_TO_PBUFFER			0x4000

/*
 * Accepted as a value in the <piAttribIList> and <pfAttribFList>
 * parameter arrays of wglChoosePixelFormatARB, and returned in the
 * <piValues> parameter array of wglGetPixelFormatAttribivARB, and the
 * <pfValues> parameter array of wglGetPixelFormatAttribfvARB:
 */
#define WGL_NO_ACCELERATION			0x2025
#define WGL_GENERIC_ACCELERATION	0x2026
#define WGL_FULL_ACCELERATION		0x2027

#define WGL_SWAP_EXCHANGE			0x2028
#define WGL_SWAP_COPY				0x2029
#define WGL_SWAP_UNDEFINED			0x202A

#define WGL_TYPE_RGBA				0x202B
#define WGL_TYPE_COLORINDEX			0x202C
#define PFD_TYPE_FLOAT_RGBA			2

#ifndef WGL_EXT_make_current_read
#define ERROR_INVALID_PIXEL_TYPE_EXT	0x2043
#endif

#ifndef WGL_ARB_make_current_read
#define WGL_ARB_make_current_read 1
#ifdef WGL_WGLEXT_PROTOTYPES
extern BOOL WINAPI wglMakeContextCurrentARB(HDC, HDC, HGLRC);
extern HDC WINAPI wglGetCurrentReadDCARB(void);
#endif /* WGL_WGLEXT_PROTOTYPES */
typedef BOOL (WINAPI *PFNWGLMAKECONTEXTCURRENTARBPROC)(HDC hDrawDC, HDC hReadDC, HGLRC hglrc);
typedef HDC (WINAPI *PFNWGLGETCURRENTREADDCARBPROC)(void);
#endif

/*
 * Inits some function pointers and return number of formats.
 * Returns 0 on failure.
 */
extern HWND getWindow(HINSTANCE hInst, int x, int y, int w, int h, int format);
extern BOOL wglExtMakeContextCurrent(HWND did, HWND rid, HGLRC gid);
extern BOOL wglExtSwapBuffers(HWND);
extern int wglExtChoosePixelFormat(const int *);
extern BOOL wglExtInit(HINSTANCE, WNDPROC);
extern void wglExtDispose(HINSTANCE);
LRESULT storeDC(HWND hWnd);
HDC fetchDC(HWND hWnd);
#endif
