/*

	            SpoutLink
	
	Receive from a sender and send again with default BGRA texture format.

	Used if the format of the sender is incompatible with a receiver
	and must be converted to default BGRA. For example, a DirectX 9
	application requiring BGRA format from an RGBA sender.
	
	The project depends on :

	ofxWinMenu - https://github.com/leadedge/ofxWinMenu
	Spout SDK  - https://github.com/leadedge/Spout2/tree/master/SPOUTSDK/SpoutGL

	The source files of the Spout SDK are not set up as an addon, so they must be copied
	to the "SpoutGL" folder within he Openframeworks folder as follows :

	C:\OPENFRAMEWORKS
		addons
			ofxWinMenu <- download the ofxWinMenu addon and copy to the addons folder
		apps
			SpoutGL <- create this folder (under "apps") and copy the SpoutGL source files into it
			myApps
				SpoutLink <- copy the entire SpoutLink folder here (under "apps\myApps")
					icon.rc
					SpoutLink.sln
					SpoutLink.vcxproj
					SpoutLink.vcxproj.filters
					src <- source files
					bin <- data files

	Open the solution file SpoutLink.sln with Visual Studio 2022
	Set to "Release" "x64" and build "SpoutLink".

	Spout 2.007
	OpenFrameworks 12.0
	Visual Studio 2022
	Copyright (C) 2019-2025 Lynn Jarvis.

	=========================================================================
	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
	=========================================================================

	02.03.20 - Some tidy up - Version 1.001
	02.03.20 - Some changes for potential GitHub release
	29.06.21 - Clean up for distribution - Version 1.002
	04.07.21 - Detect NVIDIA graphics for memory diagnostics
	05.07.21 - Profile and optimize for speed - Version 1.003
	26.10.21 - Update downloads - SpoutLink_6.zip
	06.01.22 - Update Win32/MD with latest SpouGL 9 Version 1.004
	15.01.22 - Update to Openframeworks 12.0 Visual Studio 2022
	16-05-25 - Update with basic functions - version 2.000
	17-05-25 - SpoutPanel inclded in exe folder
			   Check on startup and register if found
			   Version 2.001
	18-05-25 - Check all possibilities for SpoutPanel registration
			   Version 2.002



*/
#include "ofApp.h"

// Help dialog
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

//--------------------------------------------------------------
void ofApp::setup()
{
	ofBackground(0);

	// OpenSpoutConsole(); // For debug
	// EnableSpoutLog();

	// Instance and window handle for the menu
	g_hInstance = GetModuleHandle(NULL);
	g_hWnd = ofGetWin32Window();

	// Set the window title
	ofSetWindowTitle("SpoutLink");

	//
	// Look for SpoutPanel.exe for sender selection
	// to provide sender and system diagnostics.
	// If not already registered, use the file provided
	// with the SpoutLink distributuion

	// Path to SpoutPanel in the executable folder
	std::string spath = GetExePath();
	spath += "SpoutPanel.exe";
	// First, check that the SpoutPanel.exe file exists there
	// In case SpoutLink has been copied to a different location
	bool bLocalFile = false;
	if (_access(spath.c_str(), 0) != -1) {
		bLocalFile = true;
	}

	// Check all possibilities for SpoutPanel registration
	bool bRegistered = false;
	
	// Check the registry for SpoutPanel
	char path[MAX_PATH]{};
	if (ReadPathFromRegistry(HKEY_CURRENT_USER, "Software\\Leading Edge\\SpoutPanel", "InstallPath", path)) {
		// Check that the SpoutPanel.exe file exists there
		if (_access(path, 0) == -1) {
			// If not, change the registry path to the executable folder if SpoutPanel was found there
			if (bLocalFile) {
				WritePathToRegistry(HKEY_CURRENT_USER, "Software\\Leading Edge\\SpoutPanel", "InstallPath", spath.c_str());
				bRegistered = true;
			}
			else {
				// Remove the SpoutPanel registry entry
				RemoveSubKey(HKEY_CURRENT_USER, "Software\\Leading Edge\\SpoutPanel");
			}
		}
		else {
			// SpoutPanel is already registered and exists
			bRegistered = true;
		}
	}
	else {
		// SpoutPanel is not registered
		// Write the registry path of the executable folder if SpoutPanel was found there
		if (bLocalFile) {
			WritePathToRegistry(HKEY_CURRENT_USER, "Software\\Leading Edge\\SpoutPanel", "InstallPath", spath.c_str());
			bRegistered = true;
		}
		// Not registered and no file in the executable folder

	} // Not registered and no backup file found

	// If not registered and no backup file, the "SelectSender"
	// function will open a messagebox with a list of senders

	// Start window size
	int windowWidth  = ofGetWidth();
	int windowHeight = ofGetHeight() + GetSystemMetrics(SM_CYMENU);

	// Resize for a menu
	ofSetWindowShape(windowWidth, windowHeight);

	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth()-windowWidth)/2, (ofGetScreenHeight()-windowHeight)/2);

	// Set a custom window icon
	SetClassLongPtrA(g_hWnd, GCLP_HICON, (LONG_PTR)LoadIconA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDI_ICON1)));

	// Remove maximize button
	DWORD dwStyle = GetWindowLong(g_hWnd, GWL_STYLE);
	SetWindowLong(g_hWnd, GWL_STYLE, dwStyle ^= WS_MAXIMIZEBOX);

	// Load a font rather than the default
	if (!myFont.load("fonts/verdana.ttf", 12, true, true))
		printf("Font not loaded\n");

	// Disable escape key exit
	ofSetEscapeQuitsApp(false);

	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, g_hWnd);
	// Register an ofApp function that is called when a menu item is selected
	menu->CreateMenuFunction(&ofApp::appMenuFunction);
	// Create a menu
	HMENU hMenu = menu->CreateWindowMenu();

	// File
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");
	menu->AddPopupItem(hPopup, "Open", false, false);
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);
	// Window
	hPopup = menu->AddPopupMenu(hMenu, "Window");
	menu->AddPopupItem(hPopup, "Show info", true); // Checked and auto-check
	// Help
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check
	// Set the menu to the window
	menu->SetWindowMenu();

	// De-couple from vsync
	ofSetVerticalSync(false);
	ofSetFrameRate(60);

	// Sender
	sender.SetSenderName("SpoutLink");

} // end setup


