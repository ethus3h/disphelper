/* This file is part of the source code for the DispHelper COM helper library.
 * DispHelper allows you to call COM objects with an extremely simple printf style syntax.
 * DispHelper can be used from C++ or even plain C. It works with most Windows compilers
 * including Dev-CPP, Visual C++ and LCC-WIN32. Including DispHelper in your project
 * couldn't be simpler as it is available in a compacted single file version.
 *
 * Included with DispHelper are over 20 samples that demonstrate using COM objects
 * including ADO, CDO, Outlook, Eudora, Excel, Word, Internet Explorer, MSHTML,
 * PocketSoap, Word Perfect, MS Agent, SAPI, MSXML, WIA, dexplorer and WMI.
 *
 * DispHelper is free open source software provided under the BSD license.
 *
 * Find out more and download DispHelper at:
 * http://sourceforge.net/projects/disphelper/
 * http://disphelper.sourceforge.net/
 */


#ifndef DISPHELPER_H_INCLUDED
#define DISPHELPER_H_INCLUDED

#include <objbase.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

HRESULT dhCreateObject(LPCOLESTR szProgId, LPCWSTR szMachine, IDispatch ** ppDisp);
HRESULT dhGetObject(LPCOLESTR szFile, LPCOLESTR szProgId, IDispatch ** ppDisp);

HRESULT dhCreateObjectEx(LPCOLESTR szProgId, REFIID riid, DWORD dwClsContext, COSERVERINFO * pServerInfo, void ** ppv);
HRESULT dhGetObjectEx(LPCOLESTR szFile, LPCOLESTR szProgId, REFIID riid, DWORD dwClsContext, LPVOID lpvReserved, void ** ppv);

HRESULT dhCallMethod(IDispatch * pDisp, LPCOLESTR szMember, ...);
HRESULT dhPutValue(IDispatch * pDisp, LPCOLESTR szMember, ...);
HRESULT dhPutRef(IDispatch * pDisp, LPCOLESTR szMember, ...);
HRESULT dhGetValue(LPCWSTR szIdentifier, void * pResult, IDispatch * pDisp, LPCOLESTR szMember, ...);

HRESULT dhInvoke(int invokeType, VARTYPE returnType, VARIANT * pvResult, IDispatch * pDisp, LPCOLESTR szMember, ...);
HRESULT dhInvokeArray(int invokeType, VARIANT * pvResult, UINT cArgs, IDispatch * pDisp, LPCOLESTR szMember, VARIANT * pArgs);

HRESULT dhCallMethodV(IDispatch * pDisp, LPCOLESTR szMember, va_list * marker);
HRESULT dhPutValueV(IDispatch * pDisp, LPCOLESTR szMember, va_list * marker);
HRESULT dhPutRefV(IDispatch * pDisp, LPCOLESTR szMember, va_list * marker);
HRESULT dhGetValueV(LPCWSTR szIdentifier, void * pResult, IDispatch * pDisp, LPCOLESTR szMember, va_list * marker);
HRESULT dhInvokeV(int invokeType, VARTYPE returnType, VARIANT * pvResult, IDispatch * pDisp, LPCOLESTR szMember, va_list * marker);

HRESULT dhAutoWrap(int invokeType, VARIANT * pvResult, IDispatch * pDisp, LPCOLESTR szMember, UINT cArgs, ...);
HRESULT dhParseProperties(IDispatch * pDisp, LPCWSTR szProperties, UINT * lpcPropsSet);

HRESULT dhEnumBegin(IEnumVARIANT ** ppEnum, IDispatch * pDisp, LPCOLESTR szMember, ...);
HRESULT dhEnumBeginV(IEnumVARIANT ** ppEnum, IDispatch * pDisp, LPCOLESTR szMember, va_list * marker);
HRESULT dhEnumNextObject(IEnumVARIANT * pEnum, IDispatch ** ppDisp);
HRESULT dhEnumNextVariant(IEnumVARIANT * pEnum, VARIANT * pvResult);

