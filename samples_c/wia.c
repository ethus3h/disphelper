/* This file contains sample code that demonstrates use of the DispHelper COM helper library.
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


/* --
wia.c:
  Demonstrates using Windows Image Acquisition(WIA) to manipulate
images. Demonstrates taking a snapshot from a video device.

  WIA is only available on Windows XP SP1 and later. These samples
use the WIA Automation layer. Currently this must be downloaded and
installed. It is available from
http://www.microsoft.com/downloads/details.aspx?FamilyID=a332a77a-01b8-4de6-91c2-b7ea32537e29
or do a search for 'WIA Automation' at the Microsoft Download Center.
 -- */


#include "disphelper.h"
#include <stdio.h>
#include <wchar.h>
#include <tchar.h>

#define HR_TRY(func) if (FAILED(func)) { printf("\n## Fatal error on line %d.\n", __LINE__); goto cleanup; }

/* Selected WIA constants */
LPCWSTR wiaFormatBMP =  L"{B96B3CAB-0728-11D3-9D7B-0000F81EF32E}";
LPCWSTR wiaFormatPNG =  L"{B96B3CAF-0728-11D3-9D7B-0000F81EF32E}";
LPCWSTR wiaFormatGIF =  L"{B96B3CB0-0728-11D3-9D7B-0000F81EF32E}";
LPCWSTR wiaFormatJPEG = L"{B96B3CAE-0728-11D3-9D7B-0000F81EF32E}";
LPCWSTR wiaFormatTIFF = L"{B96B3CB1-0728-11D3-9D7B-0000F81EF32E}";
LPCWSTR wiaCommandTakePicture = L"{AF933CAC-ACAD-11D2-A093-00C04F72DC3C}";
const int VideoDeviceType = 3;


/* **************************************************************************
 * ConvertImage:
 *   Demonstrates using WIA to convert an image from one format to another.
 * See http://msdn.microsoft.com/library/en-us/wiaaut/wia/wiax/overviews/HowToUseFilters.asp
 * under 'Convert Filter: Create a Compressed JPEG File from Another File'.
 *
 ============================================================================ */
void ConvertImage(LPCTSTR szInput, LPCTSTR szOutput, LPCWSTR szFormat, int iQuality)
{
	DISPATCH_OBJ(ImgIn);    /* as ImageFile */
	DISPATCH_OBJ(ImgOut);   /* as ImageFile */
	DISPATCH_OBJ(IP);       /* as ImageProcess */
	LPWSTR szFilterId = NULL;

	HR_TRY( dhCreateObject(L"WIA.ImageFile", NULL, &ImgIn) );
	HR_TRY( dhCreateObject(L"WIA.ImageProcess", NULL, &IP) );

	/* Load input image */
	HR_TRY( dhCallMethod(ImgIn, L".LoadFile(%T)", szInput) );

	/* Get the Convert filter id and create the filter */
	HR_TRY( dhGetValue(L"%S", &szFilterId, IP, L".FilterInfos(%S).FilterID", L"Convert") );
	HR_TRY( dhCallMethod(IP, L".Filters.Add(%S)", szFilterId) );

	/* Set the filter properties */
	HR_TRY( dhPutValue(IP, L".Filters(%d).Properties(%S).Value = %S", 1, L"FormatID", szFormat) );
	HR_TRY( dhPutValue(IP, L".Filters(%d).Properties(%S).Value = %d", 1, L"Quality", iQuality) ); /* Only used for JPEG */

	/* Apply the filter to the input image and get the returned image in ImgOut */
	HR_TRY( dhGetValue(L"%o", &ImgOut, IP, L".Apply(%o)", ImgIn) );

	/* Save output image */
	HR_TRY( dhCallMethod(ImgOut, L".SaveFile(%T)", szOutput) );

cleanup:
	dhFreeString(szFilterId);
	SAFE_RELEASE(ImgIn);
	SAFE_RELEASE(ImgOut);
	SAFE_RELEASE(IP);
}


/* **************************************************************************
 * Thumbnail:
 *   Demonstrates using a filter chain to resize an image as well as convert
 * it to a specified format. This can be used to create thumbnails.
 *
 ============================================================================ */
void Thumbnail(LPCTSTR szInput, LPCTSTR szOutput, LPCWSTR szFormat, int iQuality, int width, int height)
{
	DISPATCH_OBJ(ImgIn);    /* as ImageFile */
	DISPATCH_OBJ(ImgOut);   /* as ImageFile */
	DISPATCH_OBJ(IP);       /* as ImageProcess */
	LPWSTR szFilterScaleId   = NULL;
	LPWSTR szFilterConvertId = NULL;

	HR_TRY( dhCreateObject(L"WIA.ImageFile", NULL, &ImgIn) );
	HR_TRY( dhCreateObject(L"WIA.ImageProcess", NULL, &IP) );

	HR_TRY( dhCallMethod(ImgIn, L".LoadFile(%T)", szInput) );

	/* Add and set up the scale filter */
	HR_TRY( dhGetValue(L"%S", &szFilterScaleId, IP, L".FilterInfos(%S).FilterID", L"Scale") );
	HR_TRY( dhCallMethod(IP, L".Filters.Add(%S)", szFilterScaleId) );
	HR_TRY( dhPutValue(IP, L".Filters(%d).Properties(%S).Value = %d", 1, L"MaximumWidth", width) );
	HR_TRY( dhPutValue(IP, L".Filters(%d).Properties(%S).Value = %d", 1, L"MaximumHeight", height) );

	/* Add and set up the convert filter */
	HR_TRY( dhGetValue(L"%S", &szFilterConvertId, IP, L".FilterInfos(%S).FilterID", L"Convert") );
	HR_TRY( dhCallMethod(IP, L".Filters.Add(%S)", szFilterConvertId) );
	HR_TRY( dhPutValue(IP, L".Filters(%d).Properties(%S).Value = %S", 2, L"FormatID", szFormat) );
	HR_TRY( dhPutValue(IP, L".Filters(%d).Properties(%S).Value = %d", 2, L"Quality", iQuality) );

	HR_TRY( dhGetValue(L"%o", &ImgOut, IP, L".Apply(%o)", ImgIn) );

	HR_TRY( dhCallMethod(ImgOut, L".SaveFile(%T)", szOutput) );

cleanup:
	dhFreeString(szFilterScaleId);
	dhFreeString(szFilterConvertId);
	SAFE_RELEASE(ImgIn);
	SAFE_RELEASE(ImgOut);
	SAFE_RELEASE(IP);
}