//--------------------------------------------------------------
void ofApp::update() {

}

void ofApp::draw() {

	// Do not receive from self
	if (strstr(receiver.GetSenderName(), "SpoutLink") != 0) {
		// Receive from the next in the list if more than one sender
		int n = receiver.GetSenderCount();
		if (n > 1) {
			char sendername[256];
			for (int i=0; i<n; i++) {
				if (receiver.GetSender(i, sendername)) {
					if (strstr(sendername, "SpoutLink") == 0) {
						receiver.SetReceiverName(sendername);
					}
				}
			}
		}
		else {
			return;
		}
	}

	if (receiver.ReceiveTexture(myTexture.getTextureData().textureID, myTexture.getTextureData().textureTarget)) {
		if (receiver.IsUpdated()) {
			myTexture.allocate(receiver.GetSenderWidth(), receiver.GetSenderHeight(), GL_RGBA);
			return;
		}
	}
	else {
		return;
	}

	if (!IsIconic(g_hWnd))
		myTexture.draw(0, 0, ofGetWidth(), ofGetHeight());

	// Send again
	sender.SendTexture(myTexture.getTextureData().textureID,
		myTexture.getTextureData().textureTarget,
		(unsigned int)myTexture.getWidth(), (unsigned int)myTexture.getHeight(), false);

	// On-screen display
	if (bShowInfo && !IsIconic(g_hWnd))
		showInfo();

}

//--------------------------------------------------------------
void ofApp::showInfo() {

	char str[256]{};
	ofSetColor(255);

	if (receiver.IsConnected()) {
		if (strcmp(receiver.GetSenderName(), "SpoutLink") != 0) {
			if (receiver.GetSenderFrame() > 0) {
				sprintf_s(str, 256, "Receiving : [%s] (%dx%d : fps %2.0f)",
					receiver.GetSenderName(),   // name
					receiver.GetSenderWidth(),  // width
					receiver.GetSenderHeight(), // height
					receiver.GetSenderFps());   // fps
			}
			else {
				sprintf_s(str, 256, "Receiving : [%s] (%dx%d)",
					receiver.GetSenderName(),
					receiver.GetSenderWidth(),
					receiver.GetSenderHeight());
			}
			myFont.drawString(str, 10, 20);
			if (!receiver.GetDX9()) {
				sprintf_s(str, 256, "Sending as [%s]",
					sender.GetName());
			}
			else {
				sprintf_s(str, 256, "2.006 DX9 mode incompatible with SpoutLink\nPlease close and open SpoutSettings");
			}
			myFont.drawString(str, 10, 40);
		}
		else {
			sprintf_s(str, 256, "Cannot receive from self. Select another sender.");
			myFont.drawString(str, 10, 20);
		}
		sprintf_s(str, 256, "Right click - select sender : Space - hide/show on-screen display");
		myFont.drawString(str, 50, ofGetHeight()-20);
	}
	else {
		sprintf_s(str, 256, "No sender detected");
		myFont.drawString(str, 10, 20);
		ofDrawBitmapString(str, 10, 20);
	}

}