HRESULT dhInitializeImp(BOOL bInitializeCOM, BOOL bUnicode);
void dhUninitialize(BOOL bUninitializeCOM);

#define dhInitializeA(bInitializeCOM) dhInitializeImp(bInitializeCOM, FALSE)
#define dhInitializeW(bInitializeCOM) dhInitializeImp(bInitializeCOM, TRUE)

#ifdef UNICODE
#define dhInitialize dhInitializeW
#else
#define dhInitialize dhInitializeA
#endif

#define AutoWrap dhAutoWrap
#define DISPATCH_OBJ(objName) IDispatch * objName = NULL
#define dhFreeString(string) SysFreeString((BSTR) string)

#ifndef SAFE_RELEASE
#ifdef __cplusplus
#define SAFE_RELEASE(pObj) { if (pObj) { (pObj)->Release(); (pObj) = NULL; } }
#else
#define SAFE_RELEASE(pObj) { if (pObj) { (pObj)->lpVtbl->Release(pObj); (pObj) = NULL; } }
#endif
#endif

#define SAFE_FREE_STRING(string) { dhFreeString(string); (string) = NULL; }




/* ===================================================================== */
#ifndef DISPHELPER_NO_WITH

#define WITH0(objName, pDisp, szMember) { \
	DISPATCH_OBJ(objName);            \
	if (SUCCEEDED(dhGetValue(L"%o", &objName, pDisp, szMember))) {

#define WITH1(objName, pDisp, szMember, arg1) { \
	DISPATCH_OBJ(objName);                  \
	if (SUCCEEDED(dhGetValue(L"%o", &objName, pDisp, szMember, arg1))) {

#define WITH2(objName, pDisp, szMember, arg1, arg2) { \
	DISPATCH_OBJ(objName);                        \
	if (SUCCEEDED(dhGetValue(L"%o", &objName, pDisp, szMember, arg1, arg2))) {

#define WITH3(objName, pDisp, szMember, arg1, arg2, arg3) { \
	DISPATCH_OBJ(objName);                              \
	if (SUCCEEDED(dhGetValue(L"%o", &objName, pDisp, szMember, arg1, arg2, arg3))) {

#define WITH4(objName, pDisp, szMember, arg1, arg2, arg3, arg4) { \
	DISPATCH_OBJ(objName);                                    \
	if (SUCCEEDED(dhGetValue(L"%o", &objName, pDisp, szMember, arg1, arg2, arg3, arg4))) {

#define WITH WITH0

#define ON_WITH_ERROR(objName) } else {

#define END_WITH(objName) } SAFE_RELEASE(objName); }

#endif /* ----- DISPHELPER_NO_WITH ----- */




/* ===================================================================== */
#ifndef DISPHELPER_NO_FOR_EACH

#define FOR_EACH0(objName, pDisp, szMember) { \
	IEnumVARIANT * xx_pEnum_xx = NULL;    \
	DISPATCH_OBJ(objName);                \
	if (SUCCEEDED(dhEnumBegin(&xx_pEnum_xx, pDisp, szMember))) { \
		while (dhEnumNextObject(xx_pEnum_xx, &objName) == NOERROR) {

#define FOR_EACH1(objName, pDisp, szMember, arg1) { \
	IEnumVARIANT * xx_pEnum_xx = NULL;          \
	DISPATCH_OBJ(objName);                      \
	if (SUCCEEDED(dhEnumBegin(&xx_pEnum_xx, pDisp, szMember, arg1))) { \
		while (dhEnumNextObject(xx_pEnum_xx, &objName) == NOERROR) {

#define FOR_EACH2(objName, pDisp, szMember, arg1, arg2) { \
	IEnumVARIANT * xx_pEnum_xx = NULL;          \
	DISPATCH_OBJ(objName);                      \
	if (SUCCEEDED(dhEnumBegin(&xx_pEnum_xx, pDisp, szMember, arg1, arg2))) { \
		while (dhEnumNextObject(xx_pEnum_xx, &objName) == NOERROR) {


#define FOR_EACH3(objName, pDisp, szMember, arg1, arg2, arg3) { \
	IEnumVARIANT * xx_pEnum_xx = NULL;          \
	DISPATCH_OBJ(objName);                      \
	if (SUCCEEDED(dhEnumBegin(&xx_pEnum_xx, pDisp, szMember, arg1, arg2, arg3))) { \
		while (dhEnumNextObject(xx_pEnum_xx, &objName) == NOERROR) {


#define FOR_EACH4(objName, pDisp, szMember, arg1, arg2, arg3, arg4) { \
	IEnumVARIANT * xx_pEnum_xx = NULL;          \
	DISPATCH_OBJ(objName);                      \
	if (SUCCEEDED(dhEnumBegin(&xx_pEnum_xx, pDisp, szMember, arg1, arg2, arg3, arg4))) { \
		while (dhEnumNextObject(xx_pEnum_xx, &objName) == NOERROR) {

#define FOR_EACH FOR_EACH0

#define ON_FOR_EACH_ERROR(objName) SAFE_RELEASE(objName); }} else {{

#define NEXT(objName) SAFE_RELEASE(objName); }} SAFE_RELEASE(objName); SAFE_RELEASE(xx_pEnum_xx); }

#endif /* ----- DISPHELPER_NO_FOR_EACH ----- */




/* ===================================================================== */
#ifndef DISPHELPER_NO_EXCEPTIONS

/* Structure to store a DispHelper exception */
typedef struct tagDH_EXCEPTION
{
	LPCWSTR szInitialFunction;
	LPCWSTR szErrorFunction;

	HRESULT hr;

	WCHAR szMember[64];
	WCHAR szCompleteMember[256];

	UINT swCode;
	LPWSTR szDescription;
	LPWSTR szSource;
	LPWSTR szHelpFile;
	DWORD dwHelpContext;

	UINT iArgError;

	BOOL bDispatchError;

#ifdef DISPHELPER_INTERNAL_BUILD
	BOOL bOld;
#endif
} DH_EXCEPTION, * PDH_EXCEPTION;

typedef void (*DH_EXCEPTION_CALLBACK) (PDH_EXCEPTION);

/* Structure to store exception options. */
typedef struct tagDH_EXCEPTION_OPTIONS
{
	HWND hwnd;
	LPCWSTR szAppName;
	BOOL bShowExceptions;
	BOOL bDisableRecordExceptions;
	DH_EXCEPTION_CALLBACK pfnExceptionCallback;
} DH_EXCEPTION_OPTIONS, * PDH_EXCEPTION_OPTIONS;

/* Functions to manipulate global exception options */
HRESULT dhToggleExceptions(BOOL bShow);
HRESULT dhSetExceptionOptions(PDH_EXCEPTION_OPTIONS pExceptionOptions);
HRESULT dhGetExceptionOptions(PDH_EXCEPTION_OPTIONS pExceptionOptions);

/* Functions to show an exception, format an exception into a string
 * and get a copy of the last exception */
HRESULT dhShowException(PDH_EXCEPTION pException);
HRESULT dhFormatException(PDH_EXCEPTION pException, LPWSTR szBuffer, UINT cchBufferSize, BOOL bFixedFont);
HRESULT dhGetLastException(PDH_EXCEPTION * pException);

#ifdef DISPHELPER_INTERNAL_BUILD

void dhEnter(void);
HRESULT dhExitEx(HRESULT hr, BOOL bDispatchError, LPCWSTR szMember, LPCWSTR szCompleteMember, EXCEPINFO * pExcepInfo, UINT iArgError, LPCWSTR szFunctionName);
void dhCleanupThreadException(void);

#define DH_ENTER(szFunctionName) static LPCWSTR xx_szFunctionName_xx = szFunctionName; \
				    dhEnter()

#define DH_EXITEX(hr, bDispatchError, szMember, szCompleteMember, pExcepInfo, iArgError) \
		dhExitEx(hr, bDispatchError, szMember, szCompleteMember, pExcepInfo, iArgError, xx_szFunctionName_xx)

#define DH_EXIT(hr, szCompleteMember) DH_EXITEX(hr, FALSE, NULL, szCompleteMember, NULL, 0)

#endif /* ----- DISPHELPER_INTERNAL_BUILD ----- */

#else  /* ----- DISPHELPER_NO_EXCEPTIONS ----- */

/* These macros define out calls to selected exception functions */
#define dhToggleExceptions(bShow) (E_NOTIMPL)
#define dhSetExceptionOptions(pExcepOptions) (E_NOTIMPL)

#ifdef DISPHELPER_INTERNAL_BUILD
#define DH_ENTER(szFunctionName)
#define DH_EXITEX(hr, bDispatchError, szMember, szCompleteMember, pExcepInfo, iArgError) \
	(((hr == DISP_E_EXCEPTION && pExcepInfo) ?                      \
	(SysFreeString(((EXCEPINFO *)(pExcepInfo))->bstrSource),        \
	 SysFreeString(((EXCEPINFO *)(pExcepInfo))->bstrDescription),   \
	 SysFreeString(((EXCEPINFO *)(pExcepInfo))->bstrHelpFile), 0) : (0)), hr)
#define DH_EXIT(hr, szCompleteMember)(hr)
#endif

#endif /* ----- DISPHELPER_NO_EXCEPTIONS ----- */






/* ===================================================================== */
#ifdef DISPHELPER_INTERNAL_BUILD

#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>

/* Macro to include or lose debug code. */
#ifdef DEBUG
#define DBG_CODE(code) code
#else
#define DBG_CODE(code)
#endif

/* Are we in unicode mode? */
extern BOOL dh_g_bIsUnicodeMode;

/* Number of objects in an array */
#undef ARRAYSIZE
#define ARRAYSIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Maximum number of arguments for a member */
#define DH_MAX_ARGS 25

/* Maximum length of a member string */
#define DH_MAX_MEMBER 512

/* This macro is missing from Dev-Cpp/Mingw */
#ifndef V_UI4
#define V_UI4(X) V_UNION(X, ulVal)
#endif

/* Macro to notify programmer of invalid identifier in debug mode. */
#define DEBUG_NOTIFY_INVALID_IDENTIFIER(chIdentifier) \
DBG_CODE( { \
		char buf[256]; \
		sprintf(buf,"DEBUG: The format string or return identifier contained the invalid identifier '%c'.\n" \
		"The valid identifiers are \"d/u/e/b/v/B/S/s/T/o/O/t/W/D/f/m\".\n" \
		"Each %% character should be followed by a valid identifier.\n" \
		"Identifiers are case sensitive.", (chIdentifier)); \
		MessageBoxA(NULL, buf, "DEBUG: Invalid Format Identifier", MB_ICONSTOP); \
	} )

#ifdef _MSC_VER
#pragma warning(disable : 4706) /* Assignment in conditional expression */
#endif

#ifndef DISPHELPER_NO_PRAGMA_LIB
#ifdef __LCC__
#pragma lib <ole32.lib>
#pragma lib <oleaut32.lib>
#pragma lib <uuid.lib>
#endif
#endif


#endif /* ----- DISPHELPER_INTERNAL_BUILD ----- */

#ifndef DISPHELPER_NO_PRAGMA_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__)
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "uuid.lib")
#endif
#endif



#ifdef __cplusplus
}
#endif

#endif /* ----- DISPHELPER_H_INCLUDED ----- */