/* **************************************************************************
 * ImageInfo:
 *   Use WIA to retrieve image information.
 *
 ============================================================================ */
void ImageInfo(LPCTSTR szInput)
{
	DISPATCH_OBJ(Img);    /* as ImageFile */
	struct ImageInfo {
		int iWidth, iHeight, iDepth, iHorRes, iVerRes, iFrameCount;
	} ImgInfo = { 0 };

	HR_TRY( dhCreateObject(L"WIA.ImageFile", NULL, &Img) );
	HR_TRY( dhCallMethod(Img, L".LoadFile(%T)", szInput) );

	dhGetValue(L"%d", &ImgInfo.iWidth,      Img, L".Width");
	dhGetValue(L"%d", &ImgInfo.iHeight,     Img, L".Height");
	dhGetValue(L"%d", &ImgInfo.iDepth,      Img, L".PixelDepth");
	dhGetValue(L"%d", &ImgInfo.iHorRes,     Img, L".HorizontalResolution");
	dhGetValue(L"%d", &ImgInfo.iVerRes,     Img, L".VerticalResolution");
	dhGetValue(L"%d", &ImgInfo.iFrameCount, Img, L".FrameCount");

	_tprintf(TEXT("Image information for %s\n"), szInput);
	wprintf(L"Width: %d\nHeight: %d\nBit Depth: %d\nHorizontal Resolution: %d\nVertical Resolution: %d\nFrame Count: %d\n\n",
	        ImgInfo.iWidth, ImgInfo.iHeight, ImgInfo.iDepth, ImgInfo.iHorRes,
	        ImgInfo.iVerRes, ImgInfo.iFrameCount);

cleanup:
	SAFE_RELEASE(Img);
}


/* **************************************************************************
 * SaveVideoFrame:
 *   Attempts to find a video device, take a picture and save it to file.
 * This sample is untested and may not work.
 * See http://msdn.microsoft.com/library/en-us/wiaaut/wia/wiax/overviews/sharedsamples.asp
 * under 'Implement a Web Camera ASP Page'.
 *
 ============================================================================ */
void SaveVideoFrame(LPCTSTR szOutput)
{
	DISPATCH_OBJ(oDeviceManager);
	BOOL bDeviceFound    = FALSE;

	HR_TRY( dhCreateObject(L"WIA.DeviceManager", NULL, &oDeviceManager) );

	FOR_EACH(oDeviceInfo, oDeviceManager, L".DeviceInfos")
	{
		int iDeviceType;

		if (SUCCEEDED(dhGetValue(L"%d", &iDeviceType, oDeviceInfo, L".Type")) &&
		                   iDeviceType == VideoDeviceType)
		{
			dhCallMethod(oDeviceInfo, L".Connect.ExecuteCommand(%S).Transfer.SaveFile(%T)",
                                         wiaCommandTakePicture, szOutput);

			bDeviceFound = TRUE;
			break;
		}

	} NEXT(oDeviceInfo);

	if (!bDeviceFound) printf("There is no Video Device\n");

cleanup:
	SAFE_RELEASE(oDeviceManager);
}


/* ============================================================================ */
int main(void)
{
	dhInitialize(TRUE);
	dhToggleExceptions(TRUE);

	printf("Converting images...\n\n");

	ConvertImage(TEXT("C:\\WINDOWS\\Web\\Wallpaper\\Bliss.bmp"), TEXT("bliss.jpg"),
	             wiaFormatJPEG, 50);

	ConvertImage(TEXT("C:\\WINDOWS\\Web\\Wallpaper\\Bliss.bmp"), TEXT("bliss.tif"),
	             wiaFormatTIFF, 100);

	printf("Creating Thumbnails...\n\n");

	Thumbnail(TEXT("C:\\WINDOWS\\Web\\Wallpaper\\Bliss.bmp"), TEXT("blissThumb.png"),
	             wiaFormatPNG, 100, 200, 200);

	Thumbnail(TEXT("C:\\WINDOWS\\Web\\Wallpaper\\Bliss.bmp"), TEXT("blissThumb.jpg"),
	             wiaFormatJPEG, 40, 200, 200);

	printf("Retrieving image information...\n\n");
	ImageInfo(TEXT("C:\\WINDOWS\\Web\\Wallpaper\\Bliss.bmp"));

	printf("Attempting to save video frame...\n\n");
	SaveVideoFrame(TEXT("frame.jpg"));

	printf("\nPress ENTER to exit...\n");
	getchar();

	dhUninitialize(TRUE);
	return 0;
}