//--------------------------------------------------------------
void ofApp::exit() {
	receiver.ReleaseReceiver();
	sender.ReleaseSender();
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	if (button == 2) { // rh button
		OpenSender();
	}
}

void ofApp::OpenSender() {
	// Open SpoutPanel for sender selection
	receiver.SelectSender();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == VK_SPACE) {
		bShowInfo = !bShowInfo;
		menu->SetPopupItem("Show info", bShowInfo);
	}
}

//--------------------------------------------------------------
//
// Menu function callback
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(std::string title, bool bChecked) {

	// File menu
	if (title == "Open") {
		OpenSender();
	}

	if (title == "Exit") {
		ofExit(); // Quit the application
	}

	// Window menu
	if (title == "Show info") {
		bShowInfo = bChecked;
	}

	// Help menu
	if (title == "About") {
		DialogBoxA(g_hInstance, MAKEINTRESOURCEA(IDD_ABOUTBOX), g_hWnd, About);
	}

} // end appMenuFunction


// Message handler for About box
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	char tmp[MAX_PATH]{};
	char about[1024]{};
	DWORD dummy = 0;
	DWORD dwSize = 0;
	LPDRAWITEMSTRUCT lpdis{};
	HWND hwnd = nullptr;
	HCURSOR cursorHand = nullptr;
	HINSTANCE hInstance = GetModuleHandle(NULL);

	switch (message) {

	case WM_INITDIALOG:
		sprintf_s(about, 256, "SpoutLink - Version ");
		// Get product version number
		if (GetModuleFileNameA(hInstance, tmp, MAX_PATH)) {
			dwSize = GetFileVersionInfoSizeA(tmp, &dummy);
			if (dwSize > 0) {
				std::vector<BYTE> data(dwSize);
				if (GetFileVersionInfoA(tmp, NULL, dwSize, &data[0])) {
					LPVOID pvProductVersion = NULL;
					unsigned int iProductVersionLen = 0;
					if (VerQueryValueA(&data[0], ("\\StringFileInfo\\080904E4\\ProductVersion"), &pvProductVersion, &iProductVersionLen)) {
						sprintf_s(tmp, MAX_PATH, "%s\n", (char *)pvProductVersion);
						strcat_s(about, 1024, tmp);
					}
				}
			}
		}
		strcat_s(about, 1024, "\n\n");
		strcat_s(about, 1024, "Receive from any sender\n");
		strcat_s(about, 1024, "Send with compatible texture format\n");
		strcat_s(about, 1024, "DXGI_FORMAT_B8G8R8A8_UNORM\n\n");
		SetDlgItemTextA(hDlg, IDC_ABOUT_TEXT, (LPCSTR)about);

		// Hyperlink hand cursor
		cursorHand = LoadCursor(NULL, IDC_HAND);
		hwnd = GetDlgItem(hDlg, IDC_SPOUT_URL);
		SetClassLongPtrA(hwnd, GCLP_HCURSOR, (LONG_PTR)cursorHand);
		break;

	case WM_DRAWITEM:
		// The blue hyperlink
		lpdis = (LPDRAWITEMSTRUCT)lParam;
		if (lpdis->itemID == -1) break;
		SetTextColor(lpdis->hDC, RGB(6, 69, 173));
		switch (lpdis->CtlID) {
			case IDC_SPOUT_URL:
				DrawTextA(lpdis->hDC, "http://spout.zeal.co", -1, &lpdis->rcItem, DT_LEFT);
				break;
			default:
				break;
		}
		break;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDC_SPOUT_URL) {
			sprintf_s(tmp, MAX_PATH, "http://spout.zeal.co");
			ShellExecuteA(hDlg, "open", tmp, NULL, NULL, SW_SHOWNORMAL);
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// .. the end ..


