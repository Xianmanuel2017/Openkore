/*############################################################################
##			NEONCUBE - RAGNAROK ONLINE PATCH CLIENT
##
##  http://openkore.sourceforge.net/neoncube
##  (c) 2005 Ansell "Cliffe" Cruz (Cliffe@xeronhosting.com)
##  
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation; either version 2 of the License, or
##  (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
##  along with this program; if not, write to the Free Software
##  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
##
##############################################################################*/

#define MAXFILESIZE (0x10000)

#ifdef _DEBUG
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


#include <windows.h>
#include <shellapi.h>
#include <wininet.h>
#include <commctrl.h>
#include <direct.h>
#include <stdio.h>
#include "main.h"
#include "resource.h"



int WINAPI WinMain(HINSTANCE hInstance,
			       HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   INT nCmdShow)
{
//	_CrtDumpMemoryLeaks();
	HINSTANCE cwebdll; 

	HWND hwnd; 
	MSG message; 
	const INT iWidth = 500; 
	const INT iHeight = 500; 
	WNDCLASSEX wc; 
	InitCommonControls();
	

	GetPrivateProfileString("server", "noticeURL", NULL, settings.szNoticeURL, sizeof(settings.szNoticeURL), iniFile); //NOTICE URL
	GetPrivateProfileString("server", "patchURL", NULL, settings.szPatchURL, sizeof(settings.szPatchURL), iniFile); //PATCH URL
	GetPrivateProfileString("server", "patchList", NULL, settings.szPatchList, sizeof(settings.szPatchList), iniFile);
	GetPrivateProfileString("server", "executable", NULL, settings.szExecutable, sizeof(settings.szExecutable), iniFile);
	GetPrivateProfileString("server", "patch_folder", NULL, settings.szPatchFolder, sizeof(settings.szPatchFolder), iniFile);
	GetPrivateProfileString("server", "registrationLink", NULL, settings.szRegistration, sizeof(settings.szRegistration), iniFile);
	GetPrivateProfileString("server", "grf", NULL, settings.szGrf, sizeof(settings.szGrf), iniFile);
	

/*	style.iFontColorRED = LoadINIInt("text","fontcolorR"); //static font color Red
    style.iFontColorGREEN = LoadINIInt("text","fontcolorG"); //static font color Green
	style.iFontColorBLUE = LoadINIInt("text","fontcolorB"); //static font color Blue

	style.iTextBgRED = LoadINIInt("text","bgColorR");
	style.iTextBgGREEN = LoadINIInt("text","bgColorG");
	style.iTextBgBLUE = LoadINIInt("text","bgColorB");
*/

	BUTTONSTYLE bsMinimize;
	bsMinimize.x = LoadINIInt("minimize","xcoord");
	bsMinimize.y = LoadINIInt("minimize","ycoord");
	bsMinimize.width = LoadINIInt("minimize","width");
	bsMinimize.height = LoadINIInt("minimize","height");

	BUTTONSTYLE bsClose;
	bsClose.x = LoadINIInt("close","xcoord");
	bsClose.y = LoadINIInt("close","ycoord");
	bsClose.width = LoadINIInt("close","width");
	bsClose.height = LoadINIInt("close","height");

	BUTTONSTYLE bsStartGame;
	bsStartGame.x = LoadINIInt("startgame","xcoord");
	bsStartGame.y = LoadINIInt("startgame","ycoord");
	bsStartGame.width = LoadINIInt("startgame","width");
	bsStartGame.height = LoadINIInt("startgame","height");

	BUTTONSTYLE bsRegister;
	bsRegister.x = LoadINIInt("register","xcoord");
	bsRegister.y = LoadINIInt("register","ycoord");
	bsRegister.width = LoadINIInt("register","width");
	bsRegister.height = LoadINIInt("register","height");

	BUTTONSTYLE bsCancel;
	bsCancel.x = LoadINIInt("cancel","xcoord");
	bsCancel.y = LoadINIInt("cancel","ycoord");
	bsCancel.width = LoadINIInt("cancel","width");
	bsCancel.height = LoadINIInt("cancel","height");

	COORDS crdProgress;
	crdProgress.x = LoadINIInt("progressbar","xcoord");
	crdProgress.y = LoadINIInt("progressbar","ycoord");
	crdProgress.width = LoadINIInt("progressbar","width");
	crdProgress.height = LoadINIInt("progressbar","height");
	

	if (!(cwebdll = LoadLibrary("nc32.dll")))
		PostError();

	//load bitmap buttons
	if(!LoadButtonBitmap())
		PostError();

	
		lpEmbedBrowserObject = (EmbedBrowserObjectPtr *)GetProcAddress((HINSTANCE)cwebdll, "EmbedBrowserObject");
		lpUnEmbedBrowserObject = (UnEmbedBrowserObjectPtr *)GetProcAddress((HINSTANCE)cwebdll, "UnEmbedBrowserObject");
		lpDisplayHTMLPage = (DisplayHTMLPagePtr *)GetProcAddress((HINSTANCE)cwebdll, "DisplayHTMLPage");
		ZeroMemory(&wc, sizeof(WNDCLASSEX));
        
	
		wc.cbSize		 = sizeof(WNDCLASSEX);
		wc.style		 = CS_OWNDC;
		wc.lpfnWndProc	 = WndProc;
		wc.cbClsExtra	 = 0;
		wc.cbWndExtra	 = 0;
		wc.hInstance	 = hInstance;
		wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = "NeonCube";
		wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);


		// register class
		if (!RegisterClassEx(&wc)) 
			PostError();
		if(!SetupNoticeClass(hInstance))
			PostError();


		// get actual screen resolution
		INT iSw = (WORD)GetSystemMetrics(SM_CXSCREEN);      
		INT iSh = (WORD)GetSystemMetrics(SM_CYSCREEN);      
		

		// center rectangle
		RECT rc = {
			(iSw - iWidth)/2, 
			(iSh - iHeight)/2, 
	 		iWidth, 
			iHeight 
		};


		
		hbmBackground = (HBITMAP)LoadImage(NULL,
									  "neoncube\\skin\\bg.bmp",
									  IMAGE_BITMAP,
									  0,
									  0,
									  LR_LOADFROMFILE);
		if(!hbmBackground)
			PostError();


		hwnd = CreateWindowEx(0,
							 "NeonCube", 
							"NeonCube Patch Client",
							 WS_POPUP,
							 rc.left,
							 rc.top, 
							 iWidth,
							 iHeight,
							 NULL, 
							 NULL, 
							 hInstance, 
							 NULL
							 );
		
		ShowWindow(hwnd, nCmdShow);


		hwndMinimize = CreateWindow(
								  "BUTTON",
								  "",
								   BS_OWNERDRAW | WS_TABSTOP | WS_CHILD,
								  bsMinimize.x,
								  bsMinimize.y,
								  bsMinimize.width,
								  bsMinimize.height,
								  hwnd,
								  (HMENU)IDC_MINIMIZE,
								  (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE),
								  NULL
								  );


		ShowWindow(hwndMinimize,nCmdShow);
		
		hwndClose = CreateWindow(
								  "BUTTON",
								  "",
								  BS_OWNERDRAW | WS_TABSTOP | WS_CHILD,
								  bsClose.x,
								  bsClose.y,
								  bsClose.width,
								  bsClose.height,
								  hwnd,
								  (HMENU)IDC_CLOSE,
								  (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE),
								  NULL
								  );
		
		ShowWindow(hwndClose,nCmdShow);

		hwndStartGame = CreateWindow(
									"BUTTON",
									"",
									BS_OWNERDRAW | WS_TABSTOP | WS_CHILD,
								    bsStartGame.x,
									bsStartGame.y,
									bsStartGame.width,
									bsStartGame.height,
									hwnd,
									(HMENU)IDC_STARTGAME,
									(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
									NULL
									);
		ShowWindow(hwndStartGame,nCmdShow);


		hwndRegister = CreateWindow(
									"BUTTON",
									"",
									BS_OWNERDRAW | WS_TABSTOP | WS_CHILD,
								    bsRegister.x,
									bsRegister.y,
									bsRegister.width,
									bsRegister.height,
									hwnd,
									(HMENU)IDC_REGISTER,
									(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
									NULL
									);
		ShowWindow(hwndRegister,nCmdShow);	
		
	
		hwndCancel = CreateWindow(
									"BUTTON",
									"",
									BS_OWNERDRAW | WS_TABSTOP | WS_CHILD,
								    bsCancel.x,
									bsCancel.y,
									bsCancel.width,
									bsCancel.height,
									hwnd,
									(HMENU)IDC_CANCEL,
									(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
									NULL
									);
		ShowWindow(hwndCancel,nCmdShow);		
		/*track mouse event*/
		if(!TME(hwnd))
		{
			FreeLibrary(cwebdll);
			PostError();
		}

		/*progress bar*/
		hwndProgress = CreateWindow(PROGRESS_CLASS,
								    (LPSTR)NULL,
									WS_CHILD | WS_VISIBLE | WS_BORDER,
									crdProgress.x,
									crdProgress.y,
									crdProgress.width,
									crdProgress.height,
									hwnd,
									(HMENU)IDC_PROGRESS,
									(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
									NULL
									);

		ShowWindow(hwndProgress,nCmdShow);


	
		/* update all windows before notice window is drawn*/
		UpdateWindow(hwndMinimize);
		UpdateWindow(hwndClose);
		UpdateWindow(hwndStartGame);
		UpdateWindow(hwndRegister);
		UpdateWindow(hwndCancel);
		
		/*draw the notice window*/
		drawNotice(hwnd, nCmdShow);
		

		
		/*message loop*/
		while(GetMessage(&message, NULL, 0, 0))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		FreeLibrary(cwebdll);
		return message.wParam;
} 




/*#################################
## MAIN WINDOW PROCEDURE
#################################*/
LRESULT CALLBACK WndProc(HWND hWnd, 
						 UINT message, 
						 WPARAM wParam, 
						 LPARAM lParam)
{ 

	switch(message)
	{
		case WM_CREATE:
		{
			COORDS crdFrame;
			crdFrame.x = LoadINIInt("frame","xcoord");
			crdFrame.y = LoadINIInt("frame","ycoord");
			crdFrame.width = LoadINIInt("frame","width");
			crdFrame.height = LoadINIInt("frame","height");


			CreateWindow(
				"STATIC",
				"",
				WS_CHILD | WS_VISIBLE | WS_BORDER,
				crdFrame.x,
				crdFrame.y,
				crdFrame.width,
				crdFrame.height,
				hWnd,
				(HMENU)332,
				(HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL
			);


			COORDS crdText;
			crdText.x = LoadINIInt("text","xcoord");
			crdText.y = LoadINIInt("text","ycoord");
			crdText.width = LoadINIInt("text","width");
			crdText.height = LoadINIInt("text","height");

			g_hwndStatic = CreateWindow(
				"STATIC",
				"Status:-----\r\nInfo:-----\r\nProgress:-----",
				WS_CHILD | WS_VISIBLE,
				crdText.x,
				crdText.y,
				crdText.width,
				crdText.height,
				hWnd,
				(HMENU)IDC_STATUS,
				(HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL
			);
			
			HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(g_hwndStatic, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));

			Sleep(1000);			
			
			hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Threader,NULL,0,NULL);
		} 
		break;
/*		case WM_CTLCOLORSTATIC:
        {		

			SetBkMode((HDC)wParam, TRANSPARENT);
		//	SetTextColor((HDC)wParam, RGB(style.iFontColorRED, style.iFontColorGREEN, style.iFontColorBLUE));				
					
			return (LRESULT)CreateSolidBrush(RGB(style.iTextBgRED,style.iTextBgGREEN,style.iTextBgBLUE));
		//	return (LRESULT)GetStockObject(NULL_BRUSH);

        }
		break;*/
		case WM_DRAWITEM:
		{ 
			DRAWITEMSTRUCT *ptr = (DRAWITEMSTRUCT*)lParam;

			if ( wParam == IDC_MINIMIZE )
			{ 
					SetBitmapToButton(ptr->hDC, ptr->hwndItem, hbmMinimize);
			}
			else if( wParam == IDC_CLOSE )
			{
				SetBitmapToButton(ptr->hDC, ptr->hwndItem, hbmClose);
			}

			else if( wParam == IDC_STARTGAME )
			{
				SetBitmapToButton(ptr->hDC, ptr->hwndItem, hbmStartGame);
			}
			else if( wParam == IDC_REGISTER )
			{
				SetBitmapToButton(ptr->hDC, ptr->hwndItem, hbmRegister);
			}
			else if( wParam == IDC_CANCEL )
			{
				SetBitmapToButton(ptr->hDC, ptr->hwndItem, hbmCancel);
			}
		}
		break;
		case WM_PAINT:
		{
			BITMAP bm;
			PAINTSTRUCT ps;

			HDC hdc = BeginPaint(hWnd, &ps);

			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmBackground);
			

			GetObject(hbmBackground, sizeof(bm), &bm);
			BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);

			EndPaint(hWnd, &ps);

		}
		case WM_LBUTTONDOWN:

			SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, NULL);
		break;
		case WM_DESTROY:
			CloseHandle(hThread);
			DeleteObject(hbmBackground);

			DeleteObject(hbmMinimize);
			DeleteObject(hbmMinimize_hover);

			DeleteObject(hbmClose);
			DeleteObject(hbmClose_hover);

			DeleteObject(hbmStartGame);
			DeleteObject(hbmStartGame_hover);

			DeleteObject(hbmRegister);
			DeleteObject(hbmRegister_hover);

			DeleteObject(hbmCancel);
			DeleteObject(hbmCancel_hover);

			PostQuitMessage(0);
		break;
		case WM_COMMAND:
			switch(wParam)
			{
				case IDC_CLOSE:
				{
					int ret;
					if(bPatchInProgress)
					{
						ret = MessageBox(hWnd,"Patch is in progress! Are you sure you want to quit?","Patch in progress",MB_OKCANCEL | MB_ICONQUESTION);
						if(ret == IDOK)
						{
							sprintf(szStatusMessage,"Status: Canceled\r\nInfo:-----\r\nProgress:-----");
							SendMessage(g_hwndStatic,WM_SETTEXT,0,(LPARAM)szStatusMessage);
							TerminateThread(hThread,0);
							SendMessage(hWnd,WM_DESTROY,0,0);
						}
					}
					else
					{
						SendMessage(hWnd,WM_DESTROY,0,0);
					}
				}
				break;
				case IDC_MINIMIZE:
					ShowWindow(hWnd,SW_MINIMIZE);
				break;
				case IDC_STARTGAME:
					if(bPatchCompleted)
					{
						if(ShellExecute(NULL,"open",settings.szExecutable,NULL,NULL,SW_SHOWNORMAL))
							SendMessage(hWnd,WM_DESTROY,0,0);
					}
					else
						MessageBox(hWnd,"Unable to start application. Wait for the patch process to complete","Error",MB_OK | MB_ICONEXCLAMATION);
				break;
				case IDC_CANCEL:
				{
					int ret;
					if(bPatchInProgress)
					{
						ret = MessageBox(hWnd,"Patch is in progress! Are you sure you want to quit?","Patch in progress",MB_OKCANCEL | MB_ICONQUESTION);
						if(ret == IDOK)
						{
							sprintf(szStatusMessage,"Status: Canceled\r\nInfo:-----\r\nProgress:-----");
							SendMessage(g_hwndStatic,WM_SETTEXT,0,(LPARAM)szStatusMessage);
							TerminateThread(hThread,0);
							SendMessage(hWnd,WM_DESTROY,0,0);
						}
					}
					else
					{
						SendMessage(hWnd,WM_DESTROY,0,0);
					}
				}	
				break;

				case IDC_REGISTER:
					/*todo*/
				break;
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}



LRESULT CALLBACK NoticeWindowProcedure(HWND hwndNotice, 
									   UINT message, 
									   WPARAM wParam, 
									   LPARAM lParam)
{
    switch (message)                  
    {
        case WM_CREATE:
    		if ((*lpEmbedBrowserObject)(hwndNotice)) 
				return -1;
            break;
        case WM_DESTROY:
    		(*lpUnEmbedBrowserObject)(hwndNotice);
    		return(TRUE);
		break;
        default: 
            return DefWindowProc(hwndNotice, message, wParam, lParam);
    }

    return 0;
}



BOOL SetupNoticeClass(HINSTANCE hInstance)
{
	WNDCLASSEX wc;

	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc	 = NoticeWindowProcedure;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_3DFACE+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "Notice";
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);
	if(!RegisterClassEx(&wc))
		PostError();
    
	return TRUE;
}


void drawNotice(HWND hwnd, int nCmdShow)
{
	COORDS crdNotice;
	crdNotice.x = LoadINIInt("notice","xcoord");
	crdNotice.y = LoadINIInt("notice","ycoord");
	crdNotice.width = LoadINIInt("notice","width");
	crdNotice.height = LoadINIInt("notice","height");

    hwndNotice = CreateWindowEx(
		0, 
		"Notice", 
		"", 
		WS_CHILD,
		crdNotice.x, 
		crdNotice.y, 
		crdNotice.width, 
		crdNotice.height,
		hwnd, 
		NULL, 
		(HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE), 
		NULL
	);
    
	(*lpDisplayHTMLPage)(hwndNotice, settings.szNoticeURL);		
    ShowWindow(hwndNotice, nCmdShow);
}


int LoadINIInt(LPCSTR section, 
			   LPCSTR key)
{
	static int ret;
	ret = GetPrivateProfileInt(section, key, NULL, styleFile);
	return ret;
}

/*
void __stdcall Juggler	(HINTERNET hInternet, DWORD dwContext,
							 DWORD dwInternetStatus,
							 LPVOID lpvStatusInformation,
							 DWORD dwStatusInformationLength)
{
	REQUEST_CONTEXT *cpContext;
	cpContext= (REQUEST_CONTEXT*)dwContext;

	switch (dwInternetStatus)
	{
		case INTERNET_STATUS_CLOSING_CONNECTION:
	sprintf(szStatusMessage,"Status: Closing connection\r\nInfo:-----\r\nProgress:-----");
	SendMessage(g_hwndStatic,WM_SETTEXT,0,(LPARAM)szStatusMessage);
			break;
		case INTERNET_STATUS_CONNECTED_TO_SERVER:
	sprintf(szStatusMessage,"Status: Connected!\r\nInfo:-----\r\nProgress:-----");
	SendMessage(g_hwndStatic,WM_SETTEXT,0,(LPARAM)szStatusMessage);
			break;
		case INTERNET_STATUS_CONNECTING_TO_SERVER:
	sprintf(szStatusMessage,"Status: Connecting...\r\nInfo:-----\r\nProgress:-----");
	SendMessage(g_hwndStatic,WM_SETTEXT,0,(LPARAM)szStatusMessage);
			break;
		case INTERNET_STATUS_CONNECTION_CLOSED:
	sprintf(szStatusMessage,"Status: Disconnected\r\nInfo:-----\r\nProgress:-----");
	SendMessage(g_hwndStatic,WM_SETTEXT,0,(LPARAM)szStatusMessage);
			break;
		case INTERNET_STATUS_RECEIVING_RESPONSE:
	sprintf(szStatusMessage,"Status: Receiving response...\r\nInfo:-----\r\nProgress:-----");
	SendMessage(g_hwndStatic,WM_SETTEXT,0,(LPARAM)szStatusMessage);
			break;
		case INTERNET_STATUS_RESPONSE_RECEIVED:
	sprintf(szStatusMessage,"Status: Response received...\r\nInfo:-----\r\nProgress:-----");
	SendMessage(g_hwndStatic,WM_SETTEXT,0,(LPARAM)szStatusMessage);
			break;
		case INTERNET_STATUS_REQUEST_COMPLETE:
	sprintf(szStatusMessage,"Status: Request complete\r\nInfo:-----\r\nProgress:-----");
	SendMessage(g_hwndStatic,WM_SETTEXT,0,(LPARAM)szStatusMessage);		
			break;
	}
	
	
}
*/
DWORD Threader(void)
{
	bPatchInProgress = TRUE;
	HINTERNET hRequest;

	g_hOpen = InternetOpen("Agent",
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL,
		NULL,
		0
	);

	if(!g_hOpen)
		return -1;

	g_hConnection = InternetConnect(g_hOpen,
		settings.szPatchURL,
		INTERNET_DEFAULT_HTTP_PORT,
		NULL,
		NULL,
		INTERNET_SERVICE_HTTP,
		0,
		(DWORD)NULL
	);

	if(!g_hConnection)
		return -1;


	/*
	iscCallback = InternetSetStatusCallback(g_hConnection, (INTERNET_STATUS_CALLBACK)Juggler);	
	*/


	HINTERNET hPatch2Request = HttpOpenRequest(g_hConnection,
									"GET",
									settings.szPatchList,
									NULL,
									NULL,
									(const char**)"*/*\0",
									0,
									NULL
							   );
	if(hPatch2Request == NULL)
		return -1;

	else
	{

		HttpSendRequest(hPatch2Request,NULL,0,NULL,0);
		
		DWORD dwPatch2ContentLen;
		DWORD dwPatch2BufLen = sizeof(dwPatch2ContentLen);
		
		//GET CONTENT LENGTH
		if(!HttpQueryInfo(hPatch2Request,HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,(LPVOID)&dwPatch2ContentLen,&dwPatch2BufLen,0))
		{
			TCHAR szMessage[50];
			lstrcpy(szMessage,"Failed to get ");
			lstrcat(szMessage,settings.szPatchList);			
			MessageBox(0,szMessage,"Error",MB_OK);
			bPatchInProgress = FALSE;
			return -1;
			
		}
		//ALLOCATE NEEDED MEMORY
		LPTSTR pPatch2TxtData = (LPTSTR)GlobalAlloc(GMEM_FIXED, dwPatch2ContentLen + 1);
		
		if(NULL == pPatch2TxtData)
			PostError();

		DWORD dwPatch2TxtBytesRead;
		
		//READ THE FILE
		InternetReadFile(hPatch2Request,pPatch2TxtData,dwPatch2ContentLen,&dwPatch2TxtBytesRead);
		
		//NULL TERMINATE
		pPatch2TxtData[dwPatch2TxtBytesRead] = 0;

		//SAVE THE FILE
		DWORD dwBytesWritten_Tmp;
		HANDLE hTmp = CreateFile("tmp.nc",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
   	    if(WriteFile(hTmp,pPatch2TxtData,dwPatch2ContentLen,&dwBytesWritten_Tmp,NULL) == 0)
			PostError();	
		CloseHandle(hTmp);
		
		//FREE MEMORY
		if(GlobalFree((HANDLE)pPatch2TxtData)!=NULL)
			PostError();

		//GET LAST INDEX
		FILE *fpLastIndex;
		UINT last_index;
		
		fpLastIndex = fopen("neoncube.file","r");
		if(!fpLastIndex)
			last_index = 0;
		else
		{
			fscanf(fpLastIndex,"%d",&last_index);
			fclose(fpLastIndex);
		}


		//OPEN FILE FOR READING/PARSING
		UINT index_tmp;
		TCHAR patch_tmp[50];
		FILE *fTmp = fopen("tmp.nc","r");

		if(!fTmp)
			PostError();
		//path to file
		TCHAR file_path[50];
		
		//add main GRF to linked list
		AddPatch(settings.szGrf,1);
		
		while(fscanf(fTmp,"%d %s\n",&index_tmp,&patch_tmp) != EOF)
		{

			if(index_tmp > last_index)
			{
				//add patch_tmp and index_tmp to patch struct
				AddPatch(patch_tmp,index_tmp);
				

				strcpy(file_path,settings.szPatchFolder);
				strcat(file_path,patch_tmp);
				hRequest = HttpOpenRequest(g_hConnection,
										   "GET",
										   file_path,
										   NULL,    
										   NULL,   
										   (const char**)"*/*\0",										
										   0,       
										   NULL);  
				HttpSendRequest(hRequest,
								NULL,    
								0,       
								NULL,   
								0);      

				DWORD dwContentLen;
				DWORD dwBufLen = sizeof(dwContentLen);
				if (HttpQueryInfo(hRequest,
								  HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
								  (LPVOID)&dwContentLen,
								  &dwBufLen,
								  0))
				{
					LPTSTR pData = (LPTSTR)GlobalAlloc(0x0000, dwContentLen + 1);
					if(NULL == pData)
						PostError();

					DWORD dwReadSize = dwContentLen / 100;

					SendMessage(hwndProgress, PBM_SETRANGE, 0,MAKELPARAM(0, 100)); 		
					INT cReadCount;
					DWORD dwBytesRead;
					DWORD dwBytesReadTotal = 0;
					LPTSTR pCopyPtr = pData;

					for (cReadCount = 0; cReadCount < 100; cReadCount++)
					{
								
						InternetReadFile(hRequest, pCopyPtr, dwReadSize, &dwBytesRead);
						pCopyPtr = pCopyPtr + dwBytesRead;
						SendMessage(hwndProgress, PBM_SETPOS, (WPARAM) cReadCount+1, 0);
						dwBytesReadTotal += dwBytesRead;
						sprintf(szStatusMessage,"Status: Downloading %s...\r\nInfo: %.2f KB of %.2f KB downloaded \r\nProgress: %d%%",patch_tmp, BytesToKB(dwBytesReadTotal), BytesToKB(dwContentLen), cReadCount+1);
						SendMessage(g_hwndStatic,WM_SETTEXT,0,(LPARAM)szStatusMessage);						
					}

				
					DWORD dwBytesWritten;
					HANDLE hFile = CreateFile(patch_tmp,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
   					if(WriteFile(hFile,pData,dwContentLen,&dwBytesWritten,NULL) == 0)
						MessageBox(0,"Failed to write data.","Error",MB_OK);
					CloseHandle(hFile);
				/*	
					InternetReadFile(hRequest,
								   pCopyPtr,
								   dwContentLen - (pCopyPtr - pData),
			    				   &dwBytesRead);
					pData[dwContentLen] = 0;
				*/			
					GlobalFree((HANDLE)pData);
				}
				else
				{
					sprintf(szStatusMessage,"Status: Failed to get %s\r\nInfo:-----\r\nProgress:-----",patch_tmp);
					SendMessage(g_hwndStatic,WM_SETTEXT,0,(LPARAM)szStatusMessage);	
					bPatchInProgress = FALSE;
					return -1;
					
				}
						
				bPatchUpToDate = FALSE;		
			}
			else
				bPatchUpToDate = TRUE;
		}
		fclose(fTmp);
		if(!bPatchUpToDate)
		{
									
			PATCH *spCurrentItem;
			spCurrentItem = spFirstItem;

			while(1)
			{
				if(!ExtractGRF(spCurrentItem->szPatchName))
				{
					TCHAR szMess[50] = "Failed to extract ";
					lstrcat(szMess,spCurrentItem->szPatchName);
					MessageBox(NULL,szMess,"Error",MB_OK | MB_ICONERROR);
				}
				if(spCurrentItem->next == NULL)
					break;
				spCurrentItem = spCurrentItem->next;							
			}
			sprintf(szStatusMessage,"Status: Repacking files...\r\nInfo:-----\r\nProgress:-----");
			SendMessage(g_hwndStatic,WM_SETTEXT,0,(LPARAM)szStatusMessage);

			STARTUPINFO siStartupInfo;
			PROCESS_INFORMATION piProcessInfo;

			memset(&siStartupInfo, 0, sizeof(siStartupInfo));
			memset(&piProcessInfo, 0, sizeof(piProcessInfo));

			siStartupInfo.cb = sizeof(siStartupInfo);

			if(!CreateProcess("neoncube\\Create.exe",     
                     NULL,                 
                     0,
                     0,
                     FALSE,
                     CREATE_DEFAULT_ERROR_MODE | CREATE_NO_WINDOW,
                     0,
                     "neoncube",                              
                     &siStartupInfo,
                     &piProcessInfo))
				PostError();
			
			WaitForSingleObject(piProcessInfo.hProcess, INFINITE);
			CloseHandle(piProcessInfo.hThread);
			CloseHandle(piProcessInfo.hProcess);


			FILE *hGrfTxt;
			
			hGrfTxt = fopen("neoncube\\data.grf.txt","w");
			if(!hGrfTxt)
				PostError();
			fprintf(hGrfTxt,"0x103\n");
			fclose(hGrfTxt);
			
			//delete extracted files
			DeleteDirectory("neoncube\\data");
			
			//delete old GRF file
			DeleteFile(settings.szGrf);

			//moves and renames new GRF file
			if(!MoveFile("neoncube\\data.grf",settings.szGrf))
				PostError();

			sprintf(szStatusMessage,"Status: Patch process complete.\r\nInfo:-----\r\nProgress:-----");
			SendMessage(g_hwndStatic,WM_SETTEXT,0,(LPARAM)szStatusMessage);
			
			//write last index
			FILE *hLastIndex;

			hLastIndex = fopen("neoncube.file","w");
			if(NULL == hLastIndex)
				PostError();
			fprintf(hLastIndex,"%d",index_tmp);
			fclose(hLastIndex);								
		}
		else
		{
			sprintf(szStatusMessage,"Status: No new updates.\r\nInfo:-----\r\nProgress:-----");
			SendMessage(g_hwndStatic,WM_SETTEXT,0,(LPARAM)szStatusMessage);
		
		}
		
	}

	bPatchCompleted = TRUE;
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hPatch2Request);
	InternetCloseHandle(g_hConnection);	
	InternetCloseHandle(g_hOpen);
	
	bPatchInProgress = FALSE;
	return S_OK;

}




void AddPatch(const char *item, 
			  INT index)
{
	/* pointer to the next item in the list */
	PATCH *spNewItem;
	PATCH *spLastItem;

	/*if *spFirstItem is NULL, add the new item at the beggining of the linked list*/
	if(spFirstItem == NULL)
	{
		spNewItem = (PATCH*)LocalAlloc(GMEM_FIXED, sizeof(PATCH));
		if(NULL == spNewItem)
			PostError();
		strcpy(spNewItem->szPatchName, item);
		spNewItem->iPatchIndex = index;
		spNewItem->next = spFirstItem;
		spFirstItem = spNewItem;
	}
	/*else add it at the end*/
	else
	{
		spLastItem = spFirstItem;
		
		while(1)
		{
			if(spLastItem->next == NULL)
				break;
			spLastItem = spLastItem->next;
		}
		spNewItem = (PATCH*)LocalAlloc(GMEM_FIXED, sizeof(PATCH));
		if(NULL == spNewItem)
			PostError();
		strcpy(spNewItem->szPatchName, item);
		spNewItem->iPatchIndex = index;
		spNewItem->next = NULL;
		spLastItem->next = spNewItem;
	}
}

void PostError(BOOL exitapp)
{
	LPTSTR szMessageBox = NULL;
	LPTSTR lpszMessage = NULL;
	DWORD dwError = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dwError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        lpszMessage,
        0, NULL 
	);

	sprintf(szMessageBox,"Application error: %s (code: %d)", lpszMessage, dwError);
	MessageBox(NULL,szMessageBox,"Error",MB_OK | MB_ICONERROR);
	LocalFree(lpszMessage);
	LocalFree(szMessageBox);
	if(exitapp)
		ExitProcess(dwError);

}
	
                          