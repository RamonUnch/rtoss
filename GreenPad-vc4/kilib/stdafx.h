#ifndef _KILIB_STDAFX_H_
#define _KILIB_STDAFX_H_

#undef   WINVER
#define  WINVER      0x0400
#undef  _WIN32_IE
#define _WIN32_IE    0x0200
#undef  _WIN32_WINNT
#define _WIN32_WINNT 0x0400

#define  OEMRESOURCE
#define  NOMINMAX
#ifdef SUPERTINY
  #define memset memset_default
#endif

#include <windows.h>
#include <shlobj.h>
#include <commdlg.h>
#include <commctrl.h>
#include <imm.h>
// dimm.hが無くてエラーになる場合、プロジェクトの設定でUSEGLOBALIMEの定義を
// 削除するか、最新の Platform SDK を導入すればビルドが通るようになります。
#ifdef USEGLOBALIME
#include <dimm.h>
#endif

#ifndef NO_MLANG
#include <mlang.h>
#endif

#ifdef SUPERTINY
  #undef memset
#endif

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif
#ifndef OFN_ENABLESIZING
#define OFN_ENABLESIZING 0x00800000
#endif
#ifndef IMR_RECONVERTSTRING
#define IMR_RECONVERTSTRING             0x0004
typedef struct tagRECONVERTSTRING {
    DWORD dwSize;
    DWORD dwVersion;
    DWORD dwStrLen;
    DWORD dwStrOffset;
    DWORD dwCompStrLen;
    DWORD dwCompStrOffset;
    DWORD dwTargetStrLen;
    DWORD dwTargetStrOffset;
} RECONVERTSTRING, *PRECONVERTSTRING, NEAR *NPRECONVERTSTRING, FAR *LPRECONVERTSTRING;
#endif
#ifndef IMR_CONFIRMRECONVERTSTRING
#define IMR_CONFIRMRECONVERTSTRING      0x0005
#endif
#ifndef WM_IME_REQUEST
#define WM_IME_REQUEST                  0x0288
#endif
#ifndef SPI_GETWHEELSCROLLLINES
#define SPI_GETWHEELSCROLLLINES         104
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA                     120
#endif
#ifndef SCS_CAP_SETRECONVERTSTRING
#define SCS_CAP_SETRECONVERTSTRING      0x00000004
#endif

#ifdef _MSC_VER
#pragma warning( disable: 4355 )
#endif

#if defined(_MSC_VER) && _MSC_VER < 1100
#define typename class
#define mutable 
#define explicit 

#include "yvals.h"
#endif


#if defined(__DMC__) || (defined(_MSC_VER) && _MSC_VER < 1300)
	#define SetWindowLongPtr SetWindowLong
	#define GetWindowLongPtr GetWindowLong
	#define UINT_PTR         UINT
	#define LONG_PTR         LONG
	#define GWLP_WNDPROC     GWL_WNDPROC
	#define GWLP_USERDATA    GWL_USERDATA
#endif

#endif // _KILIB_STDAFX_H_